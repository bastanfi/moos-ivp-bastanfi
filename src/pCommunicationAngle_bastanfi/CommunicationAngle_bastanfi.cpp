/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_bastanfi.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <string>
#include "MBUtils.h"
#include <sstream>
#include "CommunicationAngle_bastanfi.h"

using namespace std;

//global inputs used for calculation purposes.
  double c_nav_x;
  double c_nav_y;
  double c_nav_depth;
  double nav_x;
  double nav_y;
  double nav_depth;

//---------------------------------------------------------
// Constructor

CommunicationAngle_bastanfi::CommunicationAngle_bastanfi()
{
  m_c0 = 1480;
  m_g = 0.016;
  m_depth = 6000;
  m_got_nav_x = false;
  m_got_nav_y = false;
  m_got_nav_depth = false;
  m_got_c_nav_x = false;
  m_got_c_nav_y = false;
  m_got_c_nav_depth = false;
  m_all_inputs = false;
  m_inputs = 0;
}

//---------------------------------------------------------
// Destructor

CommunicationAngle_bastanfi::~CommunicationAngle_bastanfi()
{
}
//---------------------------------------------------------
// calculate Distance

double CommunicationAngle_bastanfi::calcDist(double x1, double x2, double y1, double y2, double z1 = 0, double z2 = 0)
{
double distance = sqrt(pow(x1 - x2,2) + pow(y1 - y2,2) + pow(z1 - z2,2));
return(distance);
}

double CommunicationAngle_bastanfi::calcCircleRadius1(double r, double nav_depth, double c_nav_depth)
{
  double circle_dist = r/2 + (pow((m_c0/m_g)+c_nav_depth,2) - pow((m_c0/m_g)+nav_depth,2))/(2*r);
  double radius = sqrt(pow(circle_dist,2) + pow(m_c0/m_g + nav_depth,2)); 
  return (radius);  
}

double CommunicationAngle_bastanfi::calcAngle(double radius, double nav_depth)
{
  double cz_0 = m_c0 + m_g * nav_depth;
  double theta = acos(cz_0 / (m_g * radius));
  return(theta);
}
//---------------------------------------------------------
// 

double CommunicationAngle_bastanfi::calcCircleRadius2(double nav_depth, double theta)
{
double cz_0 = m_c0 + m_g * nav_depth;
double radius = cz_0 / (m_g * cos(theta));\
return(radius);
}

double CommunicationAngle_bastanfi::calcArcLength(double dist_3d, double radius)
{
  double s = 2*radius*asin(dist_3d/(2*radius));
  return(s);
}

double CommunicationAngle_bastanfi::calcDist2(double theta, double radius, double s)
{
  double r = radius * (sin(theta)+sin(s/radius-theta));
  return(r);
}

double CommunicationAngle_bastanfi::calcAngle2(double s, double radius, double angle_0)
{
  double angle_s = - (s / radius - angle_0);
  return(angle_s);
}

double CommunicationAngle_bastanfi::calcTL(double J, double theta_0, double c_zs, double cz_0)
{
  double p_s = 1/(4*M_PI)* sqrt(abs(c_zs/cz_0*cos(theta_0)/J));
  double p_1 = 1/(4*M_PI);
  double tl = -20*log10(p_s/p_1);
  return(tl);
}

std::string CommunicationAngle_bastanfi::calculate(double c_nav_x,double nav_x,double c_nav_y, double nav_y, double c_nav_depth, double nav_depth)
{
  //Calculate the Grazing Angle
  double r_0 = calcDist(c_nav_x,nav_x,c_nav_y,nav_y);
  double radius_0 = calcCircleRadius1(r_0, nav_depth, c_nav_depth); 
  double theta_0 = calcAngle(radius_0,nav_depth);
  double grazing_angle = -theta_0 * 180/M_PI;
  //Calculate Transmission Loss
  double c_zs = m_c0 + m_g * c_nav_depth;
  double d_theta = 0.00001;
  double theta_1 = theta_0 + d_theta;
  double radius_1 = calcCircleRadius2(nav_depth, theta_1);
  double dist_3d = calcDist(c_nav_x,nav_x,c_nav_y, nav_y, c_nav_depth, nav_depth);
  double s = calcArcLength(dist_3d, radius_0);
  double r_1 = calcDist2(theta_1,radius_1,s);
  double dr_dtheta = (r_1 - r_0)/(theta_1 - theta_0);
  double theta_s = calcAngle2(s, radius_0, theta_0);
  double J = r_0/sin(theta_s)*dr_dtheta;
  double cz_0 = m_c0 + m_g * nav_depth;
  double tl = calcTL(J, theta_0, c_zs, cz_0);
  //boolean function that determines if there is a transmission pass
  bool connected = canConnect(cz_0, theta_0, radius_0, s);
  //if connected, Outputs CONNECTIVITY LOCATION as current position and returns grazing angle output
  if(connected){
    Notify("CONNECTIVITY_LOCATION","x=" + doubleToString(nav_x,0) + ",y=" + doubleToString(nav_y,0) + ",depth=" + doubleToString(nav_depth,0) + ",userid=bastanfi@mit.edu");
    return("elev_angle=" + doubleToString(grazing_angle,4)+",transmission_loss="+doubleToString(tl,4)+",userid=bastanfi@mit.edu");
    }
  //if not connected, calculate a new location
  else{
    //shortest straight line path is to go to the center of the circle that is created by the collaborator
    //and is tangent to the maximum depth... the following calculation does that.
    double theta_new = acos(cz_0 / (m_g * m_depth + m_c0));
    double radius_new = cz_0 / (m_g * cos(theta_new));
    double theta_s_new = acos(c_zs/cz_0 * cos(theta_new));
    double alpha_new = M_PI / 2 - theta_s_new;
    double base = r_0 - radius_new * cos(alpha_new);
    double height = nav_depth + m_c0 / m_g;
    //hyp is the straight line distance from the current position, to the center of the circle that creates the
    //optimum radius for the sound speed to reach the collaborator.
    double hyp = sqrt(pow(base,2) + pow(height,2));
    //calculate the distance traveled required along the hyp to get to that optimum radius.
    double dist_travel = hyp - radius_new;
    //distance traveled along the r-axis
    double dist_travel_atdepth = dist_travel * base / hyp;
    //new nav depth to obtain
    double new_nav_depth = nav_depth - height / hyp * dist_travel;
    //convert distance traveled along r-axis to x and y.
    double new_nav_x = nav_x + dist_travel_atdepth * (c_nav_x - nav_x)/r_0;
    double new_nav_y = nav_y + dist_travel_atdepth * (c_nav_y - nav_y)/r_0;
    //if the new nav depth calculated is less then 0, determine the new location by only changing the x and y position.
    if(new_nav_depth < 0){
      double theta_s_new = acos(c_zs / (m_g * m_depth + m_c0));
      double s_new = radius_new * (theta_new - theta_s_new);
      double r_new = radius_new * (sin(theta_new) + sin(s_new/radius_new - theta_new));
      double dist_travel = r_0 - r_new;
      double dist_travel_x = dist_travel * (c_nav_x - nav_x)/r_0;
      double dist_travel_y = dist_travel * (c_nav_y - nav_y)/r_0;
      double new_nav_x = nav_x + dist_travel_x;
      double new_nav_y = nav_y + dist_travel_y;
    }
    Notify("CONNECTIVITY_LOCATION", "x = " + doubleToString(new_nav_x,0) + ",y=" + doubleToString(new_nav_y,0) + ",depth=" + doubleToString(new_nav_depth,0) + ",userid=bastanfi@mit.edu");
    return("NaN");
  }
}

bool CommunicationAngle_bastanfi::canConnect(double cz_0, double theta_0, double radius_0, double s)
{
  double z_max = cz_0/(m_g * cos(theta_0)) - m_c0/m_g;
  if (z_max > m_depth){
    double s_z_max = radius_0 * theta_0;
    if (s_z_max > s)
      Notify("TROUBLESHOOT",doubleToString(z_max));
      return(false);
    }
  return(true);
}
//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle_bastanfi::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;


  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string c_name;
    string key = msg.GetKey();
    if(key == "COLLABORATOR_NAME"){
      c_name = msg.GetString(); 
      m_s_c_nav_x = c_name + "_NAV_X";
      m_s_c_nav_y = c_name + "_NAV_Y";
      m_s_c_nav_depth = c_name + "_NAV_DEPTH";
      Register(m_s_c_nav_x,0);
      Register(m_s_c_nav_y,0);
      Register(m_s_c_nav_depth,0);
    }
    if(key == m_s_c_nav_x){
      c_nav_x  = msg.GetDouble();
      m_got_c_nav_x = true;
    }
    if(key == m_s_c_nav_y){
      c_nav_y = msg.GetDouble();
      m_got_c_nav_y = true;
    }
    if(key == m_s_c_nav_depth){
      c_nav_depth = msg.GetDouble();
      m_got_c_nav_depth = true;
    }
    if(key == "NAV_X"){
      nav_x = msg.GetDouble();
      m_got_nav_x = true;
    }
    if(key == "NAV_Y"){
      nav_y = msg.GetDouble();
      m_got_nav_y = true;
    }
    if(key == "NAV_DEPTH"){
      nav_depth = msg.GetDouble();
      m_got_nav_depth = true;
    }
    m_inputs = m_got_c_nav_x + m_got_c_nav_y + m_got_c_nav_depth + m_got_nav_x + m_got_nav_y + m_got_nav_depth;
    if (m_inputs == 6)
      m_all_inputs = true;
    if (m_all_inputs){
      std::string grazing_angle = calculate(c_nav_x, nav_x, c_nav_y, nav_y, c_nav_depth, nav_depth);
      Notify("ACOUSTIC_PATH",grazing_angle);
    }
    
#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle_bastanfi::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle_bastanfi::Iterate()
{  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle_bastanfi::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle_bastanfi::RegisterVariables()
{
  Register("COLLABORATOR_NAME",0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
  Register("NAV_DEPTH",0);
}
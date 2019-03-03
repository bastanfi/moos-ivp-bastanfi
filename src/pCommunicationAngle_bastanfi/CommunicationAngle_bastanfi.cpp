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

  string c_name;
  string m_s_c_nav_x;
  string m_s_c_nav_y;
  string m_s_c_nav_depth;

  bool got_nav_x = false;
  bool got_nav_y = false;
  bool got_nav_depth = false;
  bool got_c_nav_x = false;
  bool got_c_nav_y = false;
  bool got_c_nav_depth = false;
  bool all_inputs = false;
  int inputs;


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
}

//---------------------------------------------------------
// Destructor

CommunicationAngle_bastanfi::~CommunicationAngle_bastanfi()
{
}
//---------------------------------------------------------
// calculate Distance

double CommunicationAngle_bastanfi::calcDist(double x1, double x2, double y1, double y2, double z1 = 0, double z2 = 9)
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

double CommunicationAngle_bastanfi::calcTL(double J, double theta_0, double c_zs)
{
  double p_s = 1/(4*M_PI)* sqrt(abs(c_zs/m_c0*cos(theta_0)/J));
  double p_1 = 1/(4*M_PI);
  double tl = -20*log10(p_s/p_1);
  return(tl);
}

std::string CommunicationAngle_bastanfi::calculate(double c_nav_x,double nav_x,double c_nav_y, double nav_y, double c_nav_depth, double nav_depth)
{
  double r_0 = calcDist(c_nav_x,nav_x,c_nav_y,nav_y);
  double radius_0 = calcCircleRadius1(r_0, nav_depth, c_nav_depth); 
  double theta_0 = calcAngle(radius_0,nav_depth);
  double grazing_angle = -theta_0 * 180/M_PI;
  double c_zs = m_c0 + m_g * c_nav_depth;
  double d_theta = 0.001;
  double theta_1 = theta_0 + d_theta;
  double radius_1 = calcCircleRadius2(nav_depth, theta_1);
  




  double dist_3d = calcDist(c_nav_x,nav_x,c_nav_y, nav_y, c_nav_depth, nav_depth);
  double s_1 = calcArcLength(dist_3d, radius_1);
  double r_1 = calcDist2(theta_1,radius_1,s_1);
  double dr_dtheta = (r_1 - r_0)/(theta_1 - theta_0);
  double s = 2*radius_0*asin(r_0/(2*radius_0));
  double theta_s = calcAngle2(s, radius_0, theta_0);
  double J = r_0/sin(theta_s)*dr_dtheta;
  double tl = calcTL(J, theta_0, c_zs);
  double cz_0 = m_c0 + m_g * nav_depth;
  bool connected = canConnect(cz_0, theta_0, radius_0, s);
  if(connected){
    Notify("CONNECTIVITY_LOCATION","x=" + doubleToString(nav_x) + ",y=" + doubleToString(nav_y) + ",depth=" + doubleToString(nav_depth) + "id=bastanfi@mit.edu");
    return("elecation_angle=" + doubleToString(grazing_angle)+",transmission_loss="+doubleToString(tl)+",userid=bastanfi@mit.edu");
    }
  else{
    double theta_new = acos(cz_0 / (m_g * m_depth + m_c0));
    double radius_new = cz_0 / (m_g * cos(theta_new));
    // double theta_s_new = acos(c_zs / (m_g * m_depth + m_c0));
    


    // double s_new = radius_new * (theta_new - theta_s_new);
    // double r_new = radius_new * (sin(theta_new) + sin(s_new/radius_new - theta_new));
    // double dist_travel = r_0 - r_new;
    // double dist_travel_x = dist_travel * (c_nav_x - nav_x)/r_0;
    // double dist_travel_y = dist_travel * (c_nav_y - nav_y)/r_0;
    // double new_nav_x = nav_x + dist_travel_x;
    // double new_nav_y = nav_y + dist_travel_y;


    double theta_s_new = acos(c_zs/cz_0 * cos(theta_new));
    double alpha_new = M_PI / 2 - theta_s_new;
    double base = r_0 - radius_new * cos(alpha_new);
    double height = nav_depth + m_c0 / m_g;
    double hyp = sqrt(pow(base,2) + pow(height,2));
    double dist_travel = hyp - radius_new;
       double dist_travel_atdepth = dist_travel * base / hyp;
       double new_nav_depth = nav_depth - height / hyp * dist_travel;
       double new_nav_x = nav_x + dist_travel_atdepth * (c_nav_x - nav_x)/r_0;
       double new_nav_y = nav_y + dist_travel_atdepth * (c_nav_y - nav_y)/r_0;


    Notify("CONNECTIVITY_LOCATION", "x = " + doubleToString(new_nav_x) + ",y=" + doubleToString(new_nav_y) + ",depth=" + doubleToString(new_nav_depth));
    return("NaN");
  }
}

bool CommunicationAngle_bastanfi::canConnect(double cz_0, double theta_0, double radius_0, double s)
{
  double z_max = cz_0/(m_g * cos(theta_0)) - m_c0/m_g;
  if (z_max > m_depth){
    double s_z_max = radius_0 * theta_0;
    if (s_z_max < s)
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
      got_c_nav_x = true;
    }
    if(key == m_s_c_nav_y){
      c_nav_y = msg.GetDouble();
      got_c_nav_y = true;
    }
    if(key == m_s_c_nav_depth){
      c_nav_depth = msg.GetDouble();
      got_c_nav_depth = true;
    }
    if(key == "NAV_X"){
      nav_x = msg.GetDouble();
      got_nav_x = true;
    }
    if(key == "NAV_Y"){
      nav_y = msg.GetDouble();
      got_nav_y = true;
    }
    if(key == "NAV_DEPTH"){
      nav_depth = msg.GetDouble();
      got_nav_depth = true;
    }
    inputs = got_c_nav_x + got_c_nav_y + got_c_nav_depth + got_nav_x + got_nav_y + got_nav_depth;
    if (inputs == 6)
      all_inputs = true;
    if (all_inputs){
      std::string grazing_angle = calculate(c_nav_x, nav_x, c_nav_y, nav_y, c_nav_depth, nav_depth);
    // double s = 2*radius * asin(distance/(2*radius));
    // double theta_f = theta - s/radius;
    Notify("TROUBLESHOOT",grazing_angle);
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






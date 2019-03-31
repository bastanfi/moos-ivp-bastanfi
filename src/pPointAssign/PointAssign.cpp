/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h"
#include <iostream>
#include <string>
#include "XYPoint.h"

using namespace std;

string troubleshoot;
//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
  m_vehicle_1_assigned = false;
  m_vehicle_2_assigned = false;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    if(key =="NODE_REPORT"){
      if(!m_vehicle_1_assigned){
        string sval = msg.GetString();
        m_vehicle_1_assigned = AssignVehicleName1(sval);
      }
      if(!m_vehicle_2_assigned){
        string sval = msg.GetString();
        m_vehicle_2_assigned = AssignVehicleName2(sval);
      }
    }
    if(key == "VISIT_POINT"){
      string sval = msg.GetString();
      if(assign_by_region){
        ArrangeByRegion(sval);
      }
      else{
        ArrangeByNumber(sval);
      }
    }
    

#if 0 // Keep these around just for template
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
// Procedure: ArrangeByNumber
bool PointAssign::AssignVehicleName1(std::string sval)
{
  m_vname1 = "VISIT_POINT_" + toupper(tokStringParse(sval,"NAME",',','=')); 
  m_x1 = stod(tokStringParse(sval,"X",',','='));
  string color_v1 = "COLOR_" + toupper(tokStringParse(sval,"NAME",',','='));
  Notify(color_v1,"red");
  Register(m_vname1,0);
  return(true);
}

//---------------------------------------------------------
// Procedure: ArrangeByNumber
bool PointAssign::AssignVehicleName2(std::string sval)
{
  string vehicle_name = "VISIT_POINT_" + toupper(tokStringParse(sval,"NAME",',','='));
  if(vehicle_name==m_vname1)
    return(false);
  else{
    m_vname2 = vehicle_name;
    Register(m_vname2,0);
    string color_v1 = "COLOR_" + toupper(tokStringParse(sval,"NAME",',','='));
     m_x2 = stod(tokStringParse(sval,"X",',','='));
     if(m_x1>m_x2)
       m_vname1.swap(m_vname2);  
       Notify(color_v1,"yellow");
    return(true);
  }
}

//---------------------------------------------------------
// Procedure: ArrangeByNumber
bool PointAssign::ArrangeByNumber(std::string sval)
{
  if(sval=="firstpoint"){
    Notify(m_vname1,sval);
    Notify(m_vname2,sval);
  }
  else if(sval=="lastpoint"){
    Notify(m_vname1,sval);
    Notify(m_vname2,sval);
  }
  else{
    string sval_saved = sval;
    m_label = tokStringParse(sval,"id",',','=');
    m_x = stod(tokStringParse(sval,"x",',','='));
    m_y = stod(tokStringParse(sval,"y",',','='));
    int id_num = stoi(m_label);

    if(id_num%2==0){
      Notify(m_vname1,sval_saved);
      m_color = "red";
    }
    if(id_num%2!=0){
      Notify(m_vname2,sval_saved);
      m_color = "yellow";
    }
    postViewPoint();
  }
  return(true);
}

bool PointAssign::ArrangeByRegion(std::string sval)
{
  double middle = 87.5;
    if(sval=="firstpoint"){
    Notify(m_vname1,sval);
    Notify(m_vname2,sval);
  }
  else if(sval=="lastpoint"){
   Notify(m_vname1,sval);
   Notify(m_vname2,sval);
  }
  else{
    m_x = stod(tokStringParse(sval,"x",',','='));
    m_y = stod(tokStringParse(sval,"y",',','='));
    if(m_x < middle){
      Notify(m_vname1,sval);
      m_color = "red";
    }
    if(m_x > middle){
      Notify(m_vname2,sval);
      m_color = "yellow";
    }
    m_label = tokStringParse(sval,"id",',','=');
    postViewPoint();
  }
  return(true);
}

void PointAssign::postViewPoint()
{
  XYPoint point (m_x,m_y);
  point.set_label(m_label);
  point.set_color("vertex",m_color);
  point.set_param("vertex_size","2");
  std::string spec = point.get_spec();
  Notify("VIEW_POINT",spec);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      if(param == "assign_by_region"){
        if(tolower(value) == "false")
          assign_by_region = false;
        else
          assign_by_region = true;
      }

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
// Procedure: registerVariables

void PointAssign::RegisterVariables()
{
  Register("VISIT_POINT",0);
  Register("NODE_REPORT",0);
}






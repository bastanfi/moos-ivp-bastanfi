/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"
#include "XYSegList.h"
#include <string>
#include <iostream>
#include <utility>
#include <functional>
#include <list>


using namespace std;

struct DistanceComparator
{
  bool operator ()(const Positions & Position1, const Positions & Position2)
  {
    return(Position1.distance < Position2.distance);
  }
};

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_gotOrig = false;
  m_old_hit = 0;
  m_new_hit = 0;
  m_old_index = 0;
  m_new_index = 0;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    if(key=="WPT_RECALCULATE"){
      string sval = msg.GetString();
      if(sval=="true")
      {
        int l = next_positions.size();
        if(l==0)
        {
          Notify("RETURN","true");
          continue;
        }
        old_positions = next_positions;
        CalcDistances(m_x_orig,m_y_orig);
        GraphLines();
        it1 = next_positions.begin();
        Notify("STATION_KEEP","false");
        Notify("WPT_SEEK","true");
      }
    }
    if(key=="NODE_REPORT_LOCAL"){
      if(!m_gotOrig){
        string sval = msg.GetString();
        m_x_orig = stod(tokStringParse(sval,"X",',','='));
        m_y_orig = stod(tokStringParse(sval,"Y",',','='));
        m_gotOrig=true;
      }
    }
    if(key=="COLOR"){
      m_color = msg.GetString();
      Notify("UPDATE_PTS","visual_hints = edge_color = " + m_color);
    }
    if(key=="VISIT_POINT"){
      string sval  = msg.GetString();
      if(sval=="lastpoint"){
        CalcDistances(m_x_orig,m_y_orig);
        GraphLines();
        continue;
      }
      if(sval=="firstpoint")
        continue;
      Positions received_positions;
      received_positions.m_positions_x = stod(tokStringParse(sval,"x",',','='));
      received_positions.m_positions_y = stod(tokStringParse(sval,"y",',','='));
      received_positions.m_points = sval;
      old_positions.push_back(received_positions);
     }
    if(key=="WPT_STAT"){
      string sval  = msg.GetString();
      string behavior_name = tokStringParse(sval,"behavior-name",',','=');
      if(sval=="behavior-name=wpt_seek,completed"||behavior_name!="wpt_seek"){
        continue;
      }
      m_new_index = stoi(tokStringParse(sval,"index",',','='));
      string delimiter = "/";
      string new_hit = tokStringParse(sval,"hits",',','=');
      string new_hit1 = new_hit.substr(0,new_hit.find(delimiter));
      m_new_hit = stoi(new_hit1);
      string s = tokStringParse(sval,"hits",',','=');
      Notify("TROUBLESHOOT",new_hit1);
      if(m_new_index!=m_old_index){
        if(m_new_hit==m_old_hit){
          ++it1;   
        }
        else{
          it1 = next_positions.erase(it1);
        }
        m_old_hit = m_new_hit;
      }
      m_old_index = m_new_index;
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

bool GenPath::CalcDistances(double x, double y)
{
  int length = old_positions.size();
  int i = 0;
  new_positions.clear();
  for(i = 0; i < length;i++){
    for(list<Positions>::iterator iter = old_positions.begin(); iter!=old_positions.end();){
      Positions& current_position = *iter;
      current_position.distance = pow(current_position.m_positions_x-x,2) + pow(current_position.m_positions_y-y,2);  
      ++iter;
    }
    old_positions.sort(DistanceComparator());
    Positions current_position = old_positions.front();
    new_positions.push_back(current_position);
    x = current_position.m_positions_x;
    y = current_position.m_positions_y;
    old_positions.pop_front();
  }
  next_positions = new_positions;
  it1 = next_positions.begin();
  return(true);
}

bool GenPath::GraphLines()
{
  XYSegList my_seglist;
  my_seglist.clear();
  double x_orig = m_x_orig;
  double y_orig = m_y_orig;
  string update_str = "points = pts = {" ;
  int i = 0;
  int length = new_positions.size();
  for(list<Positions>::iterator iter = new_positions.begin(); iter!=new_positions.end();){
    Positions& current_position = *iter;
    i = i + 1;
    double x_val = current_position.m_positions_x;
    double y_val = current_position.m_positions_y;
    update_str = update_str + to_string(x_val)+ "," + to_string(y_val);
    my_seglist.add_vertex(x_val,y_val);
    if(i != length)
      update_str = update_str + ":";
    ++iter;
  }
  update_str = update_str + "}";
  Notify("UPDATE_PTS",update_str); 
  Notify("TROUBLESHOOT",update_str);
  return(true);
}



//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
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

bool GenPath::Iterate()
{

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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

void GenPath::RegisterVariables()
{
Register("NODE_REPORT_LOCAL",0);
Register("NAV_X",0);
Register("NAV_Y",0);
Register("VISIT_POINT",0);
Register("UPDATES_VAR",0);
Register("COLOR",0);
Register("WPT_REACHED",0);
Register("WPT_INDEX",0);
Register("WPT_STAT",0);
Register("WPT_RECALCULATE",0);
Register("UPDATE_PTS",0);
Notify("UTS_PAUSE","false");
}


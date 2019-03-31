/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class PointAssign : public CMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   bool ArrangeByRegion(std::string sval);
   bool ArrangeByNumber(std::string sval);
   bool AssignVehicleName1(std::string sval);
   bool AssignVehicleName2(std::string sval);
   void postViewPoint();

 protected:
   void RegisterVariables();
   
   bool assign_by_region;
   bool m_vehicle_1_assigned;
   bool m_vehicle_2_assigned;
   std::string m_vname1;
   std::string m_vname2;
   std::string m_color;
   std::string m_label;
   double m_x;
   double m_y;
   double m_x1;
   double m_x2;


 private: // Configuration variables

 private: // State variables
};

#endif 

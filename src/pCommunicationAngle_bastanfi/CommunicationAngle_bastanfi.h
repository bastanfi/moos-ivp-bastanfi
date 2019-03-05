/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle_bastanfi.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_bastanfi_HEADER
#define CommunicationAngle_bastanfi_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iterator>
#include <string>
#include "MBUtils.h"
#include <math.h>
#include <vector>
#include <list>
#include <iostream>
#include <numeric>


class CommunicationAngle_bastanfi : public CMOOSApp
{
 public:
   CommunicationAngle_bastanfi();
   ~CommunicationAngle_bastanfi();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   double calcDist(double x1, double x2, double y1, double y2, double z1, double z2);
   double calcCircleRadius1(double r, double nav_depth, double c_nav_depth);
   std::string calculate(double c_nav_x,double nav_x, double c_nav_y, double nav_y, double c_nav_depth, double nav_depth);
   double calcAngle(double radius, double nav_depth);
   double calcCircleRadius2(double nav_depth, double theta);
   double calcArcLength(double dist_3d, double radius);
   double calcDist2(double theta, double radius, double s);
   double calcAngle2(double s, double radius, double angle_0);
   double calcTL(double J,double theta_0,double c_zs, double cz_0);
   
   bool canConnect(double cz_0, double theta_0, double radius_0, double s);
   //bool pathExist(double nav_depth, double theta, double c_nav_depth);

   std::string m_s_c_nav_x;
   std::string m_s_c_nav_y;
   std::string m_s_c_nav_depth;
   bool m_got_nav_x;
   bool m_got_nav_y;
   bool m_got_nav_depth;
   bool m_got_c_nav_x;
   bool m_got_c_nav_y;
   bool m_got_c_nav_depth;
   bool m_all_inputs;
   int m_inputs;

   double m_c0;
   double m_g;
   double m_depth;
 private: // Configuration variables
 
 private: // State variables
};

#endif 

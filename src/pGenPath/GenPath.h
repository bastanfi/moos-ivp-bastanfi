/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <list>
#include "Positions.h"
#include <math.h>


class GenPath : public CMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   bool CalcDistances(double x, double y);
   bool CompareDistance(const double& dist1, const double& dist2);
   bool GraphLines();
   bool m_wpt_reached;


 protected:
    void RegisterVariables();
    std::list<double> m_old_position_x;
    std::list<double> m_old_position_y;
    std::list<Positions> old_positions;
    std::list<Positions> new_positions;
    std::list<Positions> next_positions;
    bool m_gotOrig;
    std::string m_color;

    double m_x_orig;
    double m_y_orig;
    double m_x;
    double m_y;
    int m_new_hit;
    int m_old_hit;
    int m_new_index;
    int m_old_index;
    std::list<Positions>::iterator it1;

 private: // Configuration variables

 private: // State variables
};

#endif 

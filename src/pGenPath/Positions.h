/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: Positions.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Positions_HEADER
#define Positions_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <list>
#include "GenPath.h"

class Positions 
{
 public:
   Positions();
   virtual ~Positions() {};

  double m_positions_x;
  double m_positions_y;
  double distance;
  std::string m_points;
 protected: // Standard MOOSApp functions to overload  



 protected:

 private: // Configuration variables

 private: // State variables
};

#endif 

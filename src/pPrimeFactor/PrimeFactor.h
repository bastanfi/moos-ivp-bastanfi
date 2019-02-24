/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <string>
#include <cstdint>
#include <list>
#include <vector>
#include "PrimeEntry.h"

using namespace std;

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   virtual ~PrimeFactor() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   list<PrimeEntry> m_primes;
   int m_calc; //similar to m_index, this is a PrimeFactor variable that has to be
   //initialized to be problerly indexed in the iterate loop
 private: // Configuration variables
 private: // State variables
};
#endif 

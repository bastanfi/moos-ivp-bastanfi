/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include "PrimeEntry.h"
#include <string>
#include <cstdint>
#include <list>
#include <math.h>
#include <sstream>
#include <iterator>

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{ 
  list<PrimeEntry> m_primes; //NOT necessary
  m_calc = 0; //necessary: 
  m_rec = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail
// Purpose: checks for new mail which are type NUM_VALUE and sets m_orig, m_received, m_start_time
bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  int received_index;
  for(p=NewMail.begin(); p!=NewMail.end(); p++){
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    if(key == "NUM_VALUE"){
      string sval  = msg.GetString();
      uint64_t orig =  strtoul(sval.c_str(),NULL,0);
      m_rec = m_rec + 1;
      PrimeEntry new_prime;
      new_prime.setReceivedIndex(m_rec);
      new_prime.setOriginalVal(orig);
      new_prime.m_start_index = MOOSTime();
      new_prime.m_current = orig;
      m_primes.push_back(new_prime);
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  for( list<PrimeEntry>::iterator iter= m_primes.begin(); iter != m_primes.end();){
    PrimeEntry& current_prime = *iter;
    current_prime.setDone(current_prime.factor(100000));
    if(current_prime.m_done==true){
      m_calc = m_calc + 1;
      current_prime.setCalculatedIndex(m_calc);
      Notify("PRIME_RESULT",current_prime.getReport());
      iter = m_primes.erase(iter); 
    }
    else 
      ++iter;
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)){
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++){
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
    }
  } 
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0);
}

//-------------------------------------------------------------

PrimeEntry::PrimeEntry()
{
//only m_iterations is required to be initialized. Needs to start at 3 to properly run through the factor funciton
  m_orig = 0; //NOT necessary
  max_steps = 0; //NOT necessary
  std::vector<uint64_t> m_factors; //NOT necessary
  m_received_index = 0; //NOT necessary
  m_calculated_index = 0; //NOT necessary
  m_start_index = 0; //NOT necessary
  m_done = false; //NOT necessary
  m_current = 0; //NOT necessary
  m_iterations = 3; //necessary
}

//-------------------------------------------------------------
// Procedure:factor
// Purpose:to perform max_steps number of iterations calculating the prime factors and placing them,
//  the iteration step in the for loop it is on as well as the updated factor 
bool PrimeEntry::factor(unsigned long int max_steps)
{
  unsigned long int iter = 0;
  while(m_current%2 ==0){
    m_factors.push_back(2);
    m_current = m_current/2;
    iter = iter+1;
    if(iter >= max_steps)
      return(false);
  }
  for (uint64_t i = m_iterations; i<= sqrt(m_current); i = i+2){\
    iter = iter+1;
    m_iterations = m_iterations+2;
    if(iter >= max_steps)
      return(false);
    while(m_current%i==0){
      m_factors.push_back(i);
      m_current = m_current/i; 
    }
  }
  if (m_current>2)
    m_factors.push_back(m_current);  
  return(true);
}

//-------------------------------------------------------------
// Procedure:getReport
// Purpose: Generates a string output for PRIME_RESULT from the class PrimeEntry
std::string PrimeEntry::getReport()
{
  stringstream ss_orig, ss_primes;
  ss_orig << m_orig;
  string s_orig = "orig=" + ss_orig.str();
  string s_rec = + ",received="+intToString(m_received_index);
  string s_calc = ",calculated=" + intToString(m_calculated_index);
  string s_solve_time = ",solve_time=" + doubleToString(MOOSTime() - m_start_index,3);
  copy(m_factors.begin(),m_factors.end(), ostream_iterator<uint64_t>(ss_primes, ":"));
  string s_primes = ss_primes.str();
  s_primes = ",primes=" + s_primes.substr(0, s_primes.length()-1);  
  string s_user = ",username=bastanfi";
  string string_out = s_orig + s_rec + s_calc + s_solve_time+s_primes + s_user;
  return(string_out);
}



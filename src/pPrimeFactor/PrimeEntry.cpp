/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                        */
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



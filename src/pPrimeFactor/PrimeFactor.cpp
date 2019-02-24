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
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++){
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();
    if(key == "NUM_VALUE"){ //Purpose: checks for NUM_VALUE and sets m_orig, m_received, m_start_time
      string sval  = msg.GetString();
      uint64_t orig =  strtoul(sval.c_str(),NULL,0);
      int received_index;
      received_index = received_index + 1;
      PrimeEntry new_prime;
      new_prime.setReceivedIndex(received_index);
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
//stores the working PrimeEntry class as the variable current prime.
    PrimeEntry& current_prime = *iter;
//perform 100 iterations of the factor function... if it fully calculates, it will set the boolean
//m_done as true.
    current_prime.setDone(current_prime.factor(100));
//if m_done is true, the all prime factors have been determined and will undergo the following
    if(current_prime.m_done==true){
  //iterates to the next calculated index and stores as m_calculated_index in current_prime
      m_calc = m_calc+1;
      current_prime.setCalculatedIndex(m_calc);
  //determines output string using the function getReport and NOTIFIES 
      Notify("PRIME_RESULT",current_prime.getReport());
  //erases completed index of m_primes
      iter = m_primes.erase(iter); 
    }
    else //if calculation is not done, iterate to the next number to calculate prime factors.
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
bool PrimeEntry::factor(unsigned long int max_steps)
{
//establishes the iteration tracker for the factor function
  unsigned long int iter = 0;
//while m_current is divisible by 2, it stores 2 as a prime factor and icrementally increases the iterations
  while(m_current%2 ==0){
    m_factors.push_back(2);
    m_current = m_current/2;
    iter = iter+1;
    //if the iterations reach the max step, return a false and exit this function
    if(iter >= max_steps)
      return(false);
  }
//m_iterations is 3 and is incremented by 2... so all odd numbers up to the square root is checked to 
//determine if it is a prime factor and then stored. Also increments by 1
  for (uint64_t i = m_iterations; i<= sqrt(m_current); i = i+2){
    iter = iter+1;
    m_iterations = m_iterations+2;
//if iterations meet the max step, return a false and exit this function.
    if(iter >= max_steps)
      return(false);
    while(m_current%i==0){
      m_factors.push_back(i);
      m_current = m_current/i; 
    }
  }
//if the final m_current is still >2 it is a prime factor and is stored.
  if (m_current>2)
    m_factors.push_back(m_current);  
//all prime factors have been determined and the function is ended returning a true value.
  return(true);
}

//-------------------------------------------------------------
// Procedure:getReport
std::string PrimeEntry::getReport()
{
  //extracs the original value and converts it into a string called s_orig..     
  stringstream ss_orig, ss_primes;
  ss_orig << m_orig;
  string s_orig = "orig" + ss_orig.str();
  //converts the received index into a string s_rec
  string s_rec = + ",received="+intToString(m_received_index);
  //converts the calculated index into a string s_calc    
  string s_calc = ",calculated=" + intToString(m_calculated_index);
  //calculate the solve time for the calculation and converts it into a string.
  string s_solve_time = ",solve_time=" + doubleToString(MOOSTime() - m_start_index,3);
  //converts m_factors into a string formate seperated by semi-colons... i.e. "3:3:5:7"
  copy(m_factors.begin(),m_factors.end(), ostream_iterator<uint64_t>(ss_primes, ":"));
  string s_primes = ss_primes.str();
  s_primes = ",primes=" + s_primes.substr(0, s_primes.length()-1);  
  string s_user = ",username=bastanfi";
  //combines the above strings to output back to the Iterate loop
  string string_out = s_orig + s_rec + s_calc + s_solve_time+s_primes + s_user;
  return(string_out);
}



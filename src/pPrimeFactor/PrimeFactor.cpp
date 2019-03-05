/************************************************************/
/*    NAME: Brian Stanfield                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

 #include "PrimeFactor.h"

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{ 
  list<PrimeEntry> m_primes; //NOT necessary
  m_calc = 0; //necessary: 
  m_rec = 0;
}
PrimeFactor::~PrimeFactor()
{ 
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
      string sval = msg.GetString();
      uint64_t orig = strtoul(sval.c_str(),NULL,0);
      m_rec = m_rec + 1;
      PrimeEntry new_prime;
      new_prime.setReceivedIndex(m_rec);
      new_prime.setOriginalVal(orig);
      new_prime.setStartTime(MOOSTime());
      new_prime.setCurrentVal(orig);
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
    if(current_prime.isDone()==true){
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
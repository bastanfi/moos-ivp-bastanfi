/************************************************************/
/*    NAME: Brian Stanfield                                 */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                    */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include <vector>
#include <cstdint>
#include "MOOS/libMOOS/MOOSLib.h"

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry 
{
public:

	PrimeEntry();
	virtual ~PrimeEntry() {};

	void setOriginalVal(uint64_t v){m_orig=v;};
	void setReceivedIndex(unsigned int v) {m_received_index = v;};
	void setCalculatedIndex(unsigned int v) {m_calculated_index = v;};
	void setDone(bool v) {m_done=v;};
	bool done() {return(m_done);};
	bool factor(unsigned long int max_steps);
	std::string getReport();
    
    uint64_t m_orig;
    unsigned long int max_steps;
    std::vector<uint64_t> m_factors;
    unsigned int m_received_index;
	unsigned int m_calculated_index;
	double m_start_index;
	bool m_done;
	uint64_t m_current;
	uint64_t m_iterations;
	protected:
};
#endif
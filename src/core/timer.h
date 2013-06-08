#ifndef AXLE_CORE_TIMER_H
#define AXLE_CORE_TIMER_H

#include "../core/settings.h"

#include <string>
#include <stack>
#include <vector>

#include "../core/types.h"

namespace ax {

class SeqTimer {
public:
	static void Begin(const std::string& info);
	static double End();	
private:
	static std::stack<double> s_time_stack;
	static std::stack<std::string> s_info_stack;
};

class AccumTimer {
public:	
	AccumTimer() : end_called_(false), begin_(false) { }
	void TimerBegin();
	void Accum(const std::string& info);
	void TimerEnd();
	void PrintResult();
private:
	double last_time_;
	std::vector<std::string> infos_;
	std::vector<double> accums_;
	uint32 accum_id_;
	bool end_called_;
	bool begin_;
};

double GetTime();
} // ax

#endif // AXLE_CORE_TIMER_H

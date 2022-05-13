#ifndef INCLUDE_PAULONNE_IDLECONTAINER_PROCSTAT_HPP_
#define INCLUDE_PAULONNE_IDLECONTAINER_PROCSTAT_HPP_
#include <string>
#include <fstream>
#include <tuple>

class ProcStat{
	public:
		ProcStat();
		uint_fast64_t read_cpu_idle_ms();

	private:
		std::ifstream filestat;
		uint_fast64_t ratio_to_ms;

};

#endif  // INCLUDE_PAULONNE_IDLECONTAINER_PROCSTAT_HPP_
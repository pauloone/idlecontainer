#include "procstat.h"
#include <unistd.h>
#include <iostream>


ProcStat::ProcStat() : filestat("/proc/stat"), ratio_to_ms(1000 / sysconf(_SC_CLK_TCK)) {}

uint_fast64_t ProcStat::read_cpu_idle_ms() {
	uint_fast64_t n;
	filestat.seekg(0);
	filestat.ignore(3) >> n >> n >> n >> n;
	return n * ratio_to_ms;
}
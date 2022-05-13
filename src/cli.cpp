#include <iostream>
#include <exception>
#include "docopt.h"
#include "container_manager.h"
#include "procstat.h"
#include <unistd.h>
#include <chrono>
#include <cstdint>

static const char USAGE[] =
R"(IDLEContainer

    Usage:
      idlecontainer [--docker-socket=<path>] trottle <container_name> <throtle_value>

    Options:
      -h --help                     Show this screen.
      -s --docker-socket=<path>     [default: unix:///var/run/docker.sock]
)";

int main(int argc, const char** argv)
{
    try {
        auto args = docopt::docopt(USAGE,
                             { argv + 1, argv + argc },
                             true,
                             "IdleContainer 0.0");

        auto container_manager = ContainerManager(args.at("--docker-socket").asString());
        auto procstat = ProcStat();
        auto number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
        const float target = std::stof(args.at("<throtle_value>").asString());
        const uint_fast64_t cpu_quota = number_of_processors * 1000; // the container will run 1000 per period per cycle

        auto running_containers = container_manager.running_containers();
        auto container_id = running_containers.find(args.at("<container_name>").asString());
        if (container_id == running_containers.end()){
            std::cout << "Container not found: " << args.at("<container_name>").asString() << std::endl;
            return 2;
        }
        container_manager.add_container(container_id->second);

        // main loop
        uint_fast64_t last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        auto new_tick = last_tick;
        auto last_idle = procstat.read_cpu_idle_ms();
        auto new_idle = last_idle;
        uint_fast64_t cpu_period = 100000;

        while(true){
            usleep(cpu_period);
            new_idle = procstat.read_cpu_idle_ms();
            new_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            float cpu_usage = 1.0f - ((float) (new_idle - last_idle) / (number_of_processors * (new_tick - last_tick)));
            float container_usage = target - cpu_usage;
            if (container_usage < 0.001){
                container_usage = 0.001; //We limit the period to 1s max
            }
            cpu_period = (uint_fast64_t) (1000.0 / container_usage);
            container_manager.throttle(cpu_period, cpu_quota);
            last_tick = new_tick;
            last_idle = new_idle;

        }

        return 0;
    } catch (std::exception& e) {
      std::cout << "Exception occured: " << e.what() << std::endl;
      return 1;
    }
}
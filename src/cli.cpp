#include <iostream>
#include <exception>
#include "docopt.h"
#include "container_manager.h"
#include "procstat.h"
#include <unistd.h>
#include <chrono>
#include <cstdint>
#include <algorithm>

static const char USAGE[] =
R"(IDLEContainer

    Usage:
      idlecontainer [options] throttle <container_name> <throttle_value>

    Options:
      -h --help                     Show this screen.
      -s --docker-socket=<path>     The path to the docker socket [default: unix:///var/run/docker.sock]
      -m --metrics                  Display the cpu usage and command in stdout, separated with commas.
)";

const float KP = 0.8;      // P coefficient of the cpu usage control PID
const float KI = 0.1;    // I coefficient of the cpu usage control PID

const float MAX_INTEGRAL = 1.2 * ContainerManager::MAX_PERIOD_US / (1000 * KI); // The integral is saturated according to the maximum command (integral * KI can achieve 1.2 time the max command)

int main(int argc, const char** argv)
{
    try {
        auto args = docopt::docopt(USAGE,
                             { argv + 1, argv + argc },
                             true,
                             "IdleContainer 0.0");

        auto procstat = ProcStat();
        auto number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
        float target_tmp; // We need to temporary use a non const value because of try catch
        try {
            target_tmp = std::stof(args.at("<throttle_value>").asString());
        } catch (std::exception& e) {
          std::cout << "throttle_value should be a float" << std::endl;
          return 100;
        }
        const float target = target_tmp;

        const uint_fast64_t cpu_quota = number_of_processors * 1000; // the container will run 1000 per period per cycle
        auto container_manager = ContainerManager(args.at("--docker-socket").asString(), cpu_quota);
        const float min_container_usage = 1000.0f / ContainerManager::MAX_PERIOD_US;
        bool metrics = args.at("--metrics").asBool();

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
        float integral = 0.0;

        if(metrics){
            std::cout << "CPU Usage" << "," << "Command" << std::endl;
        }

        while(true){
            usleep(cpu_period);
            new_idle = procstat.read_cpu_idle_ms();
            new_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            float cpu_usage = 1.0f - ((float) (new_idle - last_idle) / (number_of_processors * (new_tick - last_tick)));
            
            // We calculate the PID
            float error = target - cpu_usage;
            integral += error;

            // We saturate the integral
            integral = std::clamp(integral, - MAX_INTEGRAL, MAX_INTEGRAL);

            float command = KP * error + KI * integral;

            cpu_period = (uint_fast64_t) (1000.0 / command);
            cpu_period = std::clamp(cpu_period, (uint_fast64_t) 1000, ContainerManager::MAX_PERIOD_US);
            container_manager.throttle(cpu_period);
            last_tick = new_tick;
            last_idle = new_idle;
            if(metrics){
                std::cout << std::to_string(cpu_usage) << "," << std::to_string(command) << std::endl;
            }

        }

        return 0;
    } catch (std::exception& e) {
      std::cout << "Exception occured: " << e.what() << std::endl;
      return 1;
    }
}
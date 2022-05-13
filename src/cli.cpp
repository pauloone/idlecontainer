#include <iostream>
#include <exception>
#include "docopt.h"
#include "container_manager.h"

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
        auto running_containers = container_manager.running_containers();
        auto container_id = running_containers.find(args.at("<container_name>").asString());
        if (container_id == running_containers.end()){
            std::cout << "Container not found: " << args.at("<container_name>").asString() << std::endl;
            return 2;
        }
        container_manager.add_container(container_id->second);
        container_manager.throttle(100000);
        return 0;
    } catch (std::exception& e) {
      std::cout << "Exception occured: " << e.what() << std::endl;
      return 1;
    }
}
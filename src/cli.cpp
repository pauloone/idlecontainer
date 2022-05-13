#include <iostream>
#include <exception>
#include "docopt.h"
#include "docker_client.h"

static const char USAGE[] =
R"(IDLEContainer

    Usage:
      idlecontainer [--docker-socket=<path>] trottle <container_id> <throtle_value>

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

        auto docker_client = DockerClient(args.at("--docker-socket").asString());
        docker_client.update_container(args.at("<container_id>").asString());
        return 0;
    } catch (std::exception& e) {
      std::cout << "Exception occured: " << e.what() << std::endl;
      return 1;
    }
}
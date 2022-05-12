#include <iostream>
#include "docopt.h"

static const char USAGE[] =
R"(IDLEContainer

    Usage:
      idlecontainer throttle <container_id> <throtle_value>
)";

int main(int argc, const char** argv)
{
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,
                         "IdleContainer 0.0");
    return 0;
}
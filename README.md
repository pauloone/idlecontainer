# IDLEcontainer

A program that limit a container to use IDLE cpu resources

This project started around the idea to be able to run project like BOINC on IDLE resources via container technology. IDLE container is a program that will throttle a container CPU quota in regards of the CPU usage of the hosts.

It means to have a very low CPU and RAM footprint in order to limits impact on the system.

The use of container technology allow to use unused server CPU resources for non critical applications.

# Version

| 0.0.1   | First basic version, that allow very simple throttling of a container by name |
| 1.0.0   | Initial version, supporting throttling a lone container with simple PID.      |

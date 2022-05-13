#ifndef INCLUDE_PAULONNE_IDLECONTAINER_DOCKER_CLIENT_HPP_
#define INCLUDE_PAULONNE_IDLECONTAINER_DOCKER_CLIENT_HPP_
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <map>
#include <vector>

class ContainerManager{
	public:
		ContainerManager(const std::string &host);
		std::map<std::string, std::string> running_containers();
		void add_container(const std::string &container_id);

	private:
		std::string uri;
		std::string curl_response;
		std::vector<std::string> containers_to_throttle;
		CURL* easy_handle;
		bool unix_socket;
		char errbuf[CURL_ERROR_SIZE];

		static size_t write(void* buf, size_t size, size_t nr, void* userdata);
		void curl_easy_set_opterr(CURLoption option, auto parameter); 

};

#endif  // INCLUDE_PAULONNE_IDLECONTAINER_DOCKER_CLIENT_HPP_
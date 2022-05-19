#include "container_manager.h"
#include "exceptions.h"
#include <stdio.h>

using json = nlohmann::json;

ContainerManager::ContainerManager(const std::string &host, const uint_fast64_t &cpu_quota) : containers_to_throttle(),
																							  cpu_throttle_format((JSON_CPU_THROTTLE_START + std::to_string(cpu_quota) + JSON_CPU_THROTTLE_END)),
																							  format_buffer_len(cpu_throttle_format.length() + std::to_string(MAX_PERIOD_US).length() + 1),
																							  format_buffer(new char[format_buffer_len]) {
	/*
		Initialise the docker client and it's internal curl handler.
	*/
	//parsing the socket path
	if(host.find("unix://") == 0){
		this->unix_socket = true;
		this->uri = "http://localhost/v1.41";
	} else {
		this->unix_socket = false;
		this->uri = host + "/v1.41";
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	this->easy_handle = curl_easy_init();

	if (this->easy_handle == NULL)
	{
	     throw DockerException("Unable to initiate curl");
	}

	//we set a common error buffer so we can use generic member functions to deal with errors.
	auto res = curl_easy_setopt(this->easy_handle, CURLOPT_ERRORBUFFER, this->errbuf);
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
	this->errbuf[0] = 0;
	
	if(this->unix_socket){
		//if it's a unix socket we set it.
		curl_easy_set_opterr(CURLOPT_UNIX_SOCKET_PATH, host.substr(7).c_str());
	}

	// Generic default callback. Curl response will be written into this->curl_response.
	curl_easy_set_opterr(CURLOPT_WRITEFUNCTION, &ContainerManager::write);
	curl_easy_set_opterr(CURLOPT_WRITEDATA, &(this->curl_response));

	// We raise an exception on error
	curl_easy_set_opterr(CURLOPT_FAILONERROR, 1L);

	// We set the content-type to application/json
	struct curl_slist *hs=NULL;
	hs = curl_slist_append(hs, "Content-Type: application/json");
	curl_easy_set_opterr(CURLOPT_HTTPHEADER, hs);
}

std::map<std::string, std::string> ContainerManager::running_containers(){
	/*
		Return a map of the running containers
	*/
	std::map<std::string, std::string> running_containers;

	// We make the query
	std::string const url = this->uri + "/containers/json";
	curl_easy_set_opterr(CURLOPT_URL, url.c_str());
	curl_easy_set_opterr(CURLOPT_POST, 0L);
	auto const res = curl_easy_perform(this->easy_handle);
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}

	// We parse the response
	auto const data = json::parse(this->curl_response);

	for (auto const& container_info : data) {
		for(auto const& name : container_info["Names"]){
			auto const name_s = name.get<std::string>();
			if(name_s.find("/")  == 0){
				running_containers[name_s.substr(1)] = container_info["Id"];
			} else {
				running_containers[name_s] = container_info["Id"];
			}
		}	
	}

	return running_containers;
}

void ContainerManager::add_container(const std::string &container_id){
	//We add the url so we don't have to concatenate the url all the time;
	this->containers_to_throttle.push_back(this->uri + "/containers/" + container_id + "/update");
}

void ContainerManager::throttle(const int_fast64_t &cpu_period){
	/*throttle all monitored containers to the specified cpu_period. The cpu_quota is set to 1000 microseconds
	*/
    curl_easy_set_opterr(CURLOPT_POSTFIELDSIZE, -1L);
	//We don't use the json library to minimize cost
	snprintf(format_buffer, format_buffer_len, cpu_throttle_format.c_str(), cpu_period);
	curl_easy_set_opterr(CURLOPT_POSTFIELDS, format_buffer);

	for(auto const& container : this->containers_to_throttle){
		curl_easy_set_opterr(CURLOPT_URL, container.c_str());
		auto const res = curl_easy_perform(this->easy_handle);
		if(res != CURLE_OK){
			throw CURLException(res, this->errbuf);
		}
	}
}

void ContainerManager::curl_easy_set_opterr(CURLoption option, auto parameter) {
	/*
		Allow to set a curl option on the handle and will raise a CURLException is curl returns an error.
	*/
	auto res = curl_easy_setopt(this->easy_handle, option, parameter);
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
}

size_t ContainerManager::write(void* buf, size_t size, size_t nr, void* outputstring) {
	/*
		curl callback function. Will write the response into a std::string.
	*/
	*(static_cast<std::string*>(outputstring)) = std::string(static_cast<char*>(buf));
	return size * nr;
}

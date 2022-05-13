#include "docker_client.h"
#include "docker_exception.h"

DockerClient::DockerClient(const std::string &host) {
	//parsing the socket path
	if(host.find("unix://") == 0){
		this->unix_socket = true;
		this->uri = "http://localhost";
	} else {
		this->unix_socket = false;
		this->uri = host;
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	this->easy_handle = curl_easy_init();

	if (this->easy_handle == NULL)
	{
	     throw DockerException("Unable to initiate curl");
	}

	auto res = curl_easy_setopt(this->easy_handle, CURLOPT_ERRORBUFFER, this->errbuf);
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
	this->errbuf[0] = 0;
	
	if(this->unix_socket){
		res = curl_easy_setopt(this->easy_handle, CURLOPT_UNIX_SOCKET_PATH, host.substr(7).c_str());
		if(res != CURLE_OK){
			throw CURLException(res, this->errbuf);
		}
	}
	res = curl_easy_setopt(this->easy_handle, CURLOPT_WRITEFUNCTION, &DockerClient::write);
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
	res = curl_easy_setopt(this->easy_handle, CURLOPT_WRITEDATA, &(this->curl_response));
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
}

void DockerClient::update_container(const std::string &container_id){
	char errbuf[CURL_ERROR_SIZE];
	std::string url = this->uri + "/containers/" + container_id + "/update";
	auto res = curl_easy_setopt(this->easy_handle, CURLOPT_URL, url.c_str());
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
	res = curl_easy_perform(this->easy_handle);;
	if(res != CURLE_OK){
		throw CURLException(res, this->errbuf);
	}
}

size_t DockerClient::write(void* buf, size_t size, size_t nr, void* outputstring) {
	*(static_cast<std::string*>(outputstring)) = std::string(static_cast<char*>(buf));
	return size * nr;
}
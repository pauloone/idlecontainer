#include <string>
#include <curl/curl.h>

class DockerClient{
	public:
		DockerClient(const std::string &host);
		void update_container(const std::string &container_id);

	private:
		std::string uri;
		std::string curl_response;
		CURL* easy_handle;
		bool unix_socket;
		char errbuf[CURL_ERROR_SIZE];

		static size_t write(void* buf, size_t size, size_t nr, void* userdata);

};
#include <exception>
#include <string>
#include "curl/curl.h"

class DockerException: public std::exception {
	public:
		DockerException(const std::string &message);
		virtual const char* what() const noexcept;

	private:
		std::string message;
};

class CURLException: public std::exception {
    public:
		CURLException(const CURLcode &curl_ecode, const char* error);
		virtual const char* what() const noexcept;

	private:
		std::string message;
};
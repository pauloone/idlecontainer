#include "exceptions.h"

DockerException::DockerException(const std::string &message) : message(message){}

const char* DockerException::what() const noexcept
{
	return this->message.c_str();
}

CURLException::CURLException(const CURLcode &curl_ecode, const char* error){
	this->message = std::string(curl_easy_strerror(curl_ecode)) + ". Details are: " + std::string(error);
}

const char* CURLException::what() const noexcept
{
	return this->message.c_str();
}
#pragma once
#include <string>
#include <exception>

class EpollServerException : public std::exception
{
private:
	std::string message;
public :
	EpollServerException(std::string _message) : message(_message) {};
	virtual const char* what() const throw() { return message.c_str(); };
};


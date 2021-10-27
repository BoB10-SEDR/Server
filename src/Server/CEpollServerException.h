#pragma once
#include "stdafx.h"
#include <exception>

class CEpollServerException : public std::exception
{
private:
	std::string message;
public :
	CEpollServerException(std::string _message) : message(_message) {};
	virtual const char* what() const throw() { return message.c_str(); };
};


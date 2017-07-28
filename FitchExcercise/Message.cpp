#include "Message.h"
#include <iostream>
#include <string>
#include <algorithm>

Message::Message()
{
	this->msg = "";
	this->msg_type = "";
	this->priority = 0;
	this->order = 0;
}

Message::Message(std::string message, std::string type, int priority,int order)
{
	this->msg = message;
	//transforming the type to lower case
	std::transform(type.begin(), type.end(), type.begin(), ::tolower);
	if(type == "red" || type == "blue" || type == "yellow")
	{ 
		this->msg_type = type;
	}
	else
	{
		throw std::invalid_argument("Message type is not compatible");
	}

	if (priority == 1 || priority == 2 || priority == 3)
	{
		this->priority = priority;
	}
	else
	{
		throw std::invalid_argument("Priority of Message is not compatible");
	}

	if (order > 0)
		this->order = order;
	else
		throw std::invalid_argument("Order of Message must be positive and  > 0");
}

std::string Message::getMsg()
{
	return this->msg;
}


std::string Message::getMsg_type()
{
	return this->msg_type;
}

int Message::getPriority()
{
	return this->priority;
}

int Message::getOrder()
{
	return this->order;
}

void Message::showMessage()
{
	
	std::cout << "Message:" << this->getMsg() << "  Type:  " << this->getMsg_type() << "  Priority:  " << this->getPriority() <<  "  Order:  " << this->order << std::endl;
}

bool Message::operator<(const Message & other) const
{
	
		return priority < other.priority;
	
}

Message::~Message()
{

}

#pragma once
#include <iostream>
class Message
{
private:
	std::string msg;
	std::string msg_type;
	int priority;
	int order;

public:
	Message();
	Message(std::string message, std::string type, int priority,int order);
	std::string getMsg();
	std::string getMsg_type();
	int getPriority();
	int getOrder();
	void showMessage();
	bool operator<(const Message &other)const;
	
	~Message();
};

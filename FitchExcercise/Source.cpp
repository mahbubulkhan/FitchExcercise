#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <thread>
#include <mutex>
#include "Message.h"

//Map is being used to store the messages
std::map<std::string, std::vector<Message>> storage;
int counter = 0;
std::mutex mu_for_insert,_mu, mu_for_delete_type,mu_for_delete_order, mu_for_delete_priority;

void interstMessages(Message &msg, bool(*checkMessageType)(std::string msgType), bool (*checkPriorityOfMessage)(int priority))
{
	
	if (checkMessageType(msg.getMsg_type()) && checkPriorityOfMessage(msg.getPriority()))
	{
		std::string name = msg.getMsg_type();
		std::unique_lock<std::mutex> locker_for_insert(mu_for_insert, std::defer_lock);
		locker_for_insert.lock();
		storage[name].push_back(msg);
		locker_for_insert.unlock();
	}
}


std::string convertToLower(std::string msgType)
{
	std::transform(msgType.begin(), msgType.end(), msgType.begin(), ::tolower);
	return msgType;
}


//for searching a message type from the map
void serchFrom(std::string search, bool (*checkMessageType)(std::string msgType))
{
	if (checkMessageType(search))
	{
		search = convertToLower(search);
		std::map<std::string, std::vector<Message>>::iterator it = storage.find(search);
		std::cout << "Searching.... " << std::endl;
		std::cout << "Message" << "   " << "Type" << "   " << "Priority" << " " << "Order" << std::endl;
		if (it != storage.end())
		{
			std::vector<Message> searchList = it->second;
			std::sort(searchList.begin(), searchList.end());
			
			for (int i = 0; i < searchList.size(); i++)
			{

				searchList[i].showMessage();
			}
		}

		else
		{
			std::cout << search << " type of message is not available!!!" << std::endl;
		}
	}
}


// To check if the messagetype is Red, Blue or Yellow
bool checkMessageType(std::string msgType)
{
	msgType = convertToLower(msgType);
	
	if (msgType == "red" || msgType == "blue" || msgType == "yellow")
	{
		return true;
	}
	else
	{
		throw std::invalid_argument("Message Type is not compatible");
	}
}

// To check if priority of message is 1, 2 or 3
bool checkPriorityOfMessage(int priority)
{
	if (priority == 1 || priority == 2 || priority == 3)
		return true;
	else
		throw std::invalid_argument("Priority Type is not compatible");
}

//Delete from the map-storage based on message type
void deleteByMessageType(std::string messageType, bool(*checkMessageType)(std::string msgType))
{
	bool flag = true;
	messageType = convertToLower(messageType);
	std::unique_lock<std::mutex> locker_for_delete_msgtype(mu_for_delete_type, std::defer_lock);
	if (checkMessageType(messageType))
	{
		locker_for_delete_msgtype.lock();
		std::map<std::string, std::vector<Message>>::iterator it = storage.find(messageType);
		if (it != storage.end())
		{
			storage.erase(it);
			flag = false;
		}
		locker_for_delete_msgtype.unlock();
	}
	else
	{
		throw std::invalid_argument("Message type for delete is not compatible");
	}

	if (flag)
	{
		std::cout << " Message of type " << messageType << " is not stored " << std::endl;
	}
	
}

//Delete message based on message prioroty  
void deleteByMessagePriority(int priority, bool(*checkPriorityOfMessage)(int val))
{
	//A falg to indicate whether at least one message of that priority is found
	bool flag = true;
	std::unique_lock<std::mutex> locker_for_delete_msgpriority(mu_for_delete_priority, std::defer_lock);
	if (checkPriorityOfMessage(priority))
	{
		locker_for_delete_msgpriority.lock();
		for (std::map<std::string, std::vector<Message>>::iterator it = storage.begin(); it != storage.end(); it++)
		{
			for (std::vector<Message>::iterator vec_it = it->second.begin(); vec_it != it->second.end(); )
			{
				if (vec_it->getPriority() == priority)
				{
					vec_it = it->second.erase(vec_it);
					flag = false;
				}
				else
					vec_it++;
			 }
			
		  }
		locker_for_delete_msgpriority.unlock();
		if (flag)
		{
			std::cout << "No message with this priority " << priority << " exists" << std::endl;
		}
	
	}
	
}

//Delete message based on message order
void deleteByMessageOrder(int order)
{
	if (order < 0)
	{
		throw std::invalid_argument("Order of Message can't be negative");
	}
	else
	{
		
		bool flag = true;
		std::unique_lock<std::mutex> locker_for_delete_msgorder(mu_for_delete_order, std::defer_lock);
		locker_for_delete_msgorder.lock();
		for (std::map<std::string, std::vector<Message>>::iterator it = storage.begin(); it != storage.end(); it++)
		{
			
			for (std::vector<Message>::iterator vec_it = it->second.begin(); vec_it != it->second.end(); )
			{
				
				if (vec_it->getOrder() ==  order)
				{
					vec_it = it->second.erase(vec_it);
					flag = false;
				}
				else
				{
					vec_it++;
				}
			}
		}
		locker_for_delete_msgorder.unlock();
		if (flag)
		{
			std::cout << "No message with this order " << order << " exists" << std::endl;
		}
	}


}

//Displays all the message 
void displayMessages()
{
	//std::cout << "" <<"Message" << " " << "Type" << "   " << "Priority" << " " << "Order" << std::endl;
	for (std::map<std::string, std::vector<Message>>::iterator it = storage.begin(); it != storage.end(); it++)
	{
		std::string index = it->first;

		std::vector<Message> msgs = it->second;

		std::sort(msgs.begin(), msgs.end());

		for (int i = 0; i < msgs.size(); i++)
			msgs[i].showMessage();

		std::cout << std::endl;
	}
}

void showOperations()
{
	std::unique_lock<std::mutex> locker(_mu, std::defer_lock);
	locker.lock();
	int action;
	while (true)
	{
		std::cout << " Enter what operations you want to perform : " << std::endl;
		std::cout << "1 : Search (By Message Type)" << std::endl;
		std::cout << "2 : Delete (By Message Type, Message Priority, Message Order)" << std::endl;
		std::cout << "3 : Display stored messages" << std::endl;
		std::cin >> action;
		if (action == 1)
		{
			std::cout << "Enter the type of message you want to search[red,yellow,blue]" << std::endl;
			std::string stringTobeSearch;
			std::cin >> stringTobeSearch;
			serchFrom(stringTobeSearch, checkMessageType);
		}
		else if (action == 2)
		{
			std::cout << "Enter how you want to delete (1: Type, 2: Priority 3: Order) " << std::endl;
			int deleteAction;
			std::cin >> deleteAction;
			if (deleteAction == 1)
			{
				std::cout << "Enter the type of message you want to delete[red,yellow,blue]" << std::endl;
				std::string stringTobeDeleted;
				std::cin >> stringTobeDeleted;
				deleteByMessageType(stringTobeDeleted, checkMessageType);
				////Show the current messages list
				displayMessages();
			}
			else if (deleteAction == 2)
			{
				std::cout << "Enter priority of message you want to delete[1,2 or 3]" << std::endl;
				int priortyToBeDeleted;
				std::cin >> priortyToBeDeleted;
				deleteByMessagePriority(priortyToBeDeleted, checkPriorityOfMessage);
				displayMessages();
			}
			else if (deleteAction == 3)
			{
				std::cout << "Enter order of message you want to delete" << std::endl;
				int orderToBeDeleted;
				std::cin >> orderToBeDeleted;
				deleteByMessageOrder(orderToBeDeleted);
				displayMessages();
			}
			else
			{
				throw std::invalid_argument("INVALID DELETE ACTION");
			}
			
		}
		else if (action == 3)
		{
			displayMessages();
		}
		else
		{
			locker.unlock();
			throw std::invalid_argument("INVALID ACTION");
		}
	}
	locker.unlock();
}

int main()
{
	try
	{
		
		Message a("This is Yellow - 1", "yellow", 1,++counter);
		interstMessages(a, checkMessageType, checkPriorityOfMessage);
		Message b("This is Yellow - 2", "yellow", 3, ++counter);
		interstMessages(b, checkMessageType, checkPriorityOfMessage);
		Message c("This is Red - 1", "red", 3, ++counter);
		interstMessages(c, checkMessageType, checkPriorityOfMessage);
		Message d("This is Red - 2", "red", 1, ++counter);
		interstMessages(d, checkMessageType, checkPriorityOfMessage);
		Message e("This is Red - 3", "red", 1, ++counter);
		interstMessages(e, checkMessageType, checkPriorityOfMessage);
		Message b1("This is Blue - 1", "blue", 1, ++counter);
		interstMessages(b1, checkMessageType, checkPriorityOfMessage);
		Message b2("This is Blue - 2", "blue", 1, ++counter);
		interstMessages(b2, checkMessageType, checkPriorityOfMessage);
		Message b3("This is Blue - 3", "blue", 2, ++counter);
		interstMessages(b3, checkMessageType, checkPriorityOfMessage);
		Message b4("This is Blue - 3", "blue", 2, ++counter);
		interstMessages(b4, checkMessageType, checkPriorityOfMessage);
		
		std::thread t1(showOperations);
		if (t1.joinable())
		{
			t1.join();
		}

	}
	catch (const std::invalid_argument& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
	catch (std::exception e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Other Exception" << std::endl;
	}

	return 0;
}
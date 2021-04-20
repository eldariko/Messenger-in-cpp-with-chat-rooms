/*
 * main.cpp
 *
 *  Created on: Feb 6, 2013
 *      Author: Eldar Yaacobi
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <iostream>
#include <string.h>
#include "TCPMessengerClient.h"
#include "UDPSocket.h"
#include "UDPMessenger.h"
#include <string>
#include <cstdlib>
using namespace std;

void printInstructions() {
	cout << "To connect to the server : c <server ip>" << endl;
	cout << "before you send messages you need to log in to the server "<< endl;
	cout << "if you dont have an account please register: register <user name> <password>"<< endl;
	cout << "To log in: login <user name> <password>" << endl;
	cout << "To be online: co <user name> <password>" << endl;
	cout << "For a list of current status of the user: l <user name>" << endl;
	cout << "before you open session type with user need to online in to the server "<< endl;
	cout << "To open session type with user: o <user>" << endl;
	cout << "For a list of open peers : lcu" << endl;
	cout << "For a list of All open peers : lu" << endl;
	cout << "After the session is opened, to send message type to client: s <user name> <message>"<< endl;
	cout << "just the user that opened the session can close him" << endl;
	cout << "To close opened session with specific peer type: cs <specific peer>" << endl;
	cout << "To disconnect from server type: d" << endl;
	cout << "To exit type: x" << endl;

}
class OnRecieveClbkImpl: public OnRecieveClbk {
	void handleMessage(string msg) {
		cout << ">" << msg << endl;
	}
};

int main() {
	cout << "Welcome to TCP messenger" << endl;
	printInstructions();
	string lo_user_name,lo_password,co_user_name,co_password,
	r_user_name,r_password,msg, command, ip, peer;
	TCPMessengerClient* messenger = new TCPMessengerClient();
	OnRecieveClbkImpl* clbk=new OnRecieveClbkImpl();
	UDPMessenger* udpMessenger= new UDPMessenger(clbk);
	while (true) {
		cin >> command;
		if (command == "c") {
			cin >> ip;
			messenger->connect(ip);
		} else if (command == "r") {
			cin >> r_user_name;
			cin >> r_password;
			messenger->registeR(r_user_name, r_password);
		} else if (command == "lo") {
			cin >> lo_user_name;
			cin >> lo_password;
			messenger->setUserName(lo_user_name);
			messenger->login(lo_user_name, lo_password);
		} else if (command == "co") {

			cin >> co_user_name;
			cin >> co_password;
			if((lo_user_name != co_user_name) || (lo_password != co_password)){
			cout<<"the pass or user name are not Matches to the log in user and pass"<<endl;
			}
			else
			messenger->connectOnline(co_user_name, co_password);
		} else if (command == "l") {
			cin >> peer;
			messenger->currentStatusOfTheClient(peer);
		} else if (command == "o") {
			cin >> peer;
			messenger->open(peer);

		} else if (command == "lu") {
			messenger->listOfAllpeers();
		} else if (command == "lcu") {
			messenger->listConnectedPeers();

		} else if (command == "s") {
			cin >> peer;
			getline(std::cin, msg);
			if (msg.size() > 0 && msg[0] == ' ')
				msg.erase(0, 1);
			if (!messenger->isActiveClientSession()) {
				cout << "Fail: No client session is active" << endl;
				printInstructions();
			} else {
				if((lo_user_name == co_user_name) && (lo_password == co_password)){
				ip = messenger->fromIpToName(peer);
				if(clbk==NULL)
				clbk = new OnRecieveClbkImpl();
				if( udpMessenger==NULL)
				udpMessenger = new UDPMessenger(clbk);
				udpMessenger->sendTo(co_user_name, ip);
				udpMessenger->sendTo(msg, ip);
				cout<<">"<<endl;
				}else cout<<"problem in send function"<<endl;

			}
		} else if (command == "cs") {

			cin >> peer;
			messenger->closeActiveSession(co_user_name,peer);
			udpMessenger->close();
			udpMessenger=NULL;
		} else if (command == "d") {
			messenger->disconnect();
		} else if (command == "x") {
			break;
		} else {
			cout << "wrong input" << endl;
			printInstructions();
		}
	}
	messenger->disconnect();
	delete messenger;
	cout << "messenger was closed" << endl;
	return 0;
}

#endif /* MAIN_CPP_ */

//============================================================================
// Name        : UDPMessenger
// Author      : Eldar Yaacobi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================

#include "UDPMessenger.h"
#include "TCPMessengerClient.h"
#include <string.h>
#include <string>
#include <cstdlib>

using namespace std;



UDPMessenger::UDPMessenger(OnRecieveClbk* clbk){
	//TODO: initiate local arguments (if needed)
rcvClbk=clbk;
	//TODO: create the socket
udpSocket=new UDPSocket(UDP_MSNGR_PORT);
this->running = true;
this->start();
	//TODO: start the thread to receive incoming messages
}

void UDPMessenger::run(){
	//TODO: create a loop in which you read from the socket and print the received messages
	char buff1[50];
	char buff2[1500];
		while(running){
			int rc = udpSocket->recv(buff1,50);
			if (rc>0 && rc<1500){
				buff1[rc] = 0;
			  }
			rc = udpSocket->recv(buff2,1500);
			if (rc>0 && rc<1500){
				buff2[rc] = 0;
				cout<<">["<<buff1<<"]  "<<buff2<<endl;
			}
		}
		cout<<"closing receiver thread"<<endl;
}
void UDPMessenger::sendTo(string msg,string ip){
char ipString[20]={0};
//char *port;
strcpy(ipString,ip.c_str());
char * psh;
psh=strtok(ipString,":");
//port=strtok(NULL,":");
string id(psh);
//int por=atoi(port);
	udpSocket->sendTo(msg,id,UDP_MSNGR_PORT);
}

void UDPMessenger::reply(string msg){
	//TODO: send the message to the address of the last received message
udpSocket->reply(msg);
}

void UDPMessenger::close(){
	//TODO: close the thread
	this->running=false;

	//TODO: close the socket
	udpSocket->cclose();


	//TODO: wait for thread to exit

	delete udpSocket;
	//TODO: delete and free any allocated resources


}


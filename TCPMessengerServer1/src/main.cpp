//============================================================================
// Name        : TCPMessengerServer
// Author      : Eldar Yaacobi
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include "TCPMessengerServer.h"

using namespace std;

void printInstructions(){
	cout<<"-----------------------"<<endl;
	cout<<"lp - list peers"<<endl;
	cout<<"lr - list of all registered users "<<endl;
	cout<<"lu - list of all users"<<endl;
	cout<<"lcu -list of all online users"<<endl;
	cout<<"ls - list of all opened sessions"<<endl;
	cout<<"x - shutdown server"<<endl;
	cout<<"-----------------------"<<endl;
}

int main(){
	cout<<"Welcome to TCP messenger Server"<<endl;
	printInstructions();
	TCPMessengerServer msngrServer;
	string list;
	msngrServer.start();
	bool loop = true;
	while(loop){
		string msg;
		string command;
		cin >> command;
		if(command == "lp"){
			msngrServer.listPeers();
		}else if(command == "lr"){
          msngrServer.printRegisteredUsers();
		}else if(command == "lu"){
			list=msngrServer.PeersList(12);
		    cout<<list<<endl;
		}else if(command == "lcu"){
			list=msngrServer.PeersList();
			  cout<<list<<endl;
		}else if(command == "ls"){
			msngrServer.printSessions();
		}else if(command == "x"){
			loop = false;
		}else{
			cout<<"wrong input"<<endl;
			printInstructions();
		}
	}
	msngrServer.close();
	msngrServer.waitForThread();
	cout<<"messenger was closed"<<endl;
	return 0;
}

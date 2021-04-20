#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"
#include <string.h>
#include <utility>
#include<pthread.h>
#define SIZE 50

pthread_mutex_t lock;
TCPSessionBroker::TCPSessionBroker(TCPMessengerServer* mesgr, TCPSocket* p1,
		TCPSocket* p2) {
		cout << "create new broker between: " << mesgr->getPeerNameByIpAndPort(p1->destIpAndPort()) << " and "
				<<mesgr->getPeerNameByIpAndPort(p2->destIpAndPort())<< endl;
	messenger = mesgr;
	peer1 = p1;
	peer2 = p2;
}

void TCPSessionBroker::run() {
	string user,peer;
	if (peer1 != NULL && peer2 != NULL) {
		cout << "Broker thread started" << endl;
		MultipleTCPSocketsListener msp;
		msp.addSocket(peer1);
		msp.addSocket(peer2);
		bool flag = true;
		string list,msg1,client,userIp;
		while (flag) {
			TCPSocket* sender = msp.listenToSocket();
			if (sender == NULL)
				break;
			TCPSocket* receiver = peer1;
			if (receiver == sender)
				receiver = peer2;
			int command = messenger->readCommandFromPeer(sender);
			switch (command) {
			case OPEN_SESSION_WITH_PEER:
				peer = messenger->readDataFromPeer(sender); //user name
						cout << "got open session command:" << messenger->getPeerNameByIpAndPort(sender->destIpAndPort())
								<< "->" << peer << endl;
			                userIp=messenger->getIpAndPortByPeerName(peer);
			                receiver = messenger->getAvailablePeerByName(peer);
							if (receiver != NULL) {
								messenger->sendCommandToPeer(sender,
										SESSION_ESTABLISHED);
								messenger->sendDataToPeer(sender,peer);
								messenger->sendDataToPeer(sender,userIp);
								messenger->sendCommandToPeer(receiver,
										OPEN_SESSION_WITH_PEER);
								client=messenger->getPeerNameByIpAndPort(sender->destIpAndPort());
								messenger->sendDataToPeer(receiver,client);
								messenger->sendDataToPeer(receiver,sender->destIpAndPort());
								messenger->addToopenSessions(client,peer);
								TCPSessionBroker * broker = new TCPSessionBroker(messenger,sender,receiver);
								broker->start();
							} else {
								cout << "FAIL: didnt find peer:" << peer << endl;
								messenger->sendCommandToPeer(sender, SESSION_REFUSED);
							}
						break;
			case CURRENT_STATUS:
				user=messenger->readDataFromPeer(sender);
			            list=messenger->sessionStatus(user);
						messenger->sendCommandToPeer(sender,CURRENT_STATUS);
			            messenger->sendDataToPeer(sender,list);
			            cout<<"broker"<<endl;
			            break;

			case CLOSE_SESSION_WITH_PEER:
				user=messenger->readDataFromPeer(sender);
				peer=messenger->readDataFromPeer(sender);
				cout << "closing session:" <<messenger->getPeerNameByIpAndPort(sender->destIpAndPort())<< " -> "
						<<messenger->getPeerNameByIpAndPort(receiver->destIpAndPort())  << endl;
				messenger->removeFromOpenSessions(user,peer);
				messenger->sendCommandToPeer(receiver, CLOSE_SESSION_WITH_PEER);
				messenger->sendDataToPeer(receiver,user);

				flag = false;
				break;
			case LIST_OF_PEERS:
						messenger->sendCommandToPeer(sender, LIST_OF_PEERS);
						msg1 = messenger->userIPList();
						messenger->sendDataToPeer(sender, msg1);
						break;
			case LIST_OF_All_PEERS:
						messenger->sendCommandToPeer(sender, LIST_OF_All_PEERS);
						msg1 = messenger->userIPList(LIST_OF_All_PEERS);
						messenger->sendDataToPeer(sender, msg1);
					break;
			default:
				cout << "peer disconnected: " << messenger->getPeerNameByIpAndPort(sender->destIpAndPort()) << endl;
				messenger->peerDisconnect(sender);
							break;
			}
		}

		cout << "closing broker between <"<< user << "> and <"
				<< peer <<">"<< endl;

	}
	delete this;
}

PeersRequestsDispatcher::PeersRequestsDispatcher(TCPMessengerServer* mesgr) {
	messenger = mesgr;
}

void PeersRequestsDispatcher::run() {
	cout << "dispatcher started" << endl;
	string user,host;
	while (messenger->running) {
		MultipleTCPSocketsListener msp;
		//while(messenger->getPeersVec().size() == 0) sleep(1);
		msp.addSockets(messenger->getOnlinePeersVec());
		msp.addSockets(messenger->getOfflinePeersVec());
		TCPSocket* readyPeer = msp.listenToSocket(2);
		if (readyPeer == NULL)
			continue;
		int command = messenger->readCommandFromPeer(readyPeer); //command
		string peerName;
		TCPSocket* scondPeer;
		string msg1, pass,userIp,client,list;
        bool check;
		switch (command) {
		case CURRENT_STATUS:
						    user=messenger->readDataFromPeer(readyPeer);
					        list=messenger->sessionStatus(user);
						messenger->sendCommandToPeer(readyPeer,CURRENT_STATUS);
					          messenger->sendDataToPeer(readyPeer,list);
					            cout<<"dispatcher"<<endl;
					            break;
		case OPEN_SESSION_WITH_PEER:
			peerName = messenger->readDataFromPeer(readyPeer); //user name

                userIp=messenger->getIpAndPortByPeerName(peerName);
				if(userIp==""){
					cout << "FAIL: didnt find peer:" << peerName << endl;
					messenger->sendCommandToPeer(readyPeer, SESSION_REFUSED);
					messenger->sendDataToPeer(readyPeer,"didnt find peer");
				break;
				}
				cout << "got open session command:" << messenger->getPeerNameByIpAndPort(readyPeer->destIpAndPort())
									<< "->" << peerName << endl;
                scondPeer = messenger->getAvailablePeerByName(peerName);
				if (scondPeer != NULL) {
					messenger->sendCommandToPeer(readyPeer,
							SESSION_ESTABLISHED);
					messenger->sendDataToPeer(readyPeer,peerName);
					messenger->sendDataToPeer(readyPeer,userIp);
					messenger->sendCommandToPeer(scondPeer,
							OPEN_SESSION_WITH_PEER);
					client=messenger->getPeerNameByIpAndPort(readyPeer->destIpAndPort());
					messenger->sendDataToPeer(scondPeer,client);
					messenger->sendDataToPeer(scondPeer,readyPeer->destIpAndPort());
					messenger->addToopenSessions(client,peerName);

					TCPSessionBroker * broker =new TCPSessionBroker(messenger,readyPeer,scondPeer);
	                broker->start();

				}else {cout<<"problem with second peer"<<endl;
				messenger->sendCommandToPeer(readyPeer, SESSION_REFUSED);
				messenger->sendDataToPeer(readyPeer,"problem with second peer");
				}

			break;
		case CLOSE_SESSION_WITH_PEER:
						user=messenger->readDataFromPeer(readyPeer);
						host=messenger->readDataFromPeer(readyPeer);
						cout << "closing session:" <<messenger->getPeerNameByIpAndPort(readyPeer->destIpAndPort())<< " -> "
								<<messenger->getPeerNameByIpAndPort(scondPeer->destIpAndPort())  << endl;
						messenger->removeFromOpenSessions(user,host);
						messenger->sendCommandToPeer(scondPeer, CLOSE_SESSION_WITH_PEER);
						messenger->sendDataToPeer(scondPeer,user);

						break;
		case LIST_OF_PEERS:
			messenger->sendCommandToPeer(readyPeer, LIST_OF_PEERS);
			msg1 = messenger->userIPList();
			messenger->sendDataToPeer(readyPeer, msg1);
			break;
		case LIST_OF_All_PEERS:
			messenger->sendCommandToPeer(readyPeer, LIST_OF_All_PEERS);
			msg1 = messenger->userIPList(LIST_OF_All_PEERS);
			messenger->sendDataToPeer(readyPeer, msg1);
		break;
		case REGISTER:
			user = messenger->readDataFromPeer(readyPeer);
			pass = messenger->readDataFromPeer(readyPeer);
			msg1=messenger->checkFile(user, pass);
			messenger->sendCommandToPeer(readyPeer,REGISTER);
			messenger->sendDataToPeer(readyPeer,msg1);
			if(msg1=="Registration success")
			cout<<"New user registered " <<user<<endl;
			break;
		case LOG_IN:
			user = messenger->readDataFromPeer(readyPeer);
			pass = messenger->readDataFromPeer(readyPeer);
            check=messenger->checkLogIn(user,1);
            if (check==false){
            	messenger->sendCommandToPeer(readyPeer,LOG_IN);
            	messenger->sendDataToPeer(readyPeer,"false");
            	break;
            }
            	check=messenger->chackIfUserExist(user, pass);
                if (check == true){
				messenger->addPeerToOflineList(readyPeer);
				messenger->addUserToOfflineList(readyPeer,user);
				messenger->sendCommandToPeer(readyPeer,LOG_IN);
				messenger->sendDataToPeer(readyPeer,"true");
			    cout<<user<<" is log in right now ."<<endl;
				break;
			}
			else{
				messenger->sendCommandToPeer(readyPeer,LOG_IN);
			    messenger->sendDataToPeer(readyPeer,"user or password are incorrect");
			break;
			}

		case CONNECT_ONLINE :
			user = messenger->readDataFromPeer(readyPeer);
			pass = messenger->readDataFromPeer(readyPeer);
			check=messenger->checkLogIn(user,2);
			 if (check==false){
				 messenger->sendCommandToPeer(readyPeer,CONNECT_ONLINE);
				 messenger->sendDataToPeer(readyPeer,"false");
			 }
			 check=messenger->chackIfUserExist(user, pass);
			if (check == true){
				cout<<"<"<<user<<"> is online ."<<endl;
				messenger->addPeerToOnlineList(readyPeer);
				messenger->addUserToOnlineList(readyPeer,user);
				messenger->sendCommandToPeer(readyPeer,CONNECT_ONLINE);
				messenger->sendDataToPeer(readyPeer,"true");
				 break;
			}
			else{
				messenger->sendCommandToPeer(readyPeer,CONNECT_ONLINE);
				messenger->sendDataToPeer(readyPeer,"you need to register before you be online");
				 break;
			}

		default:
			cout << "peer disconnected: " << messenger->getPeerNameByIpAndPort(readyPeer->destIpAndPort()) << endl;
			messenger->peerDisconnect(readyPeer);
			break;
		}
	}
	cout << "dispatcher ended" << endl;
}

/**
 * Construct a TCP server socket
 */
TCPMessengerServer::TCPMessengerServer() {
	tcpServerSocket = new TCPSocket(MSNGR_PORT);
	running = false;
	dispatcher = NULL;
	file = NULL;
	read = 0;
}

void TCPMessengerServer::run() {
	running = true;
	dispatcher = new PeersRequestsDispatcher(this);
	dispatcher->start();
	while (running) {
		TCPSocket* peerSocket = tcpServerSocket->listenAndAccept();
		//TODO: check if the client not already has a socket
		if (peerSocket != NULL) {
			cout << "new peer Connected: " << peerSocket->destIpAndPort()<< endl;
			this->sendCommandToPeer(peerSocket,CONNECT);
			this->sendDataToPeer(peerSocket,"connect Successfully");
            this->addPeerToOflineList(peerSocket);
		}
		else
			cout<<"problem"<<endl;
	}
}


void TCPMessengerServer::listPeers() {
	tOpenedPeers::iterator iter = onlinePeers.begin();
	tOpenedPeers::iterator endIter = onlinePeers.end();
	for (; iter != endIter; iter++) {
		cout << (*iter).second->destIpAndPort() << endl;
	}
}
void TCPMessengerServer::addToopenSessions(string client,string host){
	listOfSessions.push_back(std::make_pair(client,host));
}
void TCPMessengerServer::printSessions(){
	if(listOfSessions.size()>0){
	vector< pair<string,string> >::iterator iter = listOfSessions.begin();
	vector< pair<string,string> >::iterator endIter = listOfSessions.end();
	 for (; iter != endIter; iter++)
		 cout<<"<"<<(*iter).first<<" , "<<(*iter).second<< ">"<<endl;

	}
	else cout<<"no sessions"<<endl;
}
void TCPMessengerServer::removeFromOpenSessions(string client,string host){
	if(host !="ALL")
	cout<<"remove "<<client<<" , "<<host<<" from open session"<<endl;
	if (listOfSessions.size()>0){
	vector< pair<string,string> >::iterator iter = listOfSessions.begin();
	 vector< pair<string,string> >::iterator endIter = listOfSessions.end();
	if (host=="ALL"){
		for (; iter != endIter; iter++){
			if(((*iter).first==client) || ((*iter).second==client))
		        	listOfSessions.erase(iter);
		}
	}else
	 for (; iter != endIter; iter++) {
			if(((*iter).first==client && (*iter).second==host) ||((*iter).second==client && (*iter).first==host))
				listOfSessions.erase(iter);
}
	}
}
string TCPMessengerServer::sessionStatus(string user){
	pthread_mutex_lock(&lock);
pair <string,string> s;
	char peers[100] = { 0 };
		char peer[30] = { 0 };
		 vector< pair<string,string> >::iterator iter = listOfSessions.begin();
		 vector< pair<string,string> >::iterator endIter = listOfSessions.end();
			 for (; iter != endIter; iter++) {
		        if((*iter).first==user  )
				 sprintf(peer,"%s,",(*iter).second.c_str());
				 if((*iter).second==user)
				 sprintf(peer,"%s,",(*iter).first.c_str());
				 strcat(peers,peer);

		    }
		string someString(peers);

		pthread_mutex_unlock(&lock);
		return someString;

}


string TCPMessengerServer::PeersList(int choice) {
	char peers[90] = { 0 };
	char peer[30] = { 0 };
	tOpenedPeers::iterator iter = onlinePeers.begin();
	tOpenedPeers::iterator endIter = onlinePeers.end();
	for (; iter != endIter; iter++) {
		sprintf(peer, ">%s\n", (*iter).second->destIpAndPort().c_str());
		strcat(peers, peer);
	}
	if (choice==12){
		iter = oflinePeers.begin();
		endIter = oflinePeers.end();
		for (; iter != endIter; iter++){
		sprintf(peer, ">%s\n", (*iter).second->destIpAndPort().c_str());
		strcat(peers, peer);
		}
	}
	std::string someString(peers);
	return someString;
}
string TCPMessengerServer::userIPList(int choice) {
	char peers[100] = { 0 };
	char peer[30] = { 0 };
	UsersIP::iterator iter = onlineUsersList.begin();
	UsersIP::iterator endIter = onlineUsersList.end();
	for (; iter != endIter; iter++) {
		sprintf(peer, ">[%s] %s\n",(*iter).first.c_str() ,(*iter).second.c_str());
		strcat(peers, peer);
	}
	if (choice==12){
		iter = oflineUsersList.begin();
		endIter = oflineUsersList.end();
		for (; iter != endIter; iter++) {
		sprintf(peer, ">[%s] %s\n",(*iter).first.c_str(),(*iter).second.c_str());
		strcat(peers, peer);
		}
	}
	std::string someString(peers);
	return someString;
}
vector<TCPSocket*> TCPMessengerServer::getOfflinePeersVec(){
	vector<TCPSocket*> vec;
	tOpenedPeers::iterator iter = oflinePeers.begin();
	tOpenedPeers::iterator endIter = oflinePeers.end();
		for (; iter != endIter; iter++) {
			vec.push_back((*iter).second);
		}
		return vec;


}
vector<TCPSocket*> TCPMessengerServer::getOnlinePeersVec() {
	vector<TCPSocket*> vec;
	tOpenedPeers::iterator iter = onlinePeers.begin();
	tOpenedPeers::iterator endIter = onlinePeers.end();
	for (; iter != endIter; iter++) {
		vec.push_back((*iter).second);
	}
	return vec;
}



void TCPMessengerServer::close() {
	cout << "closing server" << endl;
	running = false;
	tcpServerSocket->cclose();
	tOpenedPeers::iterator iter = onlinePeers.begin();
	tOpenedPeers::iterator endIter = onlinePeers.end();
	for (; iter != endIter; iter++) {
		((*iter).second)->cclose();
	}
	 iter = oflinePeers.begin();
	 endIter = oflinePeers.end();
		for (; iter != endIter; iter++) {
			((*iter).second)->cclose();
		}
	dispatcher->waitForThread();
	iter = onlinePeers.begin();
	endIter = onlinePeers.end();
	for (; iter != endIter; iter++) {
		delete (*iter).second;
	}
	iter = oflinePeers.begin();
		endIter = oflinePeers.end();
		for (; iter != endIter; iter++) {
			delete (*iter).second;
		}
		UsersIP::iterator iter1=onlineUsersList.begin();
		UsersIP::iterator endIter1=onlineUsersList.end();
			for (; iter1 != endIter1; iter1++) {
				 onlineUsersList.erase((*iter1).first) ;
			}
			 iter1=oflineUsersList.begin();
			 endIter1=oflineUsersList.end();
						for (; iter1 != endIter1; iter1++) {
							oflineUsersList.erase((*iter1).first);
						}

	cout << "server closed" << endl;
}
void TCPMessengerServer::peerDisconnect(TCPSocket* peer) {
	  pthread_mutex_lock(&lock);
if (peer !=NULL){
	onlinePeers.erase(peer->destIpAndPort());
    oflinePeers.erase(peer->destIpAndPort());
    string name= getPeerNameByIpAndPort(peer->destIpAndPort());
    onlineUsersList.erase(name);
    oflineUsersList.erase(name);
    		peer->cclose();
	this->removeFromOpenSessions(name,"ALL");

	delete peer;
    		pthread_mutex_unlock(&lock);
}
}

bool TCPMessengerServer::checkLogIn(string user,int chose){
	UsersIP::iterator iter = onlineUsersList.find(user);
	UsersIP::iterator endIter = onlineUsersList.end();
	if (iter == endIter) {
		if(chose==2)
			return true;
			UsersIP::iterator iter = oflineUsersList.find(user);
			UsersIP::iterator endIter = oflineUsersList.end();
		if (iter == endIter)
			return true;
		else return false;
	}
	else return false;
}

TCPSocket* TCPMessengerServer::getAvailablePeerByIpAndPort(string ipAndPort) {
	tOpenedPeers::iterator iter = onlinePeers.find(ipAndPort);
	tOpenedPeers::iterator endIter = onlinePeers.end();
	if (iter == endIter) {
		return NULL;
	}
	return (*iter).second;
}

TCPSocket* TCPMessengerServer::getAvailablePeerByName(string peerName) {
	UsersIP::iterator iter = onlineUsersList.find(peerName);
	UsersIP::iterator endIter = onlineUsersList.end();
	if (iter == endIter) {
		return NULL;
	}
	return getAvailablePeerByIpAndPort((*iter).second);
}
string TCPMessengerServer::getIpAndPortByPeerName(string peerName){
	UsersIP::iterator iter = onlineUsersList.find(peerName);
	UsersIP::iterator endIter = onlineUsersList.end();
	if (iter == endIter) {
			return "";
		}
		return (*iter).second;

}
string TCPMessengerServer::getPeerNameByIpAndPort(string ipAndPort){
	UsersIP::iterator iter = onlineUsersList.begin();
	UsersIP::iterator endIter = onlineUsersList.end();
	for (; iter != endIter; iter++) {
				if((*iter).second==ipAndPort){
					return (*iter).first;
				}
			}
	iter = oflineUsersList.begin();
	endIter = oflineUsersList.end();
	for (; iter != endIter; iter++) {
		if((*iter).second==ipAndPort){
				return (*iter).first;
				}
	}
	return "";
}


void TCPMessengerServer::addPeerToOflineList(TCPSocket* peer) {
	onlinePeers.erase(peer->destIpAndPort());

	oflinePeers[peer->destIpAndPort()]=peer;
}

void TCPMessengerServer::addPeerToOnlineList(TCPSocket* peer) {
    oflinePeers.erase(peer->destIpAndPort());
	onlinePeers[peer->destIpAndPort()] = peer;
}
void TCPMessengerServer::addUserToOnlineList(TCPSocket * peer,string userName)
{
	oflineUsersList.erase(userName);
	onlineUsersList[userName]=peer->destIpAndPort();
}
void TCPMessengerServer::addUserToOfflineList(TCPSocket * peer,string userName)
{
	//UsersIP::iterator=onlineUsersList.find(userName);
	//delete (*iterator);
	onlineUsersList.erase(userName);
	oflineUsersList[userName]=peer->destIpAndPort();
}


int TCPMessengerServer::readCommandFromPeer(TCPSocket* peer) {
	int command = 0;
	int rt = peer->recv((char*) &command, 4);
	if (rt < 1)
		return rt;
	command = ntohl(command);
	return command;
}

string TCPMessengerServer::readDataFromPeer(TCPSocket* peer) {
	string msg;
	char buff[1500];
	int msgLen;
	peer->recv((char*) &msgLen, 4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen) {
		rc = peer->recv((char*) &buff[totalrc], msgLen - totalrc);
		if (rc > 0) {
			totalrc += rc;
		} else {
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen) {
		buff[msgLen] = 0;
		msg = buff;
	} else {
		peer->cclose();
	}
	return msg;
}

void TCPMessengerServer::sendCommandToPeer(TCPSocket* peer, int command) {
	command = htonl(command);
	peer->send((char*) &command, 4);
}

void TCPMessengerServer::sendDataToPeer(TCPSocket* peer, string msg) {
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	peer->send((char*) &msgLen, 4);
	peer->send(msg.data(), msg.length());
}
string TCPMessengerServer::checkFile(string user, string pass) {
	char line[50];
	int len = 0;
	int odd = 1;
	file = fopen("user_pass.txt", "a+");
	if (file == NULL)
		return "can not open file";
	while ((fgets(line, SIZE, file)) != NULL) {
		if (odd % 2 == 0) {
			odd++;
			continue;
		}
		len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		if (line == user) {

			return "the user is exist in the database";
		}
		odd++;
	}
	fputs(user.c_str(), file);
	fputs("\n", file);
	fputs(pass.c_str(), file);
	fputs("\n", file);

	fclose(file);
	return "Registration success";


}
bool TCPMessengerServer::chackIfUserExist(string user,string pass)
{
	char line[50];
		int len = 0;
		int odd = 1;
		file = fopen("user_pass.txt", "r+");
		if (file == NULL)
			return false;
		while ((fgets(line, SIZE, file)) != NULL) {
			if (odd % 2 == 0) {
					odd++;
					continue;
				}
				len = strlen(line);
				if (len > 0 && line[len - 1] == '\n')
					line[len - 1] = '\0';
				if (line == user) {
                     if(fgets(line, SIZE, file)!= NULL)
                     {
                    	 len = strlen(line);
                    	 if (len > 0 && line[len - 1] == '\n')
                    	 	line[len - 1] = '\0';
                    	 if (line == pass)
                    	 {
                    		 fclose(file);
                    		 return true;
                    	 }
                     }
				}
				odd++;
		}
		fclose(file);
		 return false;
}
void TCPMessengerServer::printRegisteredUsers(){
	char line[50];
			int len = 0;
			int odd = 1;
			file = fopen("user_pass.txt", "r+");
			if (file == NULL){
			 cout<<"problem with open file"<<endl;
			return;
			}
			cout<<"Registered Users: "<<endl;
			cout<<"-----------------------"<<endl;
			while ((fgets(line, SIZE, file)) != NULL){
				if (odd % 2 == 0) {
									odd++;
									continue;
								}
				len = strlen(line);
					if (len > 0 && line[len - 1] == '\n')
						line[len - 1] = '\0';
					cout<<">"<<line<<endl;
					odd++;

			}
			cout<<"-----------------------"<<endl;
			fclose(file);
			return;
}

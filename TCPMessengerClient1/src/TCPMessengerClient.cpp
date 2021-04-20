#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"

/**
 * initialize all properties
 */
TCPMessengerClient::TCPMessengerClient() {
	socket = NULL;
	running = false;
	peerAddress = "";
	sessionActive = false;
	registr = false;
	connected = false;
	log_in = false;
	online = false;
	serverAddress = "";
}

/**
 * client receive loop, activated once a connection to the server is established
 */
void TCPMessengerClient::run() {
	running = true;
	string msg;
	while (running) {
		int command = readCommand();
		string user, ip;
		switch (command) {
		case SEND_MSG_TO_PEER:
			msg = readDataFromPeer();
			cout << ">>" << msg << endl;
			break;
		case SESSION_REFUSED:
			msg = readDataFromPeer();
			cout<<"didnt find the peer"<<endl;
			break;
		case CLOSE_SESSION_WITH_PEER:
			user=readDataFromPeer();
			cout << "Session was closed by remote peer :" <<user<< endl;
			availableConnect.erase(user);
			break;
		case OPEN_SESSION_WITH_PEER:
			user = readDataFromPeer();
			ip = readDataFromPeer();
			listOfAllMyPeers(user, ip);
			cout << "Session was opened by remote user: " << user << endl;

			sessionActive = true;
			break;
		case SESSION_ESTABLISHED:
			user = readDataFromPeer();
			ip = readDataFromPeer();
			listOfAllMyPeers(user, ip);
			cout << "Session was opened with: " << user << endl;
			sessionActive = true;
			break;
		case LIST_OF_PEERS:
			cout << "open peers list:" << endl;
			msg = readDataFromPeer();
			cout << msg << endl;
			break;
		case CURRENT_STATUS:
			msg = readDataFromPeer();
			if(msg=="")
			cout<<"not connected"<<endl;
			else
				cout << "client " << user << " connected to : "<< msg << endl;
			break;
		case LIST_OF_All_PEERS:
			cout << "all peers list:" << endl;
			msg = readDataFromPeer();
			cout << msg << endl;
			break;
		case REGISTER:
			msg = readDataFromPeer();
			cout << msg << endl;
			if (msg == "Registration success")
				registr = true;
			break;
		case LOG_IN:
			msg = readDataFromPeer();

			if (msg == "true") {
				log_in = true;
				cout << "Login Succeeded" << endl;
			}else if(msg=="false"){
				cout<<"the user is log in right now"<<endl;
			}

			else{
				cout << "user or password are incorrect" << endl;
			    this->setUserName("");
			}
			break;
		case CONNECT_ONLINE:
			msg = readDataFromPeer();

			if (msg == "true") {
				cout << "now you are online" << endl;
				online = true;
			}else if(msg=="false"){
				cout<<"the user is log in right now"<<endl;
			}
			else
				cout << "you need to register before you be online" << endl;
			break;
		case CONNECT:
			msg = readDataFromPeer();
			cout << msg << endl;
			break;

		default:
			cout
					<< "communication with server was interrupted - connection closed"
					<< endl;
			running = false;
			socket->cclose();
			running = false;
			peerAddress = "";
			sessionActive = false;
			registr = false;
			connected = false;
			log_in = false;
			online = false;
			serverAddress = "";
			delete socket;
			break;
		}
	}
}

bool TCPMessengerClient::connect(string ip) {
	if (connected)
		disconnect();
	socket = new TCPSocket(ip, MSNGR_PORT);
	if (socket == NULL)
		return false;
	connected = true;
	serverAddress = ip;
	start();
	return true;
}

void TCPMessengerClient::registeR(string user_name, string password) {
	if (connected) {
		sendCommand(REGISTER);
		sendData(user_name);
		sendData(password);
	} else
		cout << "you need to connect first" << endl;
}

bool TCPMessengerClient::login(string user_name, string pass) {
	if (!sessionActive) {
		if (socket == NULL) {
			cout << "null socket-you need to connect to the server" << endl;
			return false;
		} else {
			sendCommand(LOG_IN);
			sendData(user_name);
			sendData(pass);
			return true;
		}
	}
	cout << "you need to connect to the server" << endl;
	return false;
}
/**
 * connect to the given server ip (the port is defined in the protocol header file)
 */
bool TCPMessengerClient::connectOnline(string user_name, string pass) {
	if (log_in) {
		if (socket == NULL) {
			cout << "null socket" << endl;
			return false;
		}
		sendCommand(CONNECT_ONLINE);
		sendData(user_name);
		sendData(pass);
		return true;
	}

	else
		cout << "you need log in before" << endl;
	return false;
}

/**
 * open session with the given peer address (ip:port)
 */
bool TCPMessengerClient::open(string name) {
	if (online) {
		cout << "Opening session with: " << name << endl;
		sendCommand(OPEN_SESSION_WITH_PEER);
		sendData(name);
		return true;
	} else
		cout << "you need to be online before you open a session" << endl;
	return false;
}

void TCPMessengerClient::listConnectedPeers() {
	cout << "Request connected user List" << endl;
	sendCommand(LIST_OF_PEERS);
}

void TCPMessengerClient::listOfAllpeers() {
	cout << "Request all users List" << endl;
	sendCommand(LIST_OF_All_PEERS);
}
/**
 * close active session
 */
bool TCPMessengerClient::closeActiveSession(string user_name,string peer) {
	sendCommand(CLOSE_SESSION_WITH_PEER);
	if(availableConnect.size()>0){
	sessionActive = false;
	peerAddress = "";
	if(peer=="ALL")
	availableConnect.clear(); //clean the map
	else availableConnect.erase(peer);//clean my name from the map in server
	sendData(user_name);
	sendData(peer);
	}
	else
		cout<<"no session to close available"<<endl;
	return true;
}

/**
 * disconnect from messenger server
 */
bool TCPMessengerClient::disconnect() {
	if (socket != NULL) {
		if (sessionActive)
			closeActiveSession(this->getUserName(),"ALL");
		this->sendCommand(0);
		socket->cclose();
		this->waitForThread();
		return true;
	}
	return false;
}

/**
 * send the given message to the connected peer
 */
bool TCPMessengerClient::send(string msg) {
	if (!sessionActive)
		return false;
	sendCommand(SEND_MSG_TO_PEER);
	sendData(msg);
	return true;
}

/**
 * return true if a session is active
 */
bool TCPMessengerClient::isActiveClientSession() {
	return sessionActive;
}

/**
 * return true if connected to the server
 */
bool TCPMessengerClient::isConnected() {
	return connected;
}

/**
 * read incoming command
 */
int TCPMessengerClient::readCommand() {
	int command;
	socket->recv((char*) &command, 4);
	command = ntohl(command);
	return command;
}

/**
 * read incoming data
 */
string TCPMessengerClient::readDataFromPeer() {
	string msg;
	char buff[1500];
	int msgLen;
	socket->recv((char*) &msgLen, 4);
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen) {
		rc = socket->recv((char*) &buff[totalrc], msgLen - totalrc);
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
		socket->cclose();
	}
	return msg;
}

/**
 * send given command
 */
void TCPMessengerClient::sendCommand(int command) {
	command = htonl(command);
	socket->send((char*) &command, 4);
}

/**
 * send given message
 */
void TCPMessengerClient::sendData(string msg) {
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	socket->send((char*) &msgLen, 4);
	socket->send(msg.data(), msg.length());
}
void TCPMessengerClient::listOfAllMyPeers(string user, string ip) {
	availableConnect[user] = ip;
}
string TCPMessengerClient::fromIpToName(string name) {
	UsersIP::iterator iter = availableConnect.find(name);
	UsersIP::iterator end = availableConnect.end();
	if (iter == end) {
		return NULL;
	}
	return (*iter).second;

}
string TCPMessengerClient::fromNameToIp(string ip) {
	UsersIP::iterator iter = availableConnect.begin();
	UsersIP::iterator end = availableConnect.end();
	for (; iter != end; iter++) {
		if ((*iter).second == ip) {
			return (*iter).first;
		}
	}
	return NULL;
}
void TCPMessengerClient::currentStatusOfTheClient(string user) {
	if (log_in) {
		sendCommand(CURRENT_STATUS);
		sendData(user);

	} else
		cout << "you need to log in first" << endl;
}

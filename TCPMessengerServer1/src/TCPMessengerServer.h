//============================================================================
// Name        : TCP Messenger Server
// Author      : Eldar Yaacobi
// Version     :
// Copyright   :
// Description : TCP Messenger application
//============================================================================
#include <strings.h>
#include <map>
#include <vector>
#include <utility>
#include "TCPSocket.h"
#include "MultipleTCPSocketsListener.h"
#include "TCPMessengerProtocol.h"
#include "MThread.h"
using namespace std;

class TCPMessengerServer;

/**
 * The session broker class manages a session between two peers.
 */
class TCPSessionBroker: public MThread{
	TCPMessengerServer* messenger;
	TCPSocket* peer1;
	TCPSocket* peer2;
public:
	TCPSessionBroker(TCPMessengerServer* mesgr, TCPSocket* p1,TCPSocket* p2);
	void run();
};

/**
 * The dispatcher server reads incoming commands from open peers and performs the required operations
 */
class PeersRequestsDispatcher: public MThread{
	TCPMessengerServer* messenger;
public:
	/**
	 * constructor that receive a reference to the parent messenger server
	 */
	PeersRequestsDispatcher(TCPMessengerServer* mesgr);

	/**
	 * The Dispatcher main loop
	 */
	void run();
};

class TCPMessengerServer: public MThread{
	friend class PeersRequestsDispatcher;
	friend class TCPSessionBroker;

	PeersRequestsDispatcher* dispatcher;
	TCPSocket* tcpServerSocket;
	bool running;

	typedef map<string, TCPSocket*> tOpenedPeers;//<ip:port,TCPSocket*>
	typedef map<string, string> UsersIP;//<user name,ip:port>

	tOpenedPeers oflinePeers;
	tOpenedPeers onlinePeers;


	UsersIP onlineUsersList;
	UsersIP oflineUsersList;
	typedef vector< pair<string,string> >sessionList;//<sender,Receiver>

    sessionList listOfSessions;
	FILE * file;
    ssize_t read;

public:
	/**
	 * Construct a TCP server socket
	 */
	TCPMessengerServer();

	/**
	 * the messenger server main loop- this loop waits for incoming clients connection,
	 * once a connection is established the new connection is added to the openedPeers
	 * on which the dispatcher handle further communication
	 */
	void run();

	/**
	 * print out the list of connected clients (for debug)
	 */
	void listPeers();

	/**
	 * close the server
	 */
	void close();
	/**
		 * get peers list
		 */
	 string PeersList(int choice=7);
	 /**
	 	 *return user name and ip list
	 	 */
	 string userIPList(int choice=7);
	 /**
	 	 * get offline peers vector list
	 	 */
	 vector<TCPSocket*> getOfflinePeersVec();
	 /**
	 	 *check if user exist and if he is not, put the user name and password into the file
	 	 */
	 string checkFile(string user,string pass);
	 /**
	 	 *check if user and password exist in a file
	 	 */
	 bool chackIfUserExist(string user,string pass);
	 /**
	 	 *add user to online list
	 	 */
	 void addUserToOnlineList(TCPSocket * peer, string userName);
	 /**
	 	 *add user to offline list
	 	 */
	 void addUserToOfflineList(TCPSocket * peer,string userName);
	 /**
	 	 *add user to to open sessions list
	 	 */
	 void addToopenSessions(string client,string host);
	 /**
	 	 * remove from open sessions list
	 	 */
	 void removeFromOpenSessions(string client,string host);
	 /**
	 	 *  check session status of user
	 	 */
	 string sessionStatus(string user);
	 /**
	 	 * print all the users that exist in the messenger
	 	 */
	 void printRegisteredUsers();
	 /**
	 	 	 * print list of open sessions
	 	 	 */
	 void printSessions();

bool checkLogIn(string user,int chose);

private:

	/**
	 * returns the open peers in a vector
	 */
	vector<TCPSocket*> getOnlinePeersVec();

	/**
	 * return the open peer that matches the given ip (IP:port)
	 * returns NULL if there is no match to the given name
	 */
	TCPSocket* getAvailablePeerByIpAndPort(string ipAndPort);

	/**
	 * return the open peer that matches the given name

	 */
	TCPSocket* getAvailablePeerByName(string peerName);
	/**
		 * get ip and port by peer name
		 */
	string getIpAndPortByPeerName(string peerName);
	/**
		 * get name by ip and port return string
		 */
	string getPeerNameByIpAndPort(string ipAndPort);
	/**
			 * Disconnect the peer
			 */
	void peerDisconnect(TCPSocket* peer);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void addPeerToOflineList(TCPSocket* peer);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void addPeerToOnlineList(TCPSocket* peer);

	/**
	 * read command from peer
	 */
	int readCommandFromPeer(TCPSocket* peer);

	/**
	 * read data from peer
	 */
	string readDataFromPeer(TCPSocket* peer);

	/**
	 * send command to peer
	 */
	void sendCommandToPeer(TCPSocket* peer, int command);

	/**
	 * send data to peer
	 */
	void sendDataToPeer(TCPSocket* peer, string msg);
};


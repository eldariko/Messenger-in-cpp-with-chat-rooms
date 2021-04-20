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
#include "MThread.h"
#include "TCPSocket.h"
#include "TCPMessengerProtocol.h"

using namespace std;

/**
 * The TCP Messenger client class
 */
class TCPMessengerClient: public MThread {
	TCPSocket* socket;
	typedef map<string, string> UsersIP;
	bool running;
	bool connected; //indicate that there is an active connection to the server
	bool registr;
	bool online;
	bool log_in;
	string serverAddress; // the connected server address
	bool sessionActive; // indicate that a session with a peer is active
	string peerAddress; // the session peer address
	UsersIP availableConnect; //list of client can connect with me
	string userName;
public:
	/**
	 * initialize all properties
	 */
	TCPMessengerClient();

	/**
	 * client receive loop, activated once a connection to the server is established
	 */
	void run();

	/**
	 register to server
	 */
	void registeR(string user_name, string password);
	/**
	 * connect online to the given server ip (the port is defined in the protocol header file)
	 */
	bool login(string user_name, string pass);
	/**
	 * connect to the given server ip (the port is defined in the protocol header file)
	 */
	bool connect(string ip);

	bool connectOnline(string user_name, string pass);

	/**
	 * open session with the given peer address (ip:port)
	 */
	bool open(string address);
	/**
	 * get a list of client peer from the server
	 */
	void listConnectedPeers();
	/**
	 * get a list of all the peers from the server
	 */
	void listOfAllMyPeers(string user, string ip);
	/**
	 *print the current status of the specific client
	 */
	void currentStatusOfTheClient(string user);

	void listOfAllpeers();
	/**
	 * close active session
	 */

	bool closeActiveSession(string user_name, string peer);

	/**
	 * disconnect from messenger server
	 */
	bool disconnect();

	/**
	 * send the given message to the connected peer
	 */
	bool send(string msg);

	/**
	 * return true if a session is active
	 */
	bool isActiveClientSession();

	/**
	 * return true if connected to the server
	 */
	bool isConnected();

	string fromIpToName(string name);
	string fromNameToIp(string ip);

	const string& getUserName() const {
		return userName;
	}

	void setUserName(const string& userName) {
		this->userName = userName;
	}

private:

	/**
	 * read incoming command
	 */
	int readCommand();

	/**
	 * read incoming data
	 */
	string readDataFromPeer();

	/**
	 * send given command
	 */
	void sendCommand(int command);

	/**
	 * send given message
	 */
	void sendData(string msg);
};


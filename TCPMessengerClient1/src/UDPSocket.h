/*
 * UDPSocket.h
 *
 *  Created on: Feb 6, 2013
 *      Author: Eldar Yaacobi
 */

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <netinet/in.h>
#include <inttypes.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>
#define UDP_MSNGR_PORT 9999
using namespace std;

class UDPSocket{
	struct sockaddr_in  s_in;
	struct sockaddr_in from;
	unsigned int fsize;
	int socket_fd;

public:
	UDPSocket(int port = UDP_MSNGR_PORT);
	int recv(char* buffer, int length);
	int sendTo(string msg, string ip, int port);
	int reply(string msg);
	void cclose();
	string fromAddr();
};

#endif /* UDPSOCKET_H_ */

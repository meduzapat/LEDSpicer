/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Emiter.cpp
 * @since		Jul 8, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Emitter.hpp"

using namespace LEDSpicer;
int main(int argc, char **argv) {

	string commandline;

	if (argc < 2) {
		cout << "invalid input" << endl;
		return 0;
	}

	Message msg;
	try {
		msg.type = static_cast<Message::Types>(std::stoi(argv[1]));
	}
	catch (...) {
		cout << "Invalid message received" << endl;
		return false;
	}

	for (int i = 2; i < argc; i++) {
		msg.data.push_back(argv[i]);
	}

	int sockfd;
	addrinfo
		hints,
		* servinfo,
		* p;

	int rv;
	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo("127.0.0.1", "16161", &hints, &servinfo)) != 0)
		throw Error(gai_strerror(rv));

	// loop through all the results and make a socket
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			cout << "Emitter: socket" << endl;
			continue;
		}

		break;
	}

	freeaddrinfo (servinfo);

	if (p == NULL)
		throw Error("Emitter: failed to create socket\n");

	if ((numbytes = ::sendto(sockfd, msg.toString().c_str(), msg.toString().size(), 0, p->ai_addr, p->ai_addrlen)) == -1)
		throw Error("Emitter: sendto");


	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	close (sockfd);

	return 0;


}

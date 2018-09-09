/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file		Socks.cpp
 * @ingroup
 * @since		Jul 8, 2018
 * @author		Patricio A. Rossi (MeduZa)
 * @copyright	Copyright © 2018 Patricio A. Rossi (MeduZa)
 */

#include "Socks.hpp"

#define BUFFER_SIZE 1024
#include <unistd.h>     //for close

#include <sys/socket.h> // For socket(), connect(), send(), and recv()
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

using namespace LEDSpicer;

Socks::Socks(const string& hostAddress, const string& hostPort, bool bind) {
	prepare(hostAddress, hostPort, bind);
}

void Socks::prepare(const string& hostAddress, const string& hostPort, bool bind) {

	if (sockFB)
		throw Error("Already prepared.");

	if (bind)
		Log::debug("Binding " + hostAddress + " port " + hostPort);
	else
		Log::debug("Connecting to " + hostAddress + " port " + hostPort);

	addrinfo
		hints,
		* serverInfo,
		* result;

	memset((char *)&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;  // AF_UNIX
	hints.ai_socktype = SOCK_DGRAM;  //SOCK_STREAM | SOCK_NONBLOCK;
//	hints.ai_protocol = IPPROTO_TCP; //IPPROTO_UDP;
	if (bind)
		hints.ai_flags = AI_PASSIVE;  //AI_CANONNAME;

	if (int errcode = getaddrinfo(hostAddress.c_str(), hostPort.c_str(), &hints, &result))
		throw Error("Unable to find the server " + hostAddress + ", port " + hostPort + " " + gai_strerror(errcode));

	for (serverInfo = result; serverInfo != nullptr; serverInfo = serverInfo->ai_next) {
		sockFB = socket(
			serverInfo->ai_family,
			serverInfo->ai_socktype,
			serverInfo->ai_protocol
		);

		if (sockFB == -1) {
			Log::debug("Failed to create socket " + string(strerror(errno)));
			continue;
		}
		int errcode = 0;
		if (bind) {
			errcode = ::bind(sockFB, serverInfo->ai_addr, serverInfo->ai_addrlen);
			if (not errcode)
				break;
			Log::debug(string(gai_strerror(errcode)));
		}
		else {
			errcode = ::connect(sockFB, serverInfo->ai_addr, serverInfo->ai_addrlen);
			if (not errcode)
				break;
			Log::debug(string(gai_strerror(errcode)));
		}
		::close(sockFB);
	}

	if (serverInfo == nullptr) {
		if (bind)
			throw Error("Failed to bind to local port");
		else
			throw Error("Failed to connect to local port");
	}

	freeaddrinfo(result);
}

bool Socks::send(const string& message) throw() {

	if (not sockFB)
		return false;

	if (message.empty())
		return true;

	return (message.length() == ::send(sockFB, message.c_str(), message.length(), 0));
}

bool Socks::recive(string& buffer) {

	if (not sockFB)
		return false;

	buffer.clear();
//	buffer.reserve(BUFFER_SIZE);

	int on = 1;
	ssize_t n = 0;

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(sockFB, &readset);

	ioctl(sockFB, FIONBIO, (char*) &on);

	// Check if there is any data to read.
	n = select(sockFB + 1, NULL, &readset, NULL, NULL);
	if (n <= 0)
		// There is nothing to receive (or error).
		return false;

	char* bufferp = new char[BUFFER_SIZE];
	n = recv(sockFB, bufferp, BUFFER_SIZE, 0);// MSG_DONTWAIT
	if (n >= 0) {
		buffer = bufferp;
		return true;
	}
	return false;
//	delete[] bufferp;
}

void Socks::disconnect() {
	if (sockFB) {
		Log::debug("Closing network connection");
		::close(sockFB);
		sockFB = 0;
	}
}

Socks::~Socks() {
	disconnect();
}


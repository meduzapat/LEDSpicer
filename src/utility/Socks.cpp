/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Socks.cpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2025 Patricio A. Rossi (MeduZa)
 *
 * @copyright LEDSpicer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @copyright LEDSpicer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Socks.hpp"

#define BUFFER_SIZE 1024
// for close
#include <unistd.h>

// For socket(), connect(), send(), and recv()
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

using namespace LEDSpicer;

Socks::Socks(const string& hostAddress, const string& hostPort, bool bind) {
	if (not hostPort.empty())
		prepare(hostAddress, hostPort, bind);
}

void Socks::prepare(const string& hostAddress, const string& hostPort, bool bind, int sockType) {

	if (sockFB)
		throw Error("Already prepared.");

	if (bind) {
		LogInfo("Listening on " + hostAddress + " port " + hostPort);
	}
	else {
		LogInfo("Connecting to " + hostAddress + " port " + hostPort);
	}

	addrinfo
		hints,
		* serverInfo,
		* result;

	memset(reinterpret_cast<char*>(&hints), 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC; // AF_UNIX
	hints.ai_socktype = sockType; // SOCK_DGRAM, SOCK_STREAM | SOCK_NONBLOCK;
	if (bind)
		hints.ai_flags = AI_PASSIVE; //AI_CANONNAME;

	if (int errcode = getaddrinfo(hostAddress.c_str(), hostPort.c_str(), &hints, &result))
		throw Error("Unable to find the server " + hostAddress + ", port " + hostPort + " " + gai_strerror(errcode));

	for (serverInfo = result; serverInfo != nullptr; serverInfo = serverInfo->ai_next) {
		sockFB = socket(
			serverInfo->ai_family,
			serverInfo->ai_socktype,
			serverInfo->ai_protocol
		);

		if (sockFB == -1) {
			LogDebug("Failed to create socket " + string(strerror(errno)));
			continue;
		}
		int errcode = 0;
		if (bind)
			errcode = ::bind(sockFB, serverInfo->ai_addr, serverInfo->ai_addrlen);
		else
			errcode = ::connect(sockFB, serverInfo->ai_addr, serverInfo->ai_addrlen);
		if (not errcode)
			break;
		LogDebug(string(gai_strerror(errcode)));
		::close(sockFB);
		sockFB = 0;
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
#ifdef DEVELOP
	LogDebug("Message sent: [" + message + "]");
#endif
	return (message.length() + 1 == ::send(sockFB, (message + '\0').c_str(), message.length() + 1, 0));
}

bool Socks::recive(string& buffer) {

	if (not sockFB)
		return false;

	buffer.clear();

	int on = 1;
	ssize_t n = 0;

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(sockFB, &readset);

	ioctl(sockFB, FIONBIO, reinterpret_cast<char*>(&on));

	// Check if there is any data to read.
	n = select(sockFB + 1, NULL, &readset, NULL, NULL);
	if (n <= 0)
		// There is nothing to receive (or error).
		return false;

	char bufferp[BUFFER_SIZE];
	n = recv(sockFB, &bufferp, BUFFER_SIZE, 0);// MSG_DONTWAIT
	if (n >= 0) {
		buffer = bufferp;
#ifdef DEVELOP
		LogDebug("Message received: [" + buffer + "]");
#endif
		return true;
	}
	return false;
}

void Socks::disconnect() {
	if (sockFB) {
		LogInfo("Closing network connection");
		::close(sockFB);
		sockFB = 0;
	}
}

Socks::~Socks() {
	disconnect();
}


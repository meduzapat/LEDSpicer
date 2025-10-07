/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      Socks.cpp
 * @since     Jul 8, 2018
 * @author    Patricio A. Rossi (MeduZa)
 *
 * @copyright Copyright © 2018 - 2026 Patricio A. Rossi (MeduZa)
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

using namespace LEDSpicer::Utilities;

Socks::Socks(const string& hostAddress, const string& hostPort, bool bind) {
	if (not hostPort.empty())
		prepare(hostAddress, hostPort, bind);
}

void Socks::prepare(const string& hostAddress, const string& hostPort, bool bind, int sockType) {

	if (socketFB) throw Error("Already prepared.");

	if (bind) {
		// Server Mode.
		LogInfo("Listening on " + hostAddress + " port " + hostPort);
	}
	else {
		// Client Mode.
		LogInfo("Connecting to " + hostAddress + " port " + hostPort);
	}

	addrinfo
		hints,
		* serverInfo,
		* result;

	memset(reinterpret_cast<char*>(&hints), 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC; // AF_UNIX
	hints.ai_socktype = sockType; // SOCK_DGRAM, SOCK_STREAM | SOCK_NONBLOCK;
	if (bind) hints.ai_flags = AI_PASSIVE; //AI_CANONNAME;

	if (int errcode = getaddrinfo(hostAddress.c_str(), hostPort.c_str(), &hints, &result))
		throw Error("Unable to find the server ") << hostAddress << ", port " << hostPort << " " << gai_strerror(errcode);

	for (serverInfo = result; serverInfo != nullptr; serverInfo = serverInfo->ai_next) {
		socketFB = socket(
			serverInfo->ai_family,
			serverInfo->ai_socktype,
			serverInfo->ai_protocol
		);

		if (socketFB == -1) {
			LogDebug("Failed to create socket " + string(strerror(errno)));
			continue;
		}

		int errcode = bind ?
			::bind(socketFB, serverInfo->ai_addr, serverInfo->ai_addrlen) :
			::connect(socketFB, serverInfo->ai_addr, serverInfo->ai_addrlen);

		if (not errcode) break;
		LogDebug("Socket error: " + string(strerror(errno)));
		::close(socketFB);
		socketFB = 0;
	}

	freeaddrinfo(result);

	if (serverInfo == nullptr) {
		if (bind)
			throw Error("Failed to bind to local port");
		else
			throw Error("Failed to connect to local port");
	}
}

bool Socks::send(const string& message) noexcept {

	if (not socketFB) return false;
	if (message.empty()) return true;

	string data = message + '\0';
	const char* ptr = data.c_str();
	size_t
		totalSent = 0,
		totalSize = data.size();
	while (totalSent < totalSize) {
		ssize_t sent = ::send(socketFB, ptr + totalSent, totalSize - totalSent, 0);
		if (sent <= 0) return false;
		totalSent += sent;
	}
#ifdef DEVELOP
	LogDebug("Message sent: [" + message + "]");
#endif
	return true;
}

bool Socks::receive(string& buffer) noexcept {
	if (!socketFB) return false;

	buffer.clear();

	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(socketFB, &readset);
	int ready = select(socketFB + 1, &readset, nullptr, nullptr, &tv);
	// Check for available data
	if (ready <= 0) return false;

	// Check pending data size (UDP specific).
	size_t pending = 0;
	if (ioctl(socketFB, FIONREAD, &pending) < 0) return false;
	if (pending == 0) return false;

	// Dynamically allocate buffer based on pending size.
	string temp;
	temp.resize(pending);
	ssize_t n = recv(socketFB, &temp[0], pending, 0);

	if (n <= 0) return false;

	// Assume null-terminated; trim null if present.
	if (n > 0 && temp[n - 1] == '\0') --n;
	temp.resize(n);
	buffer = std::move(temp);

#ifdef DEVELOP
	LogDebug("Message received: [" + buffer + "]");
#endif

	return true;
}

void Socks::disconnect() {
	if (socketFB) {
		LogInfo("Closing network connection");
		::close(socketFB);
		socketFB = 0;
	}
}

Socks::~Socks() {
	disconnect();
}


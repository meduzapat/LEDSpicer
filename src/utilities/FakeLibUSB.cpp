/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FakeLibUSB.cpp
 * @since     Aug 23, 2025
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

#include "FakeLibUSB.hpp"

using LEDSpicer::Utilities::Log;
using LEDSpicer::Utilities::Utility;

bool fakeFailInit = false;
bool fakeFailOpen = false;

std::vector<libusb_device*> fakeDevices;

std::vector<FakeTransfer> fakeTransfers;

int libusb_init(libusb_context** ctx) {
	LogNotice("Using a Fake libusb");
	if (fakeFailInit) return LIBUSB_ERROR_IO;
	*ctx = new libusb_context();
	return LIBUSB_SUCCESS;
}

void libusb_exit(libusb_context* ctx) {
	delete ctx;
}

ssize_t libusb_get_device_list(libusb_context*, libusb_device*** list) {
	*list = new libusb_device*[fakeDevices.size() + 1];  // +1 for null terminator.
	for (size_t i = 0; i < fakeDevices.size(); ++i) {
		(*list)[i] = fakeDevices[i];
	}
	(*list)[fakeDevices.size()] = nullptr;
	return fakeDevices.size();
}

void libusb_free_device_list(libusb_device** list, int) {
	// Don't free fakeDevices; managed externally.
	delete[] list;
}

int libusb_get_device_descriptor(libusb_device* dev, libusb_device_descriptor* desc) {
	*desc = dev->descriptor;
	return LIBUSB_SUCCESS;
}

int libusb_get_active_config_descriptor(libusb_device* dev, libusb_config_descriptor** config) {
	if (not dev->configuration) return LIBUSB_ERROR_NOT_FOUND;
	*config = const_cast<libusb_config_descriptor*>(dev->configuration);
	return LIBUSB_SUCCESS;
}

void libusb_free_config_descriptor(libusb_config_descriptor*) {
	// Don't free; the configuration is owned by the fake device.
}

libusb_device *libusb_get_device(libusb_device_handle *dev_handle) {
	LogDebug("Fake libusb_get_device called");
	if (not dev_handle) return nullptr;
	return dev_handle->associated_dev;
}

int libusb_open(libusb_device* dev, libusb_device_handle** handle) {
	if (fakeFailOpen or not dev) return LIBUSB_ERROR_IO;
	*handle = new libusb_device_handle();
	(*handle)->associated_dev = dev;
	return LIBUSB_SUCCESS;
}

int libusb_claim_interface(libusb_device_handle*, int) {
	return LIBUSB_SUCCESS;
}

int libusb_release_interface(libusb_device_handle*, int) {
	return LIBUSB_SUCCESS;
}

int libusb_reset_device(libusb_device_handle*) {
	return LIBUSB_SUCCESS;
}

void libusb_close(libusb_device_handle* handle) {
	delete handle;
}

int libusb_control_transfer(
	libusb_device_handle* handle,
	uint8_t request_type,
	uint8_t b_request,
	uint16_t w_value,
	uint16_t w_index,
	unsigned char* data,
	uint16_t w_length,
	unsigned int timeout
) {
	// Reads are served from the report descriptor the test gave the device.
	if (request_type & LIBUSB_ENDPOINT_IN) {
		const std::vector<uint8_t>& report = handle->associated_dev->reportDescriptor;
		const uint16_t size = w_length < report.size() ? w_length : report.size();
		std::copy(report.begin(), report.begin() + size, data);
		LogDebug("Fake Read: wValue: " + Utility::hex2str(w_value) +
			" wIndex: " + Utility::hex2str(w_index) + " wLength: " + std::to_string(size));
		return size;
	}
	LogDebug("Fake Transfer: wValue: " + Utility::hex2str(w_value) +
		" wIndex: " + Utility::hex2str(w_index) + " wLength: " + std::to_string(w_length));
	fakeTransfers.push_back({w_value, w_index, std::vector<uint8_t>(data, data + w_length)});
	// Fake success: "transferred" all bytes.
	return w_length;
}

int libusb_interrupt_transfer(
	libusb_device_handle* dev_handle,
	unsigned char endpoint,
	unsigned char* data,
	int length,
	int* actual_length,
	unsigned int timeout
) {
	LogDebug("Fake Interrupt Transfer: endpoint: " + Utility::hex2str(endpoint) +
		" length: " + std::to_string(length) + " timeout: " + std::to_string(timeout));
	if (actual_length)
		*actual_length = length;
	return LIBUSB_SUCCESS;
}

int libusb_set_option(libusb_context*, int, ...) {
	return LIBUSB_SUCCESS;
}

int libusb_set_auto_detach_kernel_driver(libusb_device_handle*, int) {
	return LIBUSB_SUCCESS;
}

const char* libusb_error_name(int) {
	return "FAKE_ERROR";
}

uint8_t libusb_get_bus_number(libusb_device* dev) {
	return dev->bus;
}

uint8_t libusb_get_device_address(libusb_device*) {
	return 1; // stable fake value
}

int libusb_get_port_numbers(
	libusb_device* dev,
	uint8_t* port_numbers,
	int port_numbers_len
) {
	if (!port_numbers || port_numbers_len <= 0) {
		return LIBUSB_ERROR_IO;
	}

	const int size = static_cast<int>(dev->ports.size()) < port_numbers_len ?
		static_cast<int>(dev->ports.size()) : port_numbers_len;
	std::copy(dev->ports.begin(), dev->ports.begin() + size, port_numbers);
	return size;
}

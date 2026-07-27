/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file      FakeLibUSB.hpp
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

#include "Log.hpp"
#include "Utility.hpp"

#pragma once

// Fake types (minimal stubs for libusb structs).
struct libusb_context {};
struct libusb_device_descriptor {
	uint16_t idVendor  = 0x1234;
	uint16_t idProduct = 0xABCD;
	uint16_t bcdDevice = 0x40;
};
struct libusb_interface_descriptor {
	uint8_t                     bInterfaceNumber = 0;
	const unsigned char*        extra            = nullptr;
	int                         extra_length     = 0;
};
struct libusb_interface {
	const libusb_interface_descriptor* altsetting = nullptr;
};
struct libusb_config_descriptor {
	uint8_t                 bNumInterfaces = 0;
	const libusb_interface* interface      = nullptr;
};
/// Tests populate these; every stub reads the device it was handed instead of a global.
struct libusb_device {
	libusb_device_descriptor        descriptor;
	uint8_t                         bus   = 1;
	std::vector<uint8_t>            ports {1};
	const libusb_config_descriptor* configuration = nullptr;
	std::vector<uint8_t>            reportDescriptor;
};
struct libusb_device_handle {
	libusb_device* associated_dev = nullptr;
};

// Constants (copied from real libusb for compatibility).
enum {
	LIBUSB_SUCCESS = 0,
	LIBUSB_ERROR_IO = -1,
	LIBUSB_ERROR_INVALID_PARAM = -2,
	LIBUSB_ERROR_ACCESS = -3,
	LIBUSB_ERROR_NO_DEVICE = -4,
	LIBUSB_ERROR_NOT_FOUND = -5,
	LIBUSB_ERROR_BUSY = -6,
	LIBUSB_ERROR_TIMEOUT = -7,
	LIBUSB_ERROR_OVERFLOW = -8,
	LIBUSB_ERROR_PIPE = -9,
	LIBUSB_ERROR_INTERRUPTED = -10,
	LIBUSB_ERROR_NO_MEM = -11,
	LIBUSB_ERROR_NOT_SUPPORTED = -12,
	LIBUSB_ERROR_OTHER = -99
};
#define LIBUSB_ENDPOINT_OUT        0x00
#define LIBUSB_ENDPOINT_IN         0x80
#define LIBUSB_REQUEST_TYPE_CLASS  0x20
#define LIBUSB_RECIPIENT_INTERFACE 0x01

#define LIBUSB_REQUEST_GET_DESCRIPTOR 0x06

// Log levels (reuse from libusb, but stubbed).
#define LIBUSB_LOG_LEVEL_NONE    0
#define LIBUSB_LOG_LEVEL_ERROR   1
#define LIBUSB_LOG_LEVEL_WARNING 2
#define LIBUSB_LOG_LEVEL_INFO    3
#define LIBUSB_LOG_LEVEL_DEBUG   4

// Options
#define LIBUSB_OPTION_LOG_LEVEL 1

// Fake globals for test control (e.g., set in tests to simulate failures).
extern bool fakeFailInit;
extern bool fakeFailOpen;
extern std::vector<libusb_device*> fakeDevices;  // Pre-populate in tests.

/// One recorded outgoing control transfer.
struct FakeTransfer {
	uint16_t             wValue;
	uint16_t             wIndex;
	std::vector<uint8_t> data;
};
/// Every outgoing transfer, in order, so tests can assert the wire format.
extern std::vector<FakeTransfer> fakeTransfers;

// Stub function declarations.
int libusb_init(libusb_context** ctx);
void libusb_exit(libusb_context* ctx);
ssize_t libusb_get_device_list(libusb_context* ctx, libusb_device*** list);
void libusb_free_device_list(libusb_device** list, int unref_devices);
int libusb_get_device_descriptor(libusb_device* dev, libusb_device_descriptor* desc);
int libusb_get_active_config_descriptor(libusb_device* dev, libusb_config_descriptor** config);
void libusb_free_config_descriptor(libusb_config_descriptor* config);
libusb_device* libusb_get_device(libusb_device_handle* dev_handle);
int libusb_open(libusb_device* dev, libusb_device_handle** handle);
int libusb_claim_interface(libusb_device_handle* handle, int interface_number);
int libusb_release_interface(libusb_device_handle* handle, int interface_number);
int libusb_reset_device(libusb_device_handle* handle);
void libusb_close(libusb_device_handle* handle);
int libusb_control_transfer(
	libusb_device_handle* handle,
	uint8_t request_type,
	uint8_t b_request,
	uint16_t w_value,
	uint16_t w_index,
	unsigned char* data,
	uint16_t w_length,
	unsigned int timeout
);
int libusb_interrupt_transfer(
	libusb_device_handle* dev_handle,
	unsigned char endpoint,
	unsigned char* data,
	int length,
	int* actual_length,
	unsigned int timeout
);
int libusb_set_option(libusb_context* ctx, int option, ...);
int libusb_set_auto_detach_kernel_driver(libusb_device_handle* dev_handle, int enable);
const char* libusb_error_name(int errcode);

uint8_t libusb_get_bus_number(libusb_device* dev);
uint8_t libusb_get_device_address(libusb_device* dev);

int libusb_get_port_numbers(
	libusb_device *dev,
	uint8_t *port_numbers,
	int port_numbers_len
);
// Add more stubs as needed (e.g., set_auto_detach_kernel_driver).

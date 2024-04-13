# Coding Rules - C++ Style Guide

## Header Comments

Header comments should include the following information:

- File name
- Date of creation or last modification
- Author(s)
- Copyright notice
- License information

### Example

```cpp
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/**
 * @file	FileName.hpp
 * @since	date (ex: Jul 4, 2020)
 * @author	Author name
 *
 * @copyright Copyright © 2018 - <CURRENT_YEAR> Author name
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
```

## Includes

- Headers should be grouped logically.
- Standard library headers should be included before other headers.
- Use relative paths for project headers.
- Use guard macros to prevent multiple inclusion, they should be after includes and before definitions.

### Example

```cpp
// To handle USB devices.
#include <libusb.h>
#include "Connection.hpp"
#include "Brands.hpp"

#ifndef LSUSB_HPP_
#define LSUSB_HPP_ 1

```

## Macros

- Macros should be defined in uppercase with underscores separating words.
- Provide comments explaining the purpose and usage of macros.

### Example

```cpp
/// The request type field for the setup packet.
#define REQUEST_TYPE LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE


```

## Namespace

- Namespace should be defined for project-specific classes and functions.

### Example

```cpp
namespace LEDSpicer {}
```
Or for nesting

```cpp
namespace LEDSpicer::Devices::Adalight {}
```

## Class Declaration

- Class names should use CamelCase.
- Use `virtual` keyword for functions that are intended to be overridden.
- Overridden functions should include `override` at the end.
- Provide brief descriptions for class and member functions.

### Example

```cpp
/**
 * LEDSpicer::ClassName
 * 
 * Bref description here.
 *
 * Class full description here.
 */
class ClassName : public OtherClassName {}

```

## Member Variables

- Member variables should be declared at the top of the class.
- Use camelCase for member variables.
- You can group same type variables together.
- Document with `///` explaining the purpose.

### Example

```cpp
protected:

	/// USB device handle.
	libusb_device_handle* handle = nullptr;

	/// The value field for the setup packet.
	uint16_t wValue = 0;

	uint8_t
		/// Interface number
		interface = 0,
		/// Board ID
		boardId   = 0;

```

## Member Functions

- Use CamelCase for function names.
- Provide brief descriptions for member functions.
- Description goes after a space
- Group related functions together (e.g: connect, disconnect).

### Example

```cpp

	/**
	 * brief descriptions.
	 *
	 * Larger descriotion goes here.
	 */
	virtual void connect();

	/**
	 * Claims the interface.
	 */
	void claimInterface();

	/**
	 * Extract a list of input sources
	 *
	 * Will process the input sources node and extract all details and save it for latter, will also call any
	 * chilldren node like maps input sources ect.
	 *
	 * @param inputName current input file.
	 * @param inputNode the input node.
	 * @return an array with the sources.
	 * @throws exception if not found or is not valid.
	 */
	static vector<string> processInputSources(const string& inputName, tinyxml2::XMLElement* inputNode);

```

## Constants

- Constants should be declared using `const` or `constexpr` where possible.
- Constants should be declared in uppercase with underscores separating words.
- Provide comments using `///` explaining the purpose and usage of constants.

### Example

```cpp
	/// Default USB timeout
	const int USB_TIMEOUT = 500;
```

## Comments

- Use Basic Doxigen style.
- Use comments to explain non-trivial code segments or to provide context.
- Comments should be clear, concise, and written in English.
- Avoid redundant comments that merely repeat the code.

### Example

```cpp
/**
 * This method will be called every time a pack of data is ready for transfer.
 */
virtual void transfer() const = 0;
```

Development

For development purposes, you may include conditional compilation directives within the header comments. This is useful for including code that should only be compiled during development or debugging phases.

```cpp
#ifdef DEVELOP
    somethingForDebug();
#endif
```
These directives allow for selectively including or excluding certain sections of code based on compile-time conditions, facilitating easier debugging and testing.

## Conclusion

In addition to the previously mentioned rules, the following guidelines should be followed:

- Tabs are used for initial indentation of lines, with one tab used per indentation level. Spaces are used for other alignment within a line.
- Use `/** */` for documenting methods and functions.
- Use `///` for documenting variables and properties.
- Follow Doxygen rules for documenting parameters and all other documentation.
- Inside classes, no indentation is needed except for `public:`, `protected:`, and `private:` sections.
- Functions should provide parameters with descriptions, return value if not `void`, and `throws` if it throws any exceptions, documented with reason and type.

These rules ensure consistency and clarity throughout the codebase, making it easier to read, understand, and maintain.

# Implementation Guidelines

## File Header

- Ensure the file header includes important information such as file name, creation date, author, and copyright information. Maintain accuracy and keep it up to date.
Example:

### Example

```cpp
/**
 * @file      FileName.cpp
 * @since     date (ex: Oct 6, 2020)
 * @author    Author
 *
 * @copyright Copyright © 2018 - CURRENT_YEAR author
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
```

## Namespace Usage

- Be cautious when using `using namespace` directives. It's generally better to avoid them in header files to prevent polluting the global namespace.

### Example:

```cpp
using namespace LEDSpicer::Devices::Adalight;
```

## Indentation

- Use tabs for indentation, ensuring consistency throughout the codebase.

### Example

```cpp
// Example of consistent indentation using tabs
	void exampleFunction() {
		if (condition) {
			// Indented code block
		}
	}
```

## Function Documentation

- Document functions with Doxygen-style comments in the header file, not in the implementation file.

## Parameter Passing

- Consider passing parameters by const reference where appropriate to avoid unnecessary copies, especially for large objects.

### Example

```cpp
void someFunction(const std::vector<int>& data);
```

## Error Handling

- Ensure proper error handling throughout the code, including appropriate use of exceptions and informative error messages. If necessary, propagate exceptions to higher levels.

## Logging

- Utilize logging for debugging and monitoring. Ensure clear log messages and reasonable log levels.
- Log functions are macros, so don't use them without {} ex bad: `if (true) LogDebug("debug");` ex correct: `if (true) {LogDebug("debug");}`

### Example

```cpp
LogDebug("Something with debug level information");
LogInfo("Something with info level information");
LogNotice("Something with notice level information");
LogWarning("Something with warning level information");
LogError("Some bad errir");
```

## Conditional Compilation

- Use conditional compilation directives sparingly and only when necessary to avoid making the code harder to understand.
- There are several debugging CONSTANTS created by autotools while configuring.
	- DEVELOP to be used for development only code.
	- SHOW_OUTPUT used by hardware to display the data sent to their controllers.
	- check config,h fo more details
	- new constants need to be docummented.

### Example

```cpp
#ifdef DEVELOP
// Debug-specific code
#endif
```

## Comments
- Add comments where necessary to explain complex algorithms, non-obvious behavior, or any other information that might not be immediately clear from the code itself. Use `//` for single-line comments and `/* */` for multi-line comments.
- Do not use `///` or `/** */` except if something need to be added explisity into the documentation by doxygen.

### Example

```cpp
// This loop iterates through the vector and performs some operation
for (const auto& element : myVector) {
    // Process each element
}
/*
 * This is a large description
 * Using multiple lines.
 */
```

## Code Organization
- Keep the code well-organized with logical groupings of functions and classes. Consider splitting large files into smaller ones for better maintainability.

## Code Reusability
- Look for opportunities to refactor common code into reusable functions or classes to reduce duplication and improve maintainability.

## Code Efficiency
- Balance readability with efficiency, especially in performance-critical sections. Utilize appropriate data structures and algorithms to optimize performance.


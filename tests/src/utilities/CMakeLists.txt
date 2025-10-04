# Test Error class
add_test_executable(ErrorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ErrorTest.cpp"
	"${COMMON_SRCS}"
	""
)

# Test Utility class
add_test_executable(UtilityTest
	"${CMAKE_CURRENT_SOURCE_DIR}/UtilityTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Direction class
add_test_executable(DirectionTest
	"${CMAKE_CURRENT_SOURCE_DIR}/DirectionTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Direction.cpp"
	""
)

# Test Color class
add_test_executable(ColorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ColorTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp"
	""
)

# Test Colors class
add_test_executable(ColorsTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ColorsTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Colors.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Colorful class
add_test_executable(ColorfulTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ColorfulTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Colorful.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Colors.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Log class
add_test_executable(LogTest
	"${CMAKE_CURRENT_SOURCE_DIR}/LogTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp"
	""
)

# Test Socks class
add_test_executable(SocksTest
	"${CMAKE_CURRENT_SOURCE_DIR}/SocksTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Socks.cpp"
	""
)

# Test Message class
add_test_executable(MessageTest
	"${CMAKE_CURRENT_SOURCE_DIR}/MessageTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Message.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Messages class
add_test_executable(MessagesTest
	"${CMAKE_CURRENT_SOURCE_DIR}/MessagesTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Socks.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Messages.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Message.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Monochromatic class
add_test_executable(MonochromaticTest
	"${CMAKE_CURRENT_SOURCE_DIR}/MonochromaticTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Monochromatic.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)

# Test Serial class with dummy port
add_test_executable(SerialTest
	"${CMAKE_CURRENT_SOURCE_DIR}/SerialTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Serial.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp"
	""
)
target_compile_definitions(SerialTest PRIVATE DRY_RUN=1)

# Test Speed class
add_test_executable(SpeedTest
	"${CMAKE_CURRENT_SOURCE_DIR}/SpeedTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp"
	""
)

# Test Time class
add_test_executable(TimeTest
	"${CMAKE_CURRENT_SOURCE_DIR}/TimeTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp"
	""
)

# Test USB class (with fake libusb under DRY_RUN)
add_test_executable(USBTest
	"${CMAKE_CURRENT_SOURCE_DIR}/USBTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/USB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/FakeLibUSB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)
target_compile_definitions(USBTest PRIVATE DRY_RUN=1)

# Test XMLHelper class
add_test_executable(XMLHelperTest
	"${CMAKE_CURRENT_SOURCE_DIR}/XMLHelperTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/XMLHelper.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	"${TINYXML2_LIBRARIES}"
)

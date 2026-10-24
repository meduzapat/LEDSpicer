# Test the Ultimate I/O board, descriptor driven interface and protocol selection.
add_test_executable(UltimateTest
	"${CMAKE_CURRENT_SOURCE_DIR}/UltimateTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/devices/Ultimarc/Ultimarc.cpp;${CMAKE_SOURCE_DIR}/src/devices/DeviceUSB.cpp;${CMAKE_SOURCE_DIR}/src/devices/Device.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/USB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/FakeLibUSB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)
target_compile_definitions(UltimateTest PRIVATE DRY_RUN=1)

# Test PacLed64, guarding the pair stream that Ultimate now shares.
add_test_executable(PacLed64Test
	"${CMAKE_CURRENT_SOURCE_DIR}/PacLed64Test.cpp"
	"${CMAKE_SOURCE_DIR}/src/devices/Ultimarc/FF00SharedCode.cpp;${CMAKE_SOURCE_DIR}/src/devices/Ultimarc/Ultimarc.cpp;${CMAKE_SOURCE_DIR}/src/devices/DeviceUSB.cpp;${CMAKE_SOURCE_DIR}/src/devices/Device.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/USB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/FakeLibUSB.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp"
	""
)
target_compile_definitions(PacLed64Test PRIVATE DRY_RUN=1)

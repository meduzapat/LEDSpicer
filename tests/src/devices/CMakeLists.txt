# Test Element class
add_test_executable(ElementTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ElementTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp"
	""
)

# Test Group class
add_test_executable(GroupTest
	"${CMAKE_CURRENT_SOURCE_DIR}/GroupTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp"
	""
)

# Test Items class including Element and Group.
add_test_executable(ItemsTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ItemsTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp"
	""
)

add_subdirectory(transitions)
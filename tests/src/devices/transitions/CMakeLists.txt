# Test Transition class
add_test_executable(TransitionTest
	"${CMAKE_CURRENT_SOURCE_DIR}/TransitionTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/devices/Profile.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Transition.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/inputs/Input.cpp"
	""
)

# Test ProgressiveTransition class
add_test_executable(ProgressiveTransitionTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ProgressiveTransitionTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/devices/Profile.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Transition.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/ProgressiveTransition.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/inputs/Input.cpp"
	""
)
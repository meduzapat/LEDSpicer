# Test Transition class
add_test_executable(TransitionTest
	"${CMAKE_CURRENT_SOURCE_DIR}/TransitionTest.cpp"
	"${CMAKE_SOURCE_DIR}/tests/mocks/MockProfile.hpp;${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/devices/Profile.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Transition.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/inputs/Input.cpp"
	""
)

# Test Progressive class
add_test_executable(ProgressiveTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ProgressiveTest.cpp"
	"${CMAKE_SOURCE_DIR}/tests/mocks/MockProfile.hpp;${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/devices/Profile.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Transition.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Progressive.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/inputs/Input.cpp"
	""
)

# Test ActorDriven class
add_test_executable(ActorDrivenTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ActorDrivenTest.cpp"
	"${COMMON_SRCS};${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/devices/Profile.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/Transition.cpp;${CMAKE_SOURCE_DIR}/src/devices/transitions/ActorDriven.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Direction.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/animations/FrameActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/DirectionActor.cpp;${CMAKE_SOURCE_DIR}/src/inputs/Input.cpp"
	""
)
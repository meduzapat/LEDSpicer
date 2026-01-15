# Test Actor class
add_test_executable(ActorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/ActorTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp"
	""
)

# Test FrameActor class
add_test_executable(FrameActorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/FrameActorTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/animations/FrameActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp"
	""
)

# Test DirectionActor class
add_test_executable(DirectionActorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/DirectionActorTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/animations/DirectionActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/FrameActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Direction.cpp"
	""
)

# Test StepActor class
add_test_executable(StepActorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/StepActorTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/animations/StepActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/DirectionActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/FrameActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Speed.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Direction.cpp"
	""
)

# Test AudioActor class
add_test_executable(AudioActorTest
	"${CMAKE_CURRENT_SOURCE_DIR}/AudioActorTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/animations/AudioActor.cpp;${CMAKE_SOURCE_DIR}/src/animations/Actor.cpp;${CMAKE_SOURCE_DIR}/src/devices/Group.cpp;${CMAKE_SOURCE_DIR}/src/devices/Element.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Color.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Utility.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Log.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Direction.cpp;${CMAKE_SOURCE_DIR}/src/utilities/Time.cpp"
	""
)
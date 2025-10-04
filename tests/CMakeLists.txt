cmake_minimum_required(VERSION 3.10)
project(LEDSpicer LANGUAGES CXX)

enable_testing()

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -fexceptions)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/tests/src
	${GTEST_INCLUDE_DIRS}
)

# Define a macro to create test executables (global)
macro(add_test_executable TEST_NAME TEST_SRC EXTRA_SRCS EXTRA_LIBS)
	add_executable(${TEST_NAME}
		${TEST_SRC}
		${EXTRA_SRCS}
	)
	target_compile_definitions(${TEST_NAME} PRIVATE
		PACKAGE_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/tests/data/"
	)
	target_link_libraries(${TEST_NAME} PRIVATE
		${GTEST_LIBRARIES}
		gtest_main
		${EXTRA_LIBS}
	)
	add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# Common source files used by multiple tests (add more as tests grow)
set(COMMON_SRCS
	""
)

add_subdirectory(src)

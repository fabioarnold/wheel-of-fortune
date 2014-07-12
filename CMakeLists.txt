cmake_minimum_required(VERSION 2.6)
set(CMAKE_VERBOSE_MAKEFILE off)

project(WheelOfFortune)
set(CMAKE_BUILD_TYPE "debug")
set(BIN WheelOfFortune)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")

include_directories(
	Source
)

set(SRC
	Source/Graphics.cpp

	Source/LineObject.cpp
	Source/WheelOfFortune.cpp

	Source/main.cpp
)

find_package(SDL2)
find_package(OpenGL)

if(APPLE)
	set(PLATFORM_SPECIFIC_CXX_FLAGS "-F /Library/Frameworks -I /usr/local/include")
endif(APPLE)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-std=c++11 ${PLATFORM_SPECIFIC_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -O0 -g -DDEBUG") # -Werror -Wpedantic

#if (APPLE)
#	set(GUI_TYPE MACOSX_BUNDLE)
#	find_library(APP_SERVICES_LIBRARY ApplicationServices)
#	set(PLATFORM_SPECIFIC_LIBS ${APP_SERVICES_LIBRARY})
#endif

add_executable(${BIN} ${GUI_TYPE} ${SRC})
target_link_libraries(${BIN} ${PLATFORM_SPECIFIC_LIBS} ${SDL2_LIBRARY} ${OPENGL_LIBRARY}) # ${CMAKE_THREAD_LIBS_INIT}

add_custom_target(run ALL "./${BIN}" DEPENDS ${BIN})

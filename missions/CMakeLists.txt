# *  @Copyright (c) 2016-2017 DJI
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy
# * of this software and associated documentation files (the "Software"), to deal
# * in the Software without restriction, including without limitation the rights
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# * copies of the Software, and to permit persons to whom the Software is
# * furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in
# * all copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# * SOFTWARE.
# *
# *

cmake_minimum_required(VERSION 2.8)
project(djiosdk-mission-sample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0")

if(NOT ONBOARDSDK_SOURCE)
	set(ONBOARDSDK_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../osdk-core")
endif()

if(NOT MODULES_HEADER_DIR)
	set(MODULES_HEADER_DIR "${ONBOARDSDK_SOURCE}/modules/inc/payload")
endif()

if(NOT FLIGHT_MODULES_HEADER_DIR)
	set(FLIGHT_MODULES_HEADER_DIR "${ONBOARDSDK_SOURCE}/modules/inc/flight")
endif()

find_package(JPEG REQUIRED)

include_directories(/usr/lib)
include_directories(${ONBOARDSDK_SOURCE}/api/inc)
include_directories(${ONBOARDSDK_SOURCE}/utility/inc)
include_directories(${ONBOARDSDK_SOURCE}/hal/inc)
include_directories(${ONBOARDSDK_SOURCE}/protocol/inc)
include_directories(${ONBOARDSDK_SOURCE}/platform/linux/inc)
include_directories(${MODULES_HEADER_DIR})
include_directories(${FLIGHT_MODULES_HEADER_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../core/inc)	
FILE(GLOB SOURCE_FILES *.hpp *.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_helpers.cpp
    )

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_helpers.cpp
    main.cpp
    uart.cpp
    camera.cpp
    mission_sample.cpp)

#add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} djiosdk-core)
target_link_libraries(${PROJECT_NAME} /usr/lib/aarch64-linux-gnu/libjpeg.so)
if(WAYPT2_CORE)
  add_executable(djiosdk-waypoint-v2-sample
      ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_environment.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../common/dji_linux_helpers.cpp
      waypoint_v2_main.cpp
      waypoint_v2_sample.cpp)
  target_link_libraries(djiosdk-waypoint-v2-sample djiosdk-core JPEG REQUIRED)
endif()

cmake_minimum_required(VERSION 3.16)

project(se-native-extensions LANGUAGES CXX)

if(MSVC)
	set(CMAKE_CXX_STANDARD 20)
else()
	set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")
	message(STATUS "Downloading CPM.cmake...")
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	file(DOWNLOAD "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
		 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake"
		 STATUS status LOG log_content)
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")
		message(FATAL_ERROR "Failed to download CPM.cmake: ${status}\n${log_content}")
	endif()
endif()
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")

CPMAddPackage(
  NAME ScratchEverywhere
  GITHUB_REPOSITORY ScratchEverywhere/ScratchEverywhere
  VERSION 0.40
  GIT_TAG experiment/native-extensions
)

set(EXTENSIONS "example") # Edit this to add new extensions

foreach(EXTENSION ${EXTENSIONS})
  add_extension(${EXTENSION} "${CMAKE_CURRENT_SOURCE_DIR}/source/${EXTENSION}.cpp")
endforeach()

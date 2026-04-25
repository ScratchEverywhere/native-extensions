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
  GIT_TAG main
)

set(EXTENSIONS "example") # Edit this to add new extensions

include(CheckIncludeFile)

# Milsko is very likely in /usr/local...
list(APPEND CMAKE_REQUIRED_INCLUDES /usr/local/include)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

check_include_file(Mw/Milsko.h HAVE_MW_MILSKO_H)
if(HAVE_MW_MILSKO_H)
  list(APPEND EXTENSIONS "milsko")
endif()

foreach(EXTENSION ${EXTENSIONS})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/${EXTENSION}")
    file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/${EXTENSION}/*.cpp")
    add_extension(${EXTENSION} ${SOURCES})
  else()
    add_extension(${EXTENSION} "${CMAKE_CURRENT_SOURCE_DIR}/source/${EXTENSION}.cpp")
  endif()
  if(${EXTENSION} STREQUAL "milsko")
    target_link_libraries(${EXTENSION} PRIVATE Mw)
  endif()
endforeach()

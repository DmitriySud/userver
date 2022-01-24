# AUTOGENERATED, DON'T CHANGE THIS FILE!


if (TARGET regex)
  if (NOT regex_FIND_VERSION)
      set(regex_FOUND ON)
      return()
  endif()

  if (regex_VERSION)
      if (regex_FIND_VERSION VERSION_LESS_EQUAL regex_VERSION)
          set(regex_FOUND ON)
          return()
      else()
          message(FATAL_ERROR
              "Already using version ${regex_VERSION} "
              "of regex when version ${regex_FIND_VERSION} "
              "was requested."
          )
      endif()
  endif()
endif()

set(FULL_ERROR_MESSAGE "Could not find `regex` package.\n\tDebian: sudo apt update && sudo apt install libboost-regex-dev\n\tMacOS: brew install boost")


include(FindPackageHandleStandardArgs)

find_library(regex_LIBRARIES_boost_regex
  NAMES boost_regex
)
list(APPEND regex_LIBRARIES ${regex_LIBRARIES_boost_regex})

find_path(regex_INCLUDE_DIRS_boost_regex_config_hpp
  NAMES boost/regex/config.hpp
)
list(APPEND regex_INCLUDE_DIRS ${regex_INCLUDE_DIRS_boost_regex_config_hpp})



if (regex_VERSION)
  set(regex_VERSION ${regex_VERSION})
endif()

if (regex_FIND_VERSION AND NOT regex_VERSION)
  include(DetectVersion)

  if (UNIX AND NOT APPLE)
    deb_version(regex_VERSION libboost-regex-dev)
  endif()
  if (APPLE)
    brew_version(regex_VERSION boost)
  endif()
endif()

 
find_package_handle_standard_args(
  regex
    REQUIRED_VARS
      regex_LIBRARIES
      regex_INCLUDE_DIRS
      
    FAIL_MESSAGE
      "${FULL_ERROR_MESSAGE}"
)
mark_as_advanced(
  regex_LIBRARIES
  regex_INCLUDE_DIRS
  
)

if (NOT regex_FOUND)
  if (regex_FIND_REQUIRED)
      message(FATAL_ERROR "${FULL_ERROR_MESSAGE}. Required version is at least ${regex_FIND_VERSION}")
  endif()

  return()
endif()

if (regex_FIND_VERSION)
  if (regex_VERSION VERSION_LESS regex_FIND_VERSION)
      message(STATUS
          "Version of regex is '${regex_VERSION}'. "
          "Required version is at least '${regex_FIND_VERSION}'. "
          "Ignoring found regex."
      )
      set(regex_FOUND OFF)
      return()
  endif()
endif()

 
if (NOT TARGET regex)
  add_library(regex INTERFACE IMPORTED GLOBAL)

  if (TARGET Boost::regex)
    target_link_libraries(regex INTERFACE Boost::regex)
  endif()
  target_include_directories(regex INTERFACE ${regex_INCLUDE_DIRS})
  target_link_libraries(regex INTERFACE ${regex_LIBRARIES})
  
  # Target regex is created
endif()

if (regex_VERSION)
  set(regex_VERSION "${regex_VERSION}" CACHE STRING "Version of the regex")
endif()

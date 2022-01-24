# AUTOGENERATED, DON'T CHANGE THIS FILE!


if (TARGET Jemalloc)
  if (NOT Jemalloc_FIND_VERSION)
      set(Jemalloc_FOUND ON)
      return()
  endif()

  if (Jemalloc_VERSION)
      if (Jemalloc_FIND_VERSION VERSION_LESS_EQUAL Jemalloc_VERSION)
          set(Jemalloc_FOUND ON)
          return()
      else()
          message(FATAL_ERROR
              "Already using version ${Jemalloc_VERSION} "
              "of Jemalloc when version ${Jemalloc_FIND_VERSION} "
              "was requested."
          )
      endif()
  endif()
endif()

set(FULL_ERROR_MESSAGE "Could not find `Jemalloc` package.\n\tDebian: sudo apt update && sudo apt install libyandex-taxi-jemalloc-dev\n\tMacOS: brew install jemalloc")

if (SANITIZE)
  message(FATAL_ERROR "jemalloc is not compatible with sanitizers,  please skip it for SANITIZE-enabled builds")
endif()

include(FindPackageHandleStandardArgs)

find_library(Jemalloc_LIBRARIES_jemalloc
  NAMES jemalloc
)
list(APPEND Jemalloc_LIBRARIES ${Jemalloc_LIBRARIES_jemalloc})

find_path(Jemalloc_INCLUDE_DIRS_jemalloc_jemalloc_h
  NAMES jemalloc/jemalloc.h
)
list(APPEND Jemalloc_INCLUDE_DIRS ${Jemalloc_INCLUDE_DIRS_jemalloc_jemalloc_h})



if (Jemalloc_VERSION)
  set(Jemalloc_VERSION ${Jemalloc_VERSION})
endif()

if (Jemalloc_FIND_VERSION AND NOT Jemalloc_VERSION)
  include(DetectVersion)

  if (UNIX AND NOT APPLE)
    deb_version(Jemalloc_VERSION libyandex-taxi-jemalloc-dev)
  endif()
  if (APPLE)
    brew_version(Jemalloc_VERSION jemalloc)
  endif()
endif()

 
find_package_handle_standard_args(
  Jemalloc
    REQUIRED_VARS
      Jemalloc_LIBRARIES
      Jemalloc_INCLUDE_DIRS
      
    FAIL_MESSAGE
      "${FULL_ERROR_MESSAGE}"
)
mark_as_advanced(
  Jemalloc_LIBRARIES
  Jemalloc_INCLUDE_DIRS
  
)

if (NOT Jemalloc_FOUND)
  if (Jemalloc_FIND_REQUIRED)
      message(FATAL_ERROR "${FULL_ERROR_MESSAGE}. Required version is at least ${Jemalloc_FIND_VERSION}")
  endif()

  return()
endif()

if (Jemalloc_FIND_VERSION)
  if (Jemalloc_VERSION VERSION_LESS Jemalloc_FIND_VERSION)
      message(STATUS
          "Version of Jemalloc is '${Jemalloc_VERSION}'. "
          "Required version is at least '${Jemalloc_FIND_VERSION}'. "
          "Ignoring found Jemalloc."
      )
      set(Jemalloc_FOUND OFF)
      return()
  endif()
endif()

 
if (NOT TARGET Jemalloc)
  add_library(Jemalloc INTERFACE IMPORTED GLOBAL)

  target_include_directories(Jemalloc INTERFACE ${Jemalloc_INCLUDE_DIRS})
  target_link_libraries(Jemalloc INTERFACE ${Jemalloc_LIBRARIES})
  
  # Target Jemalloc is created
endif()

if (Jemalloc_VERSION)
  set(Jemalloc_VERSION "${Jemalloc_VERSION}" CACHE STRING "Version of the Jemalloc")
endif()

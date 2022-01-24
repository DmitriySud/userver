# AUTOGENERATED, DON'T CHANGE THIS FILE!


if (TARGET GssApi)
  if (NOT GssApi_FIND_VERSION)
      set(GssApi_FOUND ON)
      return()
  endif()

  if (GssApi_VERSION)
      if (GssApi_FIND_VERSION VERSION_LESS_EQUAL GssApi_VERSION)
          set(GssApi_FOUND ON)
          return()
      else()
          message(FATAL_ERROR
              "Already using version ${GssApi_VERSION} "
              "of GssApi when version ${GssApi_FIND_VERSION} "
              "was requested."
          )
      endif()
  endif()
endif()

set(FULL_ERROR_MESSAGE "Could not find `GssApi` package.\n\tDebian: sudo apt update && sudo apt install libkrb5-dev\n\tMacOS: brew install krb5")


include(FindPackageHandleStandardArgs)

find_library(GssApi_LIBRARIES_gssapi_krb5_gssapi
  NAMES gssapi_krb5 gssapi
)
list(APPEND GssApi_LIBRARIES ${GssApi_LIBRARIES_gssapi_krb5_gssapi})

find_path(GssApi_INCLUDE_DIRS_gssapi_h
  NAMES gssapi.h
  PATH_SUFFIXES gssapi
)
list(APPEND GssApi_INCLUDE_DIRS ${GssApi_INCLUDE_DIRS_gssapi_h})



if (GssApi_VERSION)
  set(GssApi_VERSION ${GssApi_VERSION})
endif()

if (GssApi_FIND_VERSION AND NOT GssApi_VERSION)
  include(DetectVersion)

  if (UNIX AND NOT APPLE)
    deb_version(GssApi_VERSION libkrb5-dev)
  endif()
  if (APPLE)
    brew_version(GssApi_VERSION krb5)
  endif()
endif()

 
find_package_handle_standard_args(
  GssApi
    REQUIRED_VARS
      GssApi_LIBRARIES
      GssApi_INCLUDE_DIRS
      
    FAIL_MESSAGE
      "${FULL_ERROR_MESSAGE}"
)
mark_as_advanced(
  GssApi_LIBRARIES
  GssApi_INCLUDE_DIRS
  
)

if (NOT GssApi_FOUND)
  if (GssApi_FIND_REQUIRED)
      message(FATAL_ERROR "${FULL_ERROR_MESSAGE}. Required version is at least ${GssApi_FIND_VERSION}")
  endif()

  return()
endif()

if (GssApi_FIND_VERSION)
  if (GssApi_VERSION VERSION_LESS GssApi_FIND_VERSION)
      message(STATUS
          "Version of GssApi is '${GssApi_VERSION}'. "
          "Required version is at least '${GssApi_FIND_VERSION}'. "
          "Ignoring found GssApi."
      )
      set(GssApi_FOUND OFF)
      return()
  endif()
endif()

 
if (NOT TARGET GssApi)
  add_library(GssApi INTERFACE IMPORTED GLOBAL)

  target_include_directories(GssApi INTERFACE ${GssApi_INCLUDE_DIRS})
  target_link_libraries(GssApi INTERFACE ${GssApi_LIBRARIES})
  
  # Target GssApi is created
endif()

if (GssApi_VERSION)
  set(GssApi_VERSION "${GssApi_VERSION}" CACHE STRING "Version of the GssApi")
endif()

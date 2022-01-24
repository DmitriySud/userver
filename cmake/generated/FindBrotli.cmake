# AUTOGENERATED, DON'T CHANGE THIS FILE!


if (TARGET Brotli)
  if (NOT Brotli_FIND_VERSION)
      set(Brotli_FOUND ON)
      return()
  endif()

  if (Brotli_VERSION)
      if (Brotli_FIND_VERSION VERSION_LESS_EQUAL Brotli_VERSION)
          set(Brotli_FOUND ON)
          return()
      else()
          message(FATAL_ERROR
              "Already using version ${Brotli_VERSION} "
              "of Brotli when version ${Brotli_FIND_VERSION} "
              "was requested."
          )
      endif()
  endif()
endif()

set(FULL_ERROR_MESSAGE "Could not find `Brotli` package.\n\tDebian: sudo apt update && sudo apt install libbrotli-dev\n\tMacOS: brew install brotli")


include(FindPackageHandleStandardArgs)

find_library(Brotli_LIBRARIES_brotlidec
  NAMES brotlidec
)
list(APPEND Brotli_LIBRARIES ${Brotli_LIBRARIES_brotlidec})
find_library(Brotli_LIBRARIES_brotlienc
  NAMES brotlienc
)
list(APPEND Brotli_LIBRARIES ${Brotli_LIBRARIES_brotlienc})

find_path(Brotli_INCLUDE_DIRS_brotli_decode_h
  NAMES brotli/decode.h
)
list(APPEND Brotli_INCLUDE_DIRS ${Brotli_INCLUDE_DIRS_brotli_decode_h})
find_path(Brotli_INCLUDE_DIRS_brotli_encode_h
  NAMES brotli/encode.h
)
list(APPEND Brotli_INCLUDE_DIRS ${Brotli_INCLUDE_DIRS_brotli_encode_h})



if (Brotli_VERSION)
  set(Brotli_VERSION ${Brotli_VERSION})
endif()

if (Brotli_FIND_VERSION AND NOT Brotli_VERSION)
  include(DetectVersion)

  if (UNIX AND NOT APPLE)
    deb_version(Brotli_VERSION libbrotli-dev)
  endif()
  if (APPLE)
    brew_version(Brotli_VERSION brotli)
  endif()
endif()

 
find_package_handle_standard_args(
  Brotli
    REQUIRED_VARS
      Brotli_LIBRARIES
      Brotli_INCLUDE_DIRS
      
    FAIL_MESSAGE
      "${FULL_ERROR_MESSAGE}"
)
mark_as_advanced(
  Brotli_LIBRARIES
  Brotli_INCLUDE_DIRS
  
)

if (NOT Brotli_FOUND)
  if (Brotli_FIND_REQUIRED)
      message(FATAL_ERROR "${FULL_ERROR_MESSAGE}. Required version is at least ${Brotli_FIND_VERSION}")
  endif()

  return()
endif()

if (Brotli_FIND_VERSION)
  if (Brotli_VERSION VERSION_LESS Brotli_FIND_VERSION)
      message(STATUS
          "Version of Brotli is '${Brotli_VERSION}'. "
          "Required version is at least '${Brotli_FIND_VERSION}'. "
          "Ignoring found Brotli."
      )
      set(Brotli_FOUND OFF)
      return()
  endif()
endif()

 
if (NOT TARGET Brotli)
  add_library(Brotli INTERFACE IMPORTED GLOBAL)

  target_include_directories(Brotli INTERFACE ${Brotli_INCLUDE_DIRS})
  target_link_libraries(Brotli INTERFACE ${Brotli_LIBRARIES})
  
  # Target Brotli is created
endif()

if (Brotli_VERSION)
  set(Brotli_VERSION "${Brotli_VERSION}" CACHE STRING "Version of the Brotli")
endif()

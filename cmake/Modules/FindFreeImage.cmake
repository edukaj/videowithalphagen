# Find the FreeImage library.
#
# This module defines
#  FreeImage_FOUND             - True if FreeImage was found.
#  FreeImage_INCLUDE_DIRS      - Include directories for FREEIMAGE headers.
#  FreeImage_LIBRARIES         - Libraries for FreeImage.
#
# To specify an additional directory to search, set FreeImage_ROOT.
#
# Copyright (c) 2010, Ewen Cheslack-Postava
# Based on FindSQLite3.cmake by:
#  Copyright (c) 2006, Jaroslaw Staniek, <js@iidea.pl>
#  Extended by Siddhartha Chaudhuri, 2008.
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

set(FreeImage_INCLUDE_DIRS)
set(FreeImage_LIBRARIES)

set(SEARCH_PATHS
  $ENV{ProgramFiles}/freeimage/include
  $ENV{SystemDrive}/freeimage/include
  $ENV{ProgramFiles}/freeimage
  $ENV{SystemDrive}/freeimage
  )
if(FreeImage_ROOT)
  set(SEARCH_PATHS
    ${FreeImage_ROOT}
    ${FreeImage_ROOT}/include
    ${SEARCH_PATHS}
    )
endif()

find_path(FreeImage_INCLUDE_DIRS
  NAMES FreeImage.h
  PATHS ${SEARCH_PATHS}
  NO_DEFAULT_PATH)
if(NOT FreeImage_INCLUDE_DIRS)  # now look in system locations
  find_path(FreeImage_INCLUDE_DIRS NAMES FreeImage.h)
endif(NOT FreeImage_INCLUDE_DIRS)

set(FreeImage_LIBRARY_DIRS)
if(FreeImage_ROOT)
  set(FreeImage_LIBRARY_DIRS ${FreeImage_ROOT})
  if(EXISTS "${FreeImage_ROOT}/lib")
    set(FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIRS} ${FreeImage_ROOT}/lib)
  endif()
  if(EXISTS "${FreeImage_ROOT}/lib/static")
    set(FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIRS} ${FreeImage_ROOT}/lib/static)
  endif()
endif()

# FreeeImage
# Without system dirs
find_library(FreeImage_LIBRARY
  NAMES freeimage
  PATHS ${FreeImage_LIBRARY_DIRS}
  NO_DEFAULT_PATH
  )
if(NOT FreeImage_LIBRARY)  # now look in system locations
  find_library(FreeImage_LIBRARY NAMES freeimage)
endif(NOT FreeImage_LIBRARY)


set(FreeImage_LIBRARIES)
if(FreeImage_LIBRARY)
  set(FreeImage_LIBRARIES ${FreeImage_LIBRARY})
endif()

# handle the QUIETLY and REQUIRED arguments and set FreeImage_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
  FreeImage
  REQUIRED_VARS
    FreeImage_INCLUDE_DIRS
    FreeImage_LIBRARIES
)

mark_as_advanced(FreeImage_INCLUDE_DIRS FreeImage_LIBRARIES)

# Find the FreeImage library.
#
# This module defines
#  FreeImage_FOUND             - True if FREEIMAGE was found.
#  FreeImage_INCLUDE_DIRS      - Include directories for FREEIMAGE headers.
#  FreeImage_LIBRARIES         - Libraries for FREEIMAGE.
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

SET(FreeImage_FOUND FALSE)
SET(FreeImage_INCLUDE_DIRS)
SET(FreeImage_LIBRARIES)

SET(SEARCH_PATHS
  $ENV{ProgramFiles}/freeimage/include
  $ENV{SystemDrive}/freeimage/include
  $ENV{ProgramFiles}/freeimage
  $ENV{SystemDrive}/freeimage
  )
IF(FreeImage_ROOT)
  SET(SEARCH_PATHS
    ${FreeImage_ROOT}
    ${FreeImage_ROOT}/include
    ${SEARCH_PATHS}
    )
ENDIF()

FIND_PATH(FreeImage_INCLUDE_DIRS
  NAMES FreeImage.h
  PATHS ${SEARCH_PATHS}
  NO_DEFAULT_PATH)
IF(NOT FreeImage_INCLUDE_DIRS)  # now look in system locations
  FIND_PATH(FreeImage_INCLUDE_DIRS NAMES FreeImage.h)
ENDIF(NOT FreeImage_INCLUDE_DIRS)

SET(FreeImage_LIBRARY_DIRS)
IF(FreeImage_ROOT)
  SET(FreeImage_LIBRARY_DIRS ${FreeImage_ROOT})
  IF(EXISTS "${FreeImage_ROOT}/lib")
    SET(FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIRS} ${FreeImage_ROOT}/lib)
  ENDIF()
  IF(EXISTS "${FreeImage_ROOT}/lib/static")
    SET(FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIRS} ${FreeImage_ROOT}/lib/static)
  ENDIF()
ENDIF()

# FREEIMAGE
# Without system dirs
FIND_LIBRARY(FREEIMAGE_LIBRARY
  NAMES freeimage
  PATHS ${FreeImage_LIBRARY_DIRS}
  NO_DEFAULT_PATH
  )
IF(NOT FREEIMAGE_LIBRARY)  # now look in system locations
  FIND_LIBRARY(FREEIMAGE_LIBRARY NAMES freeimage)
ENDIF(NOT FREEIMAGE_LIBRARY)

SET(FreeImage_LIBRARIES)
IF(FREEIMAGE_LIBRARY)
  SET(FreeImage_LIBRARIES ${FREEIMAGE_LIBRARY})
ENDIF()

IF(FreeImage_INCLUDE_DIRS AND FreeImage_LIBRARIES)
  SET(FreeImage_FOUND TRUE)
  IF(NOT FreeImage_FIND_QUIETLY)
    MESSAGE(STATUS "Found FreeImage: headers at ${FreeImage_INCLUDE_DIRS}, libraries at ${FreeImage_LIBRARY_DIRS} :: ${FreeImage_LIBRARIES}")
  ENDIF(NOT FreeImage_FIND_QUIETLY)
ELSE(FreeImage_INCLUDE_DIRS AND FreeImage_LIBRARIES)
  SET(FreeImage_FOUND FALSE)
  IF(FREEIMAGE_FIND_REQUIRED)
    MESSAGE(STATUS "FreeImage not found")
  ENDIF(FREEIMAGE_FIND_REQUIRED)
ENDIF(FreeImage_INCLUDE_DIRS AND FreeImage_LIBRARIES)

MARK_AS_ADVANCED(FreeImage_INCLUDE_DIRS FreeImage_LIBRARIES)


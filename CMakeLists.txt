project(videowithalphagen)
cmake_minimum_required(VERSION 3.1)

set (CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules ${CMAKE_SOURCE_DIR}/cmake/Modules)
set (CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(BOOST_MIN_VERSION "1.58.0")

find_package( Boost ${BOOST_MIN_VERSION} COMPONENTS
    program_options
    filesystem
    system    
    REQUIRED )

find_package( OpenCV REQUIRED )
find_package( FreeImage REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${FreeImage_INCLUDE_DIRS} )

add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

aux_source_directory( . SRC_LIST)
include_directories( . )

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries( ${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    ${FreeImage_LIBRARIES}
    )

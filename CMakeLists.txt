project(videowithalphagen)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( Boost 1.61 COMPONENTS
    program_options
    filesystem
    system
    REQUIRED )

find_package( OpenCV REQUIRED )
find_package( FreeImage REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${FreeImage_INCLUDE_PATH} )

include_directories( external )

add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

aux_source_directory( . SRC_LIST)
include_directories( . )

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries( ${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    ${FreeImage_LIBRARY}
    )

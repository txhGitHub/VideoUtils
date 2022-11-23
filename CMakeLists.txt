cmake_minimum_required(VERSION 3.5)

project(VideoUtils LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

#添加资源文件
file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_SOURCE_DIR}/*.qrc)

#设置静态动态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out/StaticLib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out/DynamicLib)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdLib/ffmpeg/lib)

#先编译子目录
add_subdirectory(VideoController)

#设置依赖的静态库
set(STATIC_LIBRARYS
                ${CMAKE_SOURCE_DIR}/Out/StaticLib/libvideocontroller.a
           )
#设置依赖的头文件
include_directories(${CMAKE_SOURCE_DIR}/ThirdLib/ffmpeg/include)
include_directories( ${CMAKE_SOURCE_DIR}/Utils/Mutex)
include_directories( ${CMAKE_SOURCE_DIR}/VideoController)
include_directories( ${CMAKE_SOURCE_DIR}/Utils)
include_directories( ${CMAKE_SOURCE_DIR}/OpenGL)
include_directories(${CMAKE_SOURCE_DIR}/Interface)

#对于一个头文件，例如，c.h,被静态库的头文件b.h包含，并且其他头文件包含了b.h，需要包含了c.h的cmake中加入头文件所在路径
#对于编译的静态库，如果依赖了实现，不能只添加头文件依赖，要么就把实现写在.h文件中，要么就添加cpp依赖



aux_source_directory(${CMAKE_SOURCE_DIR}/OpenGL OPENGL_SRCS)

#include_directories(${CMAKE_CURRENT_BINARY_DIR}/VideoUtils_autogen/include)


find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES VideoUtils_zh_CN.ts)


qt5_add_resources(SOURCES  ${QRC_SOURCE_FILES})

#设置工程源文件
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
       ${TS_FILES}
       ${OPENGL_SRCS}
)

if(ANDROID)
  add_library(VideoUtils SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

  )
else()
  add_executable(VideoUtils
    ${PROJECT_SOURCES}
    ${QRC_FILES}
    ${SOURCES}
  )
endif()

target_link_libraries(VideoUtils PRIVATE Qt5::Widgets ${STATIC_LIBRARYS}
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    postproc
    swresample
    swscale)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

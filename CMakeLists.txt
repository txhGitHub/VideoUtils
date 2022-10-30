cmake_minimum_required(VERSION 3.5)

project(VideoUtils LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置静态动态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out/StaticLib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Out/DynamicLib)

#先编译子目录
#add_subdirectory(ThirdLib)
add_subdirectory(Encode)
add_subdirectory(Decode)
#add_subdirectory(Utils)

#设置依赖的静态库
set(STATIC_LIBRARYS ${CMAKE_SOURCE_DIR}/Out/StaticLib/libdecode.a
                ${CMAKE_SOURCE_DIR}/Out/StaticLib/libencode.a
           )
#设置依赖的头文件
include_directories( ${CMAKE_SOURCE_DIR}/Encode)
include_directories( ${CMAKE_SOURCE_DIR}/Utils)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES VideoUtils_zh_CN.ts)

if(ANDROID)
  add_library(VideoUtils SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
  )
else()
  add_executable(VideoUtils
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
  )
endif()

target_link_libraries(VideoUtils PRIVATE Qt5::Widgets ${STATIC_LIBRARYS})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

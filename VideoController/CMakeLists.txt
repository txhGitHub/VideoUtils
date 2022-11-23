set(Module "videocontroller")

message(STATUS "Start build mode :  ${Module}")
include_directories(${CMAKE_SOURCE_DIR}/ThirdLib/ffmpeg/include)
include_directories(${CMAKE_SOURCE_DIR}/Utils)
include_directories(${CMAKE_SOURCE_DIR}/Utils/Mutex)
include_directories(${CMAKE_SOURCE_DIR}/Interface)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdLib/ffmpeg/lib)

#message(STATUS "Start build mode :  ${CMAKE_SOURCE_DIR}")
aux_source_directory(./ CURRENT_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/Utils UTILS_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/Utils/Mutex MUTEX_SRCS)

add_library(${Module} STATIC  ${CURRENT_SRCS} ${UTILS_SRCS} ${MUTEX_SRCS})

message(STATUS "End building  : " ${Module} "\n")

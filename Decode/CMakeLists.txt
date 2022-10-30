set(Module "decode")

message(STATUS "Start build mode :  ${Module}")
include_directories(${CMAKE_SOURCE_DIR}/Decode
    ${CMAKE_SOURCE_DIR}/ThirdLib/ffmpeg/include)
#find_package()
#增加链接库
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdLib/ffmpeg/lib)

#message(STATUS "Start build mode :  ${CMAKE_SOURCE_DIR}")
aux_source_directory(./ CURRENT_SRCS)

add_library(${Module} STATIC  ${CURRENT_SRCS}
        )

#install(TARGETS ${Target}
#        EXPORT ${Target}
#        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}    #动态库安装路径
#        LIBRARY DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}    #静态库安装路径
#        )

message(STATUS "End building  : " ${Module} "\n")

#设置链接库
#target_link_libraries(${Module} avcodec avdevice avfilter avformat avutil postproc swresample swscale)


include_directories(.)

aux_source_directory(. SRCS)
add_executable(main ${SRCS})
target_link_libraries(main ${OpenCV_LIBS} jsoncpp_lib)

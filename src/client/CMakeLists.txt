aux_source_directory(. SRC_LIST)

add_executable(ChatClient ${SRC_LIST})

target_link_libraries(ChatClient muduo_net muduo_base pthread mysqlclient hiredis curl)



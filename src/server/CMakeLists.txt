#定义了一个SRC_LIST变量，其中包含了该目录下所有源文件

#当前文件夹下的cpp->SRC_LIST
aux_source_directory(. SRC_LIST)
#当前文件下的database文件夹下的cpp->DB_LIST
aux_source_directory(./database DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)

add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})
target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient hiredis curl)




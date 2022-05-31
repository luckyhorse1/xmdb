cmake_minimum_required(VERSION 3.5)

project(xmdb)

include_directories(src/include)

aux_source_directory(src BASE)
aux_source_directory(src/access ACCESS)
aux_source_directory(src/catalog CATALOG)
aux_source_directory(src/storage STORAGE)

add_executable(xmdb main.cpp ${BASE} ${ACCESS} ${CATALOG} ${STORAGE})
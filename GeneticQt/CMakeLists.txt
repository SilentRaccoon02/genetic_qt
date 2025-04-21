cmake_minimum_required(VERSION 3.16)
project(GeneticQt LANGUAGES CXX)


# --- Packages ---
if(WIN32)
    include("C:/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(protolib common.proto)
target_link_libraries(protolib gRPC::grpc++)

target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate(TARGET protolib LANGUAGE cpp)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET protolib LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

# --- Sources ---
set(HEADERS
    geneticqt.h

)

set(SOURCES
    main.cpp
    geneticqt.cpp

)


# --- Target ---
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES}
    rpcclient.h rpcclient.cpp
    rpcclientimpl.h rpcclientimpl.cpp
    rpcserver.h rpcserver.cpp
    rpcserverimpl.h rpcserverimpl.cpp

)


# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets protolib)
target_link_libraries(${PROJECT_NAME} PRIVATE Common Models Views)

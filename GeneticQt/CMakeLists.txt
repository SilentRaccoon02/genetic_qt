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
    rpcclient.h
    rpcclientimpl.h
    rpcserver.h
    rpcserverimpl.h
)

set(SOURCES
    main.cpp
    geneticqt.cpp
    rpcclient.cpp
    rpcclientimpl.cpp
    rpcserver.cpp
    rpcserverimpl.cpp
)


# --- Target ---
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})


# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets protolib)
target_link_libraries(${PROJECT_NAME} PRIVATE Common Models Views)


# --- Install ---
if(WIN32)
    set(GENETIC_QT_INSTALL_DIR "${CMAKE_BINARY_DIR}/install")
    file(GLOB VCPKG_DLLS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
    install(TARGETS ${PROJECT_NAME} DESTINATION ${GENETIC_QT_INSTALL_DIR})
    install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION ${GENETIC_QT_INSTALL_DIR})
    install(FILES ${VCPKG_DLLS} DESTINATION ${GENETIC_QT_INSTALL_DIR})
endif()

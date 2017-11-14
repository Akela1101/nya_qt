cmake_minimum_required(VERSION 3.9)
project(nya_qt)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)

# Easylogging
set(build_static_lib ON cache)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_subdirectory(../easyloggingpp easyloggingpp)

# Temporary disable SSL
add_definitions(-DQT_NO_OPENSSL)


add_library(${PROJECT_NAME}
        src/ApplicationDaemonNya.cpp
        src/ApplicationDaemonNya.hpp
        src/ApplicationGuiNya.cpp
        src/ApplicationGuiNya.hpp
        src/ApplicationNya.cpp
        src/ApplicationNya.hpp
        src/ConfigNya.cpp
        src/ConfigNya.hpp
        src/CryptoNya.cpp
        src/CryptoNya.hpp
        src/DirNya.cpp
        src/DirNya.hpp
        src/NyaQt.hpp
        src/SingletonNya.hpp
        src/StreamNya.cpp
        src/StreamNya.hpp
        src/TimeNya.cpp
        src/TimeNya.hpp
        src/XmlNya.cpp
        src/XmlNya.hpp)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        ../nya/src
        ../easyloggingpp/src)

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        easyloggingpp
        dbghelp)

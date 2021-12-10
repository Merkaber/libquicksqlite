cmake_minimum_required(VERSION 3.20)

set(DEFAULT_BUILD_TYPE "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(quicksqlite VERSION 1.0.0)

add_compile_definitions(LIBRARY_NAME="${CMAKE_PROJECT_NAME}: ")

configure_file("${CMAKE_PROJECT_NAME}.pc.in" "${CMAKE_PROJECT_NAME}.pc" @ONLY)

set(SRC_FILES src/quicksqlite.cpp include/quicksqlite.h src/Tools.cpp src/Tools.h)

find_package(SQLite3 REQUIRED)

add_library(${CMAKE_PROJECT_NAME} ${SRC_FILES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> PRIVATE src)

target_link_libraries(${CMAKE_PROJECT_NAME} sqlite3)

install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT quicksqliteConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME})

install(EXPORT quicksqliteConfig DESTINATION share/quicksqlite/cmake)

install(FILES "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

export(TARGETS ${CMAKE_PROJECT_NAME} FILE quicksqliteConfig.cmake)

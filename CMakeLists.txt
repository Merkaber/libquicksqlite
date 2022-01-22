# similar cmake structure like: https://github.com/pabloariasal/modern-cmake-sample
# by Pablo Arias: https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

cmake_minimum_required(VERSION 3.20)
project(libquicksqlite VERSION 1.0.0 LANGUAGES CXX)

# Find custom modules if needed
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Create target
set(SRC_FILES src/quicksqlite.cpp include/quicksqlite/quicksqlite.h src/Tools.cpp src/Tools.h)
add_library(quicksqlite ${SRC_FILES})

# Add alias so that the library can be used inside the build tree
add_library(QuickSQLite::quicksqlite ALIAS quicksqlite)

# Set target properties
target_include_directories(quicksqlite
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(quicksqlite PRIVATE cxx_std_17)
target_compile_options(quicksqlite PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>
)

# Link targets with our own target
target_link_libraries(quicksqlite
    PUBLIC
        sqlite3
)

# Set install paths for UNIX and WIN32
if(UNIX)
    include(GNUInstallDirs)
else()
    if(WIN32)
        set(CMAKE_INSTALL_LIBDIR "lib")
        set(CMAKE_INSTALL_DATADIR "share")
        set(CMAKE_INSTALL_INCLUDEDIR "include")
        set(CMAKE_INSTALL_BINDIR "bin")
        message(STATUS "Setting installation destination on Windows to: ${CMAKE_INSTALL_PREFIX}")
    else()
        message(FATAL_ERROR "System not UNIX nor WIN32 - not implemented yet")
    endif()
endif()

# Set place where the configs going to be installed
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/QuickSQLite)

install(TARGETS quicksqlite
    EXPORT quicksqlite-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# This is required so that the exported target has the name QuickSQLite and not quicksqlite
set_target_properties(quicksqlite PROPERTIES EXPORT_NAME QuickSQLite)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the targets to a script
install(EXPORT quicksqlite-targets
    FILE
        QuickSQLiteTargets.cmake
    NAMESPACE
        QuickSQLite::
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QuickSQLiteConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/QuickSQLiteConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QuickSQLiteConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, the configversion and the custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QuickSQLiteConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/QuickSQLiteConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Export from the build tree
export(EXPORT quicksqlite-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/QuickSQLiteTargets.cmake
    NAMESPACE QuickSQLite::
)

export(PACKAGE QuickSQLite)

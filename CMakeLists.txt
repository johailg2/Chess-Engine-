cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Include Python headers
include_directories(${Python3_INCLUDE_DIRS})

# Find pybind11
# Set the path to the pybind11 package configuration directory
set(pybind11_DIR "C:/Users/johai/AppData/Local/Programs/Python/Python312/Lib/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)

# Include pybind11 headers
include_directories(${pybind11_INCLUDE_DIRS})

# Source files
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/Engine/src/main.cpp
    ${CMAKE_SOURCE_DIR}/Engine/src/bitboards.cpp
    ${CMAKE_SOURCE_DIR}/Engine/src/chessboard.cpp
    ${CMAKE_SOURCE_DIR}/Engine/src/wrapper.cpp
    ${CMAKE_SOURCE_DIR}/Engine/src/attackTables.cpp
    ${CMAKE_SOURCE_DIR}/Engine/src/eval.cpp
)

# Add executable for the main application
add_executable(chess_engine ${SOURCE_FILES})

# Link the executable with Python libraries
target_link_libraries(chess_engine ${Python3_LIBRARIES})

# Create the Python extension module
#add_library(chessBoardWrap MODULE ${SOURCE_FILES})

# Set properties for the Python extension target
#set_target_properties(chessBoardWrap PROPERTIES PREFIX "")
#set_target_properties(chessBoardWrap PROPERTIES SUFFIX ".pyd")

# Link the Python extension target with the necessary libraries
#target_link_libraries(chessBoardWrap PRIVATE pybind11::module ${Python3_LIBRARIES})

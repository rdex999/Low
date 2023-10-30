cmake_minimum_required(VERSION 3.27.7)
project(Low VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(low source/main/main.cpp)

target_compile_features(low PUBLIC cxx_std_23)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

add_subdirectory(source/parseArgs)
add_subdirectory(source/lexer)
add_subdirectory(source/parse)
add_subdirectory(source/genAsm)

target_link_libraries(low
    parseArgs
    lexer
    parse
    genAsm
)
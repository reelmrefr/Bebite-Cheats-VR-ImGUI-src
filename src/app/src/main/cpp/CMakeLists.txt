cmake_minimum_required(VERSION 3.22.1)

project("BebiteCheats")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DOBBY_GENERATE_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/Dobby EXCLUDE_FROM_ALL)

add_subdirectory(extern/BNM-Android EXCLUDE_FROM_ALL)
target_link_libraries(BNM PUBLIC dobby)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_subdirectory(extern/imgui EXCLUDE_FROM_ALL)

add_library(${CMAKE_PROJECT_NAME} SHARED
        src/native-lib.cpp
        src/czrender.cpp
        include/XRInput.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${BNM_INCLUDE_DIRECTORIES}
    extern/BNM-Android/src/private
    include
    extern/imgui
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        BNM
        imgui
)

cmake_minimum_required(VERSION 3.10)

set(Test0 SDL2_test)
set(Test1 BasicRayTracing)
set(Test2 DirectLight)
set(Test3 HardShadows)

set(main ${Test3})
project(${main})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_executable(${PROJECT_NAME} src/${main}.cpp)

set(SUBDIRS 
    cg_math 
    simple_cg 
    SDL2)

foreach(subdir IN LISTS SUBDIRS)
    add_subdirectory(libs/${subdir})
endforeach(subdir IN LISTS SUBDIRS)

target_link_libraries(${PROJECT_NAME} cg_math)
target_link_libraries(${PROJECT_NAME} simple_cg)
target_link_libraries(${PROJECT_NAME} SDL2 SDL2main)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/lib/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND $<TARGET_FILE:${PROJECT_NAME}>#运行程序
        COMMENT "Running ${PROJECT_NAME}..."
)
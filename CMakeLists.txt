cmake_minimum_required(VERSION 3.7)

project(russell)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_custom_target(
    russell-content
    COMMAND make -C ${PROJECT_SOURCE_DIR} OUTDIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/content content
)

file(GLOB FILES "src/*.cpp")
file(GLOB GAMEFILES "src/game/*.cpp")
file(GLOB DEBUGFILES "src/debug/*.cpp")
add_executable(russell 
    ${FILES} ${GAMEFILES} ${DEBUGFILES})
set_property(TARGET russell PROPERTY CXX_STANDARD 17)
set_property(TARGET russell PROPERTY CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(russell PROPERTIES
        COMPILE_FLAGS "${COMPILE_FLAGS} -Wall -Wextra")
endif()

add_subdirectory("vendor/glad")
add_subdirectory("vendor/json")
add_subdirectory("vendor/stb")
add_subdirectory("deps/glfw")
add_subdirectory("deps/portaudio")
add_subdirectory("deps/freetype-2.9.1")
# include_directories("vendor/glfw")
# include_directories("vendor/glad")
# include_directories("vendor/json")
# include_directories("vendor/KHR")
# include_directories("vendor/stb")
include_directories("vendor")

include_directories("deps/freetype-2.9.1/include")
include_directories("deps/glfw/include")
include_directories("deps/portaudio/include")
include_directories("deps/libaudiodecoder/include")

add_dependencies(russell
    russell-content)

target_link_libraries(russell
    #statically linked
    glad glfw stb_image jsoncpp
    portaudio freetype
    # dynamically linked
    sndfile)

    
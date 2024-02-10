cmake_minimum_required(VERSION 3.25)
project(math)

set(CMAKE_CXX_STANDARD 20)

set(JAVA_HOME, /Users/derek/zulu-jdk11/zulu-11.jdk/Contents/Home)

file(GLOB_RECURSE src
        "*.hpp"
        "*.h"
        "*.cpp"
        "*.c"
        )

file (GLOB x11lib
        "/opt/X11/lib/*.dylib")

list(FILTER src EXCLUDE REGEX "cmake")

link_directories(/Users/derek/zulu-jdk11/zulu-11.jdk/Contents/Home/lib/server)
link_directories(/usr/local/lib/oatpp-1.3.0)
link_directories(/opt/X11/lib)

add_library(math SHARED ${src} gui/hook.cpp gui/hook.h java/jasperfinder.cpp java/jasperfinder.h java/betterkeystrokes.cpp java/betterkeystrokes.h)

target_include_directories(math PRIVATE /Users/derek/zulu-jdk11/zulu-11.jdk/Contents/Home/include/darwin)
target_include_directories(math PRIVATE /Users/derek/zulu-jdk11/zulu-11.jdk/Contents/Home/include)
target_include_directories(math PRIVATE /usr/local/include/oatpp-1.3.0/oatpp)
target_include_directories(math PRIVATE /opt/X11/include)
target_include_directories(math PRIVATE .)

target_link_libraries(math PRIVATE jvm)
target_link_libraries(math PRIVATE oatpp)
target_link_libraries(math PRIVATE ${x11lib})
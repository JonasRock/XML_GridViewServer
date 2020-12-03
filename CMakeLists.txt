cmake_minimum_required(VERSION 3.0.0)
project(XML_GridViewServer VERSION 0.1.0)

# Set your boost locations
set(BOOST_ROOT)
set(BOOST_LIBRARYDIR C:/boost/boost_1_74_0/stage/lib)
set(BOOST_INCLUDEDIR C:/boost/boost_1_74_0/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

add_executable(XML_GridViewServer
  src/main.cpp
  src/ioHandler.cpp
  src/messageParser.cpp
  src/xmlService.cpp
  src/xmlParser.cpp
  src/config.cpp
  include/extern/pugixml.cpp
)

target_include_directories(XML_GridViewServer PUBLIC include)
target_include_directories(XML_GridViewServer PUBLIC include/extern)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(include/extern)

target_link_libraries(XML_GridViewServer PUBLIC ws2_32 -static)
target_compile_options(XML_GridViewServer PRIVATE -Werror -Wall -Wextra)

set(Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.74.0 REQUIRED COMPONENTS filesystem iostreams)
target_link_libraries(XML_GridViewServer PUBLIC Boost::filesystem Boost::iostreams)
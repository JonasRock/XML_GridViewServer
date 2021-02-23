cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(XML_GridViewServer VERSION 0.1.0)

add_executable(XML_GridViewServer
  src/main.cpp
  src/ioHandler.cpp
  src/messageParser.cpp
  src/xmlService.cpp
  src/xmlParser.cpp
  src/config.cpp
  include/extern/pugixml.cpp
)

if(MSVC)
    target_compile_options(XML_GridViewServer PUBLIC /std:c++17 /nr:false /o2)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(XML_GridViewServer PUBLIC -static -Wall -Wextra)
endif()
    
if(WIN32)
    target_link_libraries(XML_GridViewServer PUBLIC ws2_32 wsock32 winpthread)
else()
    target_link_libraries(XML_GridViewServer PUBLIC pthread)
endif()

target_include_directories(XML_GridViewServer PUBLIC include)
target_include_directories(XML_GridViewServer PUBLIC include/extern)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(include/extern)

set(Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.74.0 REQUIRED COMPONENTS filesystem iostreams)
target_link_libraries(XML_GridViewServer PUBLIC Boost::filesystem Boost::iostreams)
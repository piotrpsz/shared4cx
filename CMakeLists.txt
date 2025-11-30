cmake_minimum_required(VERSION 4.0)
project(shared4cx VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS iostreams)
message("Boost include dir: ${Boost_INCLUDE_DIRS}")
message("Boost libraries: ${Boost_LIBRARIES}")
message("Boost found: ${Boost_FOUND}")

find_package(date REQUIRED)

add_library(shared4cx STATIC
        types.h
        shared.cpp shared.h
        gzip.cpp gzip.h
        datime.h
        error.h
)

target_link_libraries(shared4cx PRIVATE
        ${Boost_LIBRARIES}
        date::date
        date::date-tz
)

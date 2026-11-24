cmake_minimum_required(VERSION 3.0...4.0)

project(template-cd LANGUAGES CXX)

add_executable(ip IP "ip.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 23)

add_executable(sp SP "sp.cpp")
set_target_properties(sp PROPERTIES CXX_STANDARD 23)

add_iso_stub(template-cd ip sp)

cmake_minimum_required(VERSION 3.0...4.0)

project(template-cd LANGUAGES CXX)

add_ip_executable(ip "ip.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 23)

add_sp_executable(sp "sp.cpp")
set_target_properties(sp PROPERTIES CXX_STANDARD 23)

add_iso_stub(template-cd ip sp)

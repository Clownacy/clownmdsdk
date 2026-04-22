cmake_minimum_required(VERSION 3.0...4.0)

project(template-cartridge LANGUAGES CXX)

add_executable(template-cartridge "main.cpp")

set_target_properties(template-cartridge PROPERTIES CXX_STANDARD 23)

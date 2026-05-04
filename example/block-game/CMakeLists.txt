cmake_minimum_required(VERSION 3.0...4.0)

project(block-game LANGUAGES CXX)

add_cartridge_executable(block-game "main.cpp")

set_target_properties(block-game PROPERTIES CXX_STANDARD 23)

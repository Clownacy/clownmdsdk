cmake_minimum_required(VERSION 3.0...4.0)

project(tofu LANGUAGES CXX)

add_executable(tofu
	"objects/base.cpp"
	"objects/base.h"
	"objects/bullet.cpp"
	"objects/bullet.h"
	"objects/player.cpp"
	"objects/player.h"
	"controller.cpp"
	"controller.h"
	"coordinate.h"
	"doubly-linked-list.cpp"
	"doubly-linked-list.h"
	"level.cpp"
	"level.h"
	"list-common.h"
	"main.cpp"
	"objects.cpp"
	"objects.h"
	"pool.cpp"
	"pool.h"
	"singly-linked-list.cpp"
	"singly-linked-list.h"
	"sprite.cpp"
	"sprite.h"
)

set_target_properties(tofu PROPERTIES CXX_STANDARD 26)

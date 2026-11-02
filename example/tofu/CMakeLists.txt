cmake_minimum_required(VERSION 3.0...4.0)

project(tofu LANGUAGES CXX)

add_cartridge_executable(tofu
	"objects/base.cpp"
	"objects/base.h"
	"objects/bullet.cpp"
	"objects/bullet.h"
	"objects/player.cpp"
	"objects/player.h"
	"container/doubly-linked-list.cpp"
	"container/doubly-linked-list.h"
	"container/list-common.h"
	"container/pool.h"
	"container/singly-linked-list.cpp"
	"container/singly-linked-list.h"
	"controller.cpp"
	"controller.h"
	"coordinate.h"
	"level.cpp"
	"level.h"
	"main.cpp"
	"object-manager.cpp"
	"object-manager.h"
	"sprite.cpp"
	"sprite.h"
)

set_target_properties(tofu PROPERTIES CXX_STANDARD 26)

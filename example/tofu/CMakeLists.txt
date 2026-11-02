cmake_minimum_required(VERSION 3.0...4.0)

project(tofu LANGUAGES CXX)

add_cartridge_executable(tofu
	"objects/base.cpp"
	"objects/base.h"
	"objects/bullet.cpp"
	"objects/bullet.h"
	"objects/player.cpp"
	"objects/player.h"
	"container/doubly-linked-list.h"
	"container/list-common.h"
	"container/pool.h"
	"container/singly-linked-list.cpp"
	"container/singly-linked-list.h"
	"controller-manager.cpp"
	"controller-manager.h"
	"coordinate.h"
	"level-manager.cpp"
	"level-manager.h"
	"main.cpp"
	"mega-drive.h"
	"object-manager.cpp"
	"object-manager.h"
	"sprite-manager.cpp"
	"sprite-manager.h"
	"vram-addresses.h"
)

set_target_properties(tofu PROPERTIES CXX_STANDARD 26)

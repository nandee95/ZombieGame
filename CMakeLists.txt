#set minimal cmake version
cmake_minimum_required (VERSION 3.1)
#set up version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

#set C++ standard
set (CMAKE_CXX_STANDARD 14 ) #filesystem
set (CMAKE_CXX_STANDARD_REQUIRED ON)

#https://blog.kitware.com/cmake-building-with-all-your-cores/
set (CMAKE_CXX_FLAGS "/MP /EHsc")

#set up project
get_filename_component(PROJECT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY) #getting parent directory
project(game CXX)

#build number increment
#set(BUILDNO_FILE "${PROJECT_DIR}/cmake_tools/buildno.txt")
#if( EXISTS ${BUILDNO_FILE})
	#file(READ BUILDNO_FILE BUILD_NUMBER)
#else()
	#set(BUILD_NUMBER "0")
#endif()
#message(${BUILDNO_FILE})
#math(EXPR BUILD_NUMBER "${BUILD_NUMBER}+1")
#file(WRITE BUILDNO_FILE "${BUILD_NUMBER}")
#message("${BUILD_NUMBER}")

#GUI options
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(DEBUG_ENABLED TRUE CACHE BOOL "Include debug information inside binaries" FORCE)

if(DEBUG_ENABLED)
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

set(SHARED_SOURCE
	System/Config.hpp
	System/Build.hpp
	System/State.cpp
	System/State.hpp
	System/Value.hpp
	System/Value.cpp
	System/Window.hpp
	System/Window.cpp
	System/CmdLine.hpp
	System/CmdLine.cpp
	System/CfgFile.hpp
	System/CfgFile.cpp
	System/Error.hpp
	System/Error.cpp
	System/Math.hpp
	System/Math.cpp
)

add_executable(client ${SHARED_SOURCE}
	System/ResourceManager.cpp
	System/ResourceManager.hpp
	main_client.cpp
	Client/Manager.hpp
	Client/Manager.cpp
	Client/SinglePlayerGame.hpp
	Client/SinglePlayerGame.cpp
	Client/Client.hpp
	Client/Client.cpp
	Client/Chat.hpp
	Client/Chat.cpp
	Client/Level.hpp
	Client/Level.cpp
	Client/Input.hpp
	Client/Input.cpp
	Client/Player.hpp
	Client/Player.cpp
	Client/Wind.cpp
	Client/Wind.hpp
	Menu/Menu.hpp
	Menu/Menu.cpp
	Physics/Physics.hpp
	Physics/Physics.cpp
	Physics/RectBody.hpp
	Physics/RectBody.cpp
	Physics/CircleBody.hpp
	Physics/CircleBody.cpp
	Physics/BodyBase.hpp
	Physics/BodyBase.cpp
	Overlays/Debug.hpp
	Overlays/Debug.cpp
	Entities/Tree.hpp
	Entities/Tree.cpp
	Entities/Particle.hpp
	Entities/Particle.cpp
	Entities/ParticleEmitter.hpp
	Entities/ParticleEmitter.cpp
	Entities/Entity.hpp
	Entities/Entity.cpp
	System/Transitions.hpp
	System/Transitions.cpp
	Entities/Cursor.cpp
	Entities/Cursor.hpp
	Entities/Grass.cpp
	Entities/Grass.hpp
	Inventory/Inventory.cpp
	Inventory/Inventory.hpp
	Inventory/ItemBase.cpp
	Inventory/ItemBase.hpp
	Inventory/BoosterEnergyDrink.cpp
	Inventory/BoosterEnergyDrink.hpp
	Inventory/ToolAxe.cpp
	Inventory/ToolAxe.hpp
	Menu/Settings.hpp
	Menu/Settings.cpp
	spine-sfml.h
	spine-sfml.cpp
	Server/Chunk.cpp
	Server/Chunk.hpp
)
set_target_properties(client PROPERTIES OUTPUT_NAME "client")
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/build_windows/")

add_executable(server ${SHARED_SOURCE}
	Server/Server.cpp
	Server/Server.hpp
	Server/Protocol.cpp
	Server/Protocol.hpp
	Server/Entity.cpp
	Server/Entity.hpp
	Server/Player.cpp
	Server/Player.hpp
	Server/Chat.cpp
	Server/Chat.hpp
	Server/Chunk.cpp
	Server/Chunk.hpp
	Server/Chat.hpp
	main_server.cpp
)

set_target_properties(server PROPERTIES OUTPUT_NAME "server")
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/build_windows/")
#add_executable(leveleditor  ${SHARED_SOURCE}
#	main_leveleditor.cpp
#	Client/Level.hpp
#	Client/Level.cpp
#	LevelEditor/LevelEditor.cpp
#	LevelEditor/LevelEditor.hpp
#	LevelEditor/ToolBase.cpp
#	LevelEditor/ToolBase.hpp
#	LevelEditor/ToolTile.cpp
#	LevelEditor/ToolTile.hpp
#	Entities/Cursor.cpp
#	Entities/Cursor.hpp
#	LevelEditor/ObjectSprite.cpp
#	LevelEditor/ObjectSprite.hpp
#	LevelEditor/ColorPicker.cpp
#	LevelEditor/ColorPicker.hpp
#	LevelEditor/Layer.cpp
#	LevelEditor/Layer.hpp
#)
#set_target_properties(leveleditor PROPERTIES OUTPUT_NAME "leveleditor")
#set_target_properties(leveleditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/build_windows/")


#add_custom_command(
#	TARGET client
#	POST_BUILD
#	COMMAND "${CMAKE_COMMAND}" -E copy "${PROJECT_DIR}/cmake_files/${CMAKE_BUILD_TYPE}/client.exe" "${PROJECT_DIR}/build_windows"
#)

#Visual Studio settings
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT client)

#add libraries
set(CMAKE_MODULE_PATH "${PROJECT_DIR}/cmake_modules")

#SFML
set(SFML_ROOT ${CMAKE_MODULE_PATH}/SFML-2.4.2/lib)
set(SFML_INCLUDE_DIR ${CMAKE_MODULE_PATH}/SFML-2.4.2/include)
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.4.2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(client ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
	target_link_libraries(server ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
#	target_link_libraries(leveleditor ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

#TGUI (was used only by the level editor)
#set(TGUI_ROOT ${CMAKE_MODULE_PATH}/TGUI-0.7/lib)
#set(TGUI_INCLUDE_DIR ${CMAKE_MODULE_PATH}/TGUI-0.7/include)
#find_package(TGUI 0.7.3 REQUIRED)
#if(TGUI_FOUND AND SFML_FOUND)
	#include_directories(${TGUI_INCLUDE_DIR})
#	target_link_libraries(leveleditor ${TGUI_LIBRARY})
	#message( ${TGUI_LIBRARY})
#endif()

#spine-c
include_directories("${CMAKE_MODULE_PATH}/spine/spine-c/spine-c/include")
target_link_libraries(client "${CMAKE_MODULE_PATH}/spine/spine-c/Debug/spine-c.lib")

#libnoise
include_directories("${CMAKE_MODULE_PATH}/libnoise-master/src/")
target_link_libraries(client "${CMAKE_MODULE_PATH}/libnoise-master/src/Debug/noise.lib")
target_link_libraries(server "${CMAKE_MODULE_PATH}/libnoise-master/src/Debug/noise.lib")

#for Debug info
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}   )
target_link_libraries(client ${OPENGL_LIBRARIES}  )

#pass version as macros
add_definitions(-DVERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${VERSION_PATCH})
add_definitions(-DBUILD_NUMBER=${BUILD_NUMBER})



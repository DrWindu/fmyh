##
##  Copyright (C) 2016-2017 the authors (see AUTHORS)
##
##  This file is part of ld35.
##
##  lair is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  lair is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with lair.  If not, see <http://www.gnu.org/licenses/>.
##

#find_package(Eigen3 REQUIRED)
#find_package(SDL2 REQUIRED)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

include_directories(
	"${EIGEN3_INCLUDE_DIR}"
	"${SDL2_INCLUDE_DIR}"
)

set(SOURCES
	main.cpp
	settings.cpp
	game.cpp
	components.cpp
	gui.cpp
	level.cpp
	commands.cpp
	main_state.cpp
	splash_state.cpp
	dialog.cpp
	logic_language.tab.cpp
	logic_language.yy.cpp
)

if(APPLE)
	list(APPEND SOURCES fmemopen.cpp)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}
	lair
	winpthread
)

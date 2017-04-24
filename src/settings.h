/*
 *  Copyright (C) 2015, 2016 The authors (see AUTHORS).
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef SETTINGS_H
#define SETTINGS_H


#include <lair/core/lair.h>
#include <lair/core/property.h>


using namespace lair;


class Settings {
public:
	Settings();

	const PropertyList& properties() const;

public:
	float   guiTransitionDuration;
	Vector2 dialogVisiblePos;
	Vector2 dialogHiddenPos;
	Vector2 characterVisiblePos;
	Vector2 characterHiddenPos;
	float   playerAnimSpeed;
	float   fadeDuration;
};


#endif

/*
 *  Copyright (C) 2015, 2016 the authors (see AUTHORS).
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


#include "settings.h"


Settings::Settings()
	: guiTransitionDuration(0.2)
    , dialogVisiblePos     (0, 0)
    , dialogHiddenPos      (0, -1000)
    , characterVisiblePos  (0, 0)
    , characterHiddenPos   (-1000, 0)
    , playerAnimSpeed      (0.3)
    , fadeDuration         (0.5)
{
}


const PropertyList& Settings::properties() const {
	static PropertyList properties;
	if(!properties.nProperties()) {
		properties.addProperty("gui_transition_duration", &Settings::guiTransitionDuration);
		properties.addProperty("dialog_visible_pos",      &Settings::dialogVisiblePos);
		properties.addProperty("dialog_hidden_pos",       &Settings::dialogHiddenPos);
		properties.addProperty("character_visible_pos",   &Settings::characterVisiblePos);
		properties.addProperty("character_hidden_pos",    &Settings::characterHiddenPos);
		properties.addProperty("player_anim_speed",       &Settings::playerAnimSpeed);
		properties.addProperty("fade_duration",           &Settings::fadeDuration);
	}
	return properties;
}

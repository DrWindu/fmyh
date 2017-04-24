/*
 *  Copyright (C) 2016 the authors (see AUTHORS)
 *
 *  This file is part of ld36.
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


#include <sstream>

#include <lair/sys_sdl2/audio_module.h>

#include "game.h"
#include "main_state.h"
#include "splash_state.h"
#include "level.h"

#include "components.h"


int echoCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	std::ostringstream out;
	out << argv[0];
	for(int i = 1; i < argc; ++i)
		out << " " << argv[i];
	dbgLogger.info(out.str());

	return 0;
}


int talkToCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc < 2) {
		dbgLogger.warning("messageCommand: wrong number of argument.");
		return -2;
	}

	state->log().info("Talk to ", argv[1], " (", self.name(), ")");
	state->startDialog("test");

	return 0;
}


int messageCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc < 2) {
		dbgLogger.warning("messageCommand: wrong number of argument.");
		return -2;
	}

//	state->popupMessage(argv[1]);

//	if(argc > 2)
//		state->setPostCommand(argc - 2, argv + 2);
//	else
//		state->setPostCommand("continue");

	return 0;
}


int nextLevelCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 2 && argc != 3) {
		dbgLogger.warning("nextLevelCommand: wrong number of argument.");
		return -2;
	}

//	if(argc == 2)
//		state->startLevel(argv[1]);
//	else
//		state->startLevel(argv[1], argv[2]);

	return 0;
}


int teleportCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 2) {
		dbgLogger.warning("teleportCommand: wrong number of argument.");
		return -2;
	}

	EntityRef target = state->_level->entity(argv[1]);
	if(!target.isValid()) {
		dbgLogger.warning("teleportCommand: target \"", target.name(), "\" not found.");
		return -2;
	}

	float depth = state->_player.transform()(2, 3);
	state->_player.place((Vector3() << target.translation2(), depth).finished());
	state->playSound("tp.wav");

	TriggerComponent* tc = state->_triggers.get(target);
	if(tc) {
		tc->inside = true;
		tc->prevInside = true;
	}

	return 0;
}


int playSoundCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 2) {
		dbgLogger.warning("playSoundCommand: wrong number of argument.");
		return -2;
	}

	state->playSound(argv[1]);

	return 0;
}


int continueCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 1) {
		dbgLogger.warning("playSoundCommand: wrong number of argument.");
		return -2;
	}

//	state->setState(STATE_PLAY);

	return 0;
}


int fadeInCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc < 1) {
		dbgLogger.warning("fadeInCommand: wrong number of argument.");
		return -2;
	}

//	state->setState(STATE_FADE_IN);

//	if(argc > 1) {
//		state->setPostCommand(argc - 1, argv + 1);
//	}

	return 0;
}


int fadeOutCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc < 1) {
		dbgLogger.warning("fadeOutCommand: wrong number of argument.");
		return -2;
	}

//	state->setState(STATE_FADE_OUT);

//	if(argc > 1) {
//		state->setPostCommand(argc - 1, argv + 1);
//	}

	return 0;
}


int disableCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 1) {
		dbgLogger.warning(argv[0], ": wrong number of argument.");
		return -2;
	}

	if(!self.isValid()) {
		dbgLogger.warning(argv[0], ": self is not set.");
		return -2;
	}

	self.setEnabled(false);

	return 0;
}


int creditsCommand(MainState* state, EntityRef self, int argc, const char** argv) {
	if(argc != 1) {
		dbgLogger.warning(argv[0], ": wrong number of argument.");
		return -2;
	}

	state->game()->splashState()->setup(nullptr, "credits.png");
	state->game()->setNextState(state->game()->splashState());
	state->quit();

	return 0;
}

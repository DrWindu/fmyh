/*
 *  Copyright (C) 2015, 2016 the authors (see AUTHORS)
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


#ifndef MAIN_STATE_H
#define MAIN_STATE_H


#include <lair/core/signal.h>

#include <lair/utils/game_state.h>
#include <lair/utils/interp_loop.h>
#include <lair/utils/input.h>
#include <lair/utils/tile_map.h>

#include <lair/render_gl2/orthographic_camera.h>
#include <lair/render_gl2/render_pass.h>

#include <lair/ec/entity.h>
#include <lair/ec/entity_manager.h>
#include <lair/ec/sprite_component.h>
#include <lair/ec/collision_component.h>
#include <lair/ec/bitmap_text_component.h>
#include <lair/ec/tile_layer_component.h>

#include "settings.h"
#include "gui.h"
#include "level.h"


#define FRAMERATE 60
#define TICKRATE  60


using namespace lair;


class Game;


class MainState : public GameState {
public:
	MainState(Game* game);
	virtual ~MainState();

	virtual void initialize();
	virtual void shutdown();

	virtual void run();
	virtual void quit();

	Game* game();

	void startGame();
	void updateTick();
	void updateFrame();

	void resizeEvent();

	EntityRef getEntity(const String& name, const EntityRef& ancestor = EntityRef());

	bool loadEntities(const Path& path, EntityRef parent = EntityRef(),
	                  const Path& cd = Path());

public:
	// More or less system stuff

	RenderPass                 _mainPass;

	EntityManager              _entities;
	SpriteRenderer             _spriteRenderer;
	SpriteComponentManager     _sprites;
	CollisionComponentManager  _collisions;
	BitmapTextComponentManager _texts;
	TileLayerComponentManager  _tileLayers;
//	AnimationComponentManager  _anims;
	InputManager               _inputs;

	SlotTracker _slotTracker;

	Settings _settings;

	OrthographicCamera _camera;

	bool        _initialized;
	bool        _running;
	InterpLoop  _loop;
	int64       _prevFrameTime;
	int64       _fpsTime;
	unsigned    _fpsCount;

	Input*      _quitInput;
	Input*      _okInput;
	Input*      _upInput;
	Input*      _downInput;
	Input*      _leftInput;
	Input*      _rightInput;

	Gui         _gui;

	LevelSP     _campLevel;
	TileMapAspectSP _tileMap;

	EntityRef   _models;
	EntityRef   _scene;
	EntityRef   _guiLayer;

	EntityRef   _player;
	Direction   _playerDir;

	int _debugCounter;
};


#endif

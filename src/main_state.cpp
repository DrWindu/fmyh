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


#include <functional>

#include <lair/core/json.h>

#include "game.h"

#include "main_state.h"


#define ONE_SEC (1000000000)


MainState::MainState(Game* game)
	: GameState(game),

      _mainPass(renderer()),

      _entities(log(), _game->serializer()),
      _spriteRenderer(renderer()),
      _sprites(assets(), loader(), &_mainPass, &_spriteRenderer),
      _texts(loader(), &_mainPass, &_spriteRenderer),
      _tileLayers(loader(), &_mainPass, &_spriteRenderer),

      _inputs(sys(), &log()),

      _camera(),

      _initialized(false),
      _running(false),
      _loop(sys()),
      _prevFrameTime(-1),
      _fpsTime(0),
      _fpsCount(0),

      _quitInput(nullptr),
      _okInput(nullptr),
      _upInput(nullptr),
      _downInput(nullptr),
      _leftInput(nullptr),
      _rightInput(nullptr),

      _gui(this),

      _debugCounter(0)
{

	_entities.registerComponentManager(&_sprites);
	_entities.registerComponentManager(&_texts);
	_entities.registerComponentManager(&_tileLayers);
}


MainState::~MainState() {
}


void MainState::initialize() {
	Path settingsLogicPath = "settings.ldl";
	Path settingsRealPath = game()->dataPath() / settingsLogicPath;
	Path::IStream in(settingsRealPath.native().c_str());
	if(in.good()) {
		log().info("Read settings \"", settingsRealPath, "\"...");
		ErrorList errors;
		LdlParser parser(&in, settingsLogicPath.utf8String(), &errors, LdlParser::CTX_MAP);
		game()->serializer().read(parser, _settings);
		errors.log(log());
	}
	else {
		log().error("Failed to read settings (\"", settingsLogicPath, "\").");
	}

	_loop.reset();
	_loop.setTickDuration( ONE_SEC /  TICKRATE);
	_loop.setFrameDuration(ONE_SEC /  FRAMERATE);
	_loop.setMaxFrameDuration(_loop.frameDuration() * 3);
	_loop.setFrameMargin(     _loop.frameDuration() / 2);

	window()->onResize.connect(std::bind(&MainState::resizeEvent, this))
	        .track(_slotTracker);

	_quitInput = _inputs.addInput("quit");
	_inputs.mapScanCode(_quitInput, SDL_SCANCODE_ESCAPE);

	_okInput = _inputs.addInput("ok");
	_inputs.mapScanCode(_okInput, SDL_SCANCODE_SPACE);
	_inputs.mapScanCode(_okInput, SDL_SCANCODE_RETURN);
	_inputs.mapScanCode(_okInput, SDL_SCANCODE_RETURN2);

	_upInput = _inputs.addInput("up");
	_inputs.mapScanCode(_upInput, SDL_SCANCODE_UP);

	_downInput = _inputs.addInput("down");
	_inputs.mapScanCode(_downInput, SDL_SCANCODE_DOWN);

	_leftInput = _inputs.addInput("left");
	_inputs.mapScanCode(_leftInput, SDL_SCANCODE_LEFT);

	_rightInput = _inputs.addInput("right");
	_inputs.mapScanCode(_rightInput, SDL_SCANCODE_RIGHT);

	// Load entities
	loadEntities("entities.ldl", _entities.root());

	_models   = getEntity("__models__");
	_scene    = getEntity("scene");
	_guiLayer = getEntity("gui");

	_campLevel = std::make_shared<Level>(this, "camp.json");
	_campLevel->preload();

	loader()->load<SoundLoader>("sound.ogg");
	//loader()->load<MusicLoader>("music.ogg");

	loader()->waitAll();

	// Set to true to debug OpenGL calls
	renderer()->context()->setLogCalls(false);

	_gui.initialize();

	_initialized = true;
}


void MainState::shutdown() {
	_gui.shutdown();

	_slotTracker.disconnectAll();

	_initialized = false;
}


void MainState::run() {
	lairAssert(_initialized);

	log().log("Starting main state...");
	_running = true;
	_loop.start();
	_fpsTime  = sys()->getTimeNs();
	_fpsCount = 0;

	startGame();

	do {
		switch(_loop.nextEvent()) {
		case InterpLoop::Tick:
			_entities.setPrevWorldTransforms();
			updateTick();
			_entities.updateWorldTransforms();
			break;
		case InterpLoop::Frame:
			updateFrame();
			break;
		}
	} while (_running);
	_loop.stop();
}


void MainState::quit() {
	_running = false;
}


Game* MainState::game() {
	return static_cast<Game*>(_game);
}


void MainState::startGame() {
	_player = _entities.cloneEntity(getEntity("player_model"), _scene, "player");

	_campLevel->initialize();
	_campLevel->start("spawn");

	//audio()->playMusic(assets()->getAsset("music.ogg"));
	audio()->playSound(assets()->getAsset("sound.ogg"), 2);
}


void MainState::updateTick() {
	loader()->finalizePending();

	_inputs.sync();

//	if(_state == STATE_PLAY) {
		// Player movement
		Vector2 offset(0, 0);
		if(_upInput->isPressed()) {
			offset(1) += 1;
			_playerDir  = UP;
		}
		if(_leftInput->isPressed()) {
			offset(0) -= 1;
			_playerDir  = LEFT;
		}
		if(_downInput->isPressed()) {
			offset(1) -= 1;
			_playerDir  = DOWN;
		}
		if(_rightInput->isPressed()) {
			offset(0) += 1;
			_playerDir  = RIGHT;
		}

		Vector2 lastPlayerPos = _player.translation2();
		float playerSpeed = 10 * float(TILE_SIZE) / float(TICKRATE);
		if(!offset.isApprox(Vector2::Zero())) {
			_player.translation2() += offset.normalized() * playerSpeed;
		}

//		_level->computeCollisions();

//		// Level logic
//		HitEventQueue hitQueue;
//		_collisions.findCollisions(hitQueue);
////		for(const HitEvent& hit: hitQueue)
////			dbgLogger.debug("hit: ", hit.entities[0].name(), ", ", hit.entities[1].name());

//		EntityRef useEntity;
//		if(_useInput->justPressed()) {
//			std::deque<EntityRef> useQueue;
//			Vector2 pos = _player.worldTransform().translation().head<2>();
//			_collisions.hitTest(useQueue, pos, HIT_USE_FLAG);

//			if(useQueue.empty()) {
//				float o = 28;
//				Vector2 offset((_playerDir == LEFT)? -o: (_playerDir == RIGHT)? o: 0,
//				               (_playerDir == DOWN)? -o: (_playerDir == UP   )? o: 0);
//				_collisions.hitTest(useQueue, pos + offset, HIT_USE_FLAG);
//			}

//			if(!useQueue.empty()) {
//				useEntity = useQueue.front();
//				dbgLogger.debug("use: ", useEntity.name());
//			}
//		}

//		updateTriggers(hitQueue, useEntity);

//		// Bump player
//		for(HitEvent& hit: hitQueue) {
//			CollisionComponent* cc = _collisions.get(hit.entities[1]);
//			if(cc && hit.entities[0] == _player && cc->hitMask() & HIT_SOLID_FLAG) {
//				CollisionComponent* pcc = _collisions.get(_player);
//				updatePenetration(pcc, pcc->worldAlignedBox(), cc->worldAlignedBox());
//			}
//		}

//		CollisionComponent* pColl = _collisions.get(_player);
//		Vector2  bump(0, 0);
//		bump(0) += std::max(0.01f + pColl->penetration(LEFT),  0.f);
//		bump(0) -= std::max(0.01f + pColl->penetration(RIGHT), 0.f);
//		bump(1) += std::max(0.01f + pColl->penetration(DOWN),  0.f);
//		bump(1) -= std::max(0.01f + pColl->penetration(UP),    0.f);
//		_player.translate(bump);

//		if(!_player.translation2().isApprox(lastPlayerPos)) {
//			float prevAnim = _playerAnim;
//			_playerAnim += _playerAnimSpeed / float(TICKRATE);
//			orientPlayer(_playerDir, 1 + int(_playerAnim) % 2);

//			if(int(prevAnim) % 2 != int(_playerAnim) % 2)
//				playSound("footstep.wav");
//		}
//		else {
//			_playerAnim = 0;
//			orientPlayer(_playerDir);
//		}

//		_overlay.setEnabled(false);
//	}

	if(_quitInput->justPressed()) {
		quit();
	}

	if(_okInput->justPressed()) {
		switch(_debugCounter % 4) {
		case 0:
			_gui.showDialog();
			break;
		case 1:
			_gui.showCharacter();
			break;
		case 2:
			_gui.hideDialog();
			break;
		case 3:
			_gui.hideCharacter();
			break;
		}

		++_debugCounter;
	}
}


void MainState::updateFrame() {
	double elapsedSec = (_prevFrameTime < 0)? 0: double(_loop.frameTime() - _prevFrameTime) / double(ONE_SEC);

	Vector2 playerPos = _player.interpTransform(_loop.frameInterp()).translation().head<2>();
	Vector2 viewSize(window()->width(), window()->height());
	Box3 viewBox((Vector3() << playerPos - viewSize / 2, 0).finished(),
	             (Vector3() << playerPos + viewSize / 2, 1).finished());
	_camera.setViewBox(viewBox);

	Transform guiTrans = _guiLayer.transform();
	guiTrans.matrix().col(3).head<2>() = viewBox.min().head<2>();
	guiTrans.matrix()(0, 0) = (float(window()->width()) / 1920.0f);
	guiTrans.matrix()(1, 1) = guiTrans.matrix()(0, 0);
	_guiLayer.transform() = guiTrans;
	_guiLayer.updateWorldTransform();
	_guiLayer.setPrevWorldTransform();

	_gui.updateAnimation(elapsedSec);

	// Rendering
	Context* glc = renderer()->context();

	_texts.createTextures();
	_tileLayers.createTextures();
	renderer()->uploadPendingTextures();

	glc->clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	_mainPass.clear();
	_spriteRenderer.clear();

	_sprites.render(_entities.root(), _loop.frameInterp(), _camera);
	_texts.render(_entities.root(), _loop.frameInterp(), _camera);
	_tileLayers.render(_entities.root(), _loop.frameInterp(), _camera);

	_mainPass.render();

	window()->swapBuffers();
	glc->setLogCalls(false);

	uint64 now = sys()->getTimeNs();
	++_fpsCount;
	if(_fpsCount == 60) {
		log().info("Fps: ", _fpsCount * float(ONE_SEC) / (now - _fpsTime));
		_fpsTime  = now;
		_fpsCount = 0;
	}

	_prevFrameTime = _loop.frameTime();
}


void MainState::resizeEvent() {
	// TODO: Support aspect ratio other that 16:9 without stretching.
	Box3 viewBox(Vector3(0, 0, 0),
	             Vector3(1920, 1080, 1));
	_camera.setViewBox(viewBox);
}


EntityRef MainState::getEntity(const String& name, const EntityRef& ancestor) {
	EntityRef entity = _entities.findByName(name, ancestor);
	if(!entity.isValid()) {
		log().error("Entity \"", name, "\" not found.");
	}
	return entity;
}


bool MainState::loadEntities(const Path& path, EntityRef parent, const Path& cd) {
	Path localPath = makeAbsolute(cd, path);
	log().info("Load entity \"", localPath, "\"");

	Path realPath = game()->dataPath() / localPath;
	Path::IStream in(realPath.native().c_str());
	if(!in.good()) {
		log().error("Unable to read \"", localPath, "\".");
		return false;
	}
	ErrorList errors;
	LdlParser parser(&in, localPath.utf8String(), &errors, LdlParser::CTX_MAP);

	bool success = _entities.loadEntitiesFromLdl(parser, parent);

	errors.log(log());

	return success;
}

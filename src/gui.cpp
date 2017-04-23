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

#include "main_state.h"

#include "gui.h"


Gui::Gui(MainState* mainState)
	: _initialized(false)
    , _mainState  (mainState)
{
}

void Gui::initialize() {
	_initialized = true;

	_dialog    = _mainState->getEntity("dialog");
	_text      = _mainState->getEntity("dialog_text");
	_character = _mainState->getEntity("gui_character");

	if(_dialog.isValid())
		_dialog.place(_mainState->_settings.dialogHiddenPos);
	if(_character.isValid())
		_character.place(_mainState->_settings.characterHiddenPos);

	_dialogAnim.reset();
	_characterAnim.reset();
}

void Gui::shutdown() {
	_dialog.release();
	_text.release();
	_character.release();

	_initialized = false;
}

bool Gui::isDialogVisible() const {
	return _dialog.translation2() == _mainState->_settings.dialogVisiblePos;
}

void Gui::showDialog() {
	if(_dialog.isValid() && !isDialogVisible()) {
		_dialogAnim.reset(
		            _dialog.translation2(),
		            _mainState->_settings.dialogVisiblePos,
		            _mainState->_settings.guiTransitionDuration,
		            [this](const Vector2& pos) { _dialog.place(pos); });
	}
}

void Gui::hideDialog() {
	if(_dialog.isValid() && isDialogVisible()) {
		_dialogAnim.reset(
		            _dialog.translation2(),
		            _mainState->_settings.dialogHiddenPos,
		            _mainState->_settings.guiTransitionDuration,
		            [this](const Vector2& pos) { _dialog.place(pos); });
	}
}

void Gui::setText(const String& message) {
	_mainState->_texts.get(_text)->setText(message);
}

void Gui::setCharacterSprite(const Path& path) {
	_mainState->_sprites.get(_character)->setTexture(path);
}

bool Gui::isCharacterVisible() const {
	return _character.translation2() == _mainState->_settings.characterVisiblePos;
}

void Gui::showCharacter() {
	if(_character.isValid() && !isCharacterVisible()) {
		_characterAnim.reset(
		            _character.translation2(),
		            _mainState->_settings.characterVisiblePos,
		            _mainState->_settings.guiTransitionDuration,
		            [this](const Vector2& pos) { _character.place(pos); });
	}
}

void Gui::hideCharacter() {
	if(_character.isValid() && isCharacterVisible()) {
		_characterAnim.reset(
		            _character.translation2(),
		            _mainState->_settings.characterHiddenPos,
		            _mainState->_settings.guiTransitionDuration,
		            [this](const Vector2& pos) { _character.place(pos); });
	}
}

bool Gui::isAnimated() {
	return !_dialogAnim.isDone() || !_characterAnim.isDone();
}

void Gui::updateAnimation(double elapsedSec) {
	const Settings& settings = _mainState->_settings;

	_dialogAnim.update(elapsedSec);
	_characterAnim.update(elapsedSec);

	_dialog.updateWorldTransform();
	_text.updateWorldTransform();
	_character.updateWorldTransform();

	_dialog.setPrevWorldTransform();
	_text.setPrevWorldTransform();
	_character.setPrevWorldTransform();
}

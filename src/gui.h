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


#ifndef GUI_H
#define GUI_H


#include <lair/core/lair.h>
#include <lair/core/property.h>

#include <lair/ec/entity.h>


using namespace lair;


class MainState;

class Animation {
public:
	Animation();
	virtual ~Animation();

	virtual bool isDone() const = 0;
	virtual void update(double elapsedSec) = 0;
};

template<typename T>
class ValueAnimation {
public:
	typedef std::function<void(const T&)> Setter;

public:
	ValueAnimation()
	    : _from()
	    , _to()
	    , _time(0)
	    , _duration(0)
	    , _setter()
	{
	}

	ValueAnimation(const T& from, const T& to, double duration, const Setter& setter)
	    : _from(from)
	    , _to(to)
	    , _time(0)
	    , _duration(duration)
	    , _setter(setter)
	{
	}

	~ValueAnimation() {
	}

	void reset() {
		_time     = 0;
		_duration = 0;
		_setter   = Setter();
	}

	void reset(const T& from, const T& to, double duration, const Setter& setter) {
		_from     = from;
		_to       = to;
		_time     = 0;
		_duration = duration;
		_setter   = setter;
	}

	virtual bool isDone() const {
		return _time >= _duration;
	}

	virtual void update(double elapsedSec) {
		if(isDone() || !_setter)
			return;

		_time += elapsedSec;

		if(isDone())
			_setter(_to);
		else
			_setter(lerp(_time / _duration, _from, _to));
	}

private:
	T      _from;
	T      _to;
	double _time;
	double _duration;
	Setter _setter;
};


class Gui {
public:
	Gui(MainState* mainState);

	void initialize();
	void shutdown();

	bool isDialogVisible() const;
	void showDialog();
	void hideDialog();
	void setText(const String& message);

	void setCharacterSprite(const Path& path);
	bool isCharacterVisible() const;
	void showCharacter();
	void hideCharacter();

	bool isAnimated();
	void updateAnimation(double elapsedSec);

public:
	bool _initialized;
	MainState* _mainState;

	EntityRef _dialog;
	EntityRef _text;
	EntityRef _character;

	ValueAnimation<Vector2> _dialogAnim;
	ValueAnimation<Vector2> _characterAnim;
};


#endif

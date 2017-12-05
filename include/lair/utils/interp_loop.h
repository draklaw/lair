/*
 *  Copyright (C) 2015 Simon Boyé
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


#ifndef _LAIR_UTILS_INTERP_LOOP_H
#define _LAIR_UTILS_INTERP_LOOP_H


#include <lair/core/lair.h>


namespace lair
{


class SysModule;

class InterpLoop {
public:
	enum EventType {
		Tick,
		Frame
	};

public:
	InterpLoop(SysModule* sys);
	~InterpLoop();

	void reset();

	void start();
	void stop();

	EventType nextEvent();

	unsigned tickCount() const;
	uint64 tickTime() const;

	unsigned frameCount() const;
	uint64  frameTime() const;
	float frameInterp() const;

	uint64 tickDuration() const;
	void setTickDuration(uint64 ns);

	uint64 frameDuration() const;
	void setFrameDuration(uint64 ns);

	uint64 maxFrameDuration() const;
	void setMaxFrameDuration(uint64 ns);

	uint64 frameMargin() const;
	void setFrameMargin(uint64 ns);

protected:
	SysModule* _sys;

	uint64   _tickDuration;
	unsigned _tickCount;
	uint64   _prevTickGameTime;
	uint64   _tickGameTime;
	uint64   _prevTickRealTime;
	uint64   _tickRealTime;
	uint64   _lastFrameRealTime;
	unsigned _frameSkipped;

	uint64   _frameDuration;	// Ideal duration of 1 frame
	uint64   _maxFrameDuration;	// After this delay, we slow down the game
	uint64   _frameMargin;		// Try to keep the rythme if we are latter than this
	unsigned _frameCount;
	uint64   _frameGameTime;
	uint64   _frameRealTime;
	float    _frameInterp;
};


}


#endif

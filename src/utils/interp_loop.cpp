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


#include <lair/core/lair.h>
#include <lair/core/log.h>

#include <lair/sys_sdl2/sys_module.h>

#include "lair/utils/interp_loop.h"


namespace lair
{


InterpLoop::InterpLoop(SysModule* sys)
    : _sys(sys),

      _tickDuration    (1000000000/60),
      _tickCount       (0),
      _prevTickGameTime(0),
      _tickGameTime    (0),
      _prevTickRealTime(0),
      _tickRealTime    (0),

      _frameDuration   (1000000000/60),
      _maxFrameDuration(1000000000/20),
      _frameMargin     (_frameDuration / 2),
      _frameCount      (0),
      _frameGameTime   (0),
      _frameRealTime   (0),
      _frameInterp     (0) {
}


InterpLoop::~InterpLoop() {
}


void InterpLoop::reset() {
	_prevTickGameTime = 0;
	_tickGameTime     = 0;
	_prevTickRealTime = 0;
	_tickRealTime     = 0;

	_frameCount       = 0;
	_frameGameTime    = 0;
	_frameRealTime    = 0;
	_frameSkipped     = 0;
}


void InterpLoop::start() {
	uint64 now = _sys->getTimeNs();

	_tickCount        = 0;
	_prevTickRealTime = 0;
	_tickRealTime     = now;
	_frameRealTime    = 0;
	_frameSkipped     = 0;
}


void InterpLoop::stop() {
}


InterpLoop::EventType InterpLoop::nextEvent() {
	uint64 now = _sys->getTimeNs();

	uint64 nextFrame = _frameRealTime + _frameDuration;
	uint64 nextTick  = _tickRealTime + 1;
	uint64 nextEvent = _frameDuration? std::min(nextTick, nextFrame): nextTick;

	while(now < nextEvent) {
		if(!frameDuration())
			return EventType::Frame;

		_sys->dispatchPendingSystemEvents();
		_sys->waitNs(nextEvent - now);
		now = _sys->getTimeNs();
	}
	_sys->dispatchPendingSystemEvents();

	// FIXME: Works only if getTimeNs is near 0 when program start.
//	uint64 maxFrameTime = _frameRealTime + _maxFrameDuration;
//	bool late = _tickRealTime > maxFrameTime;
//	if(late) {
//		// We are late !
//		int64 offset = _tickRealTime - maxFrameTime;
//		if(offset > 0) {
//			_prevTickRealTime += offset;
//			_tickRealTime     += offset;
//		}
//	}

	if(_frameSkipped < 3 && _tickRealTime < now) {
		if(_frameRealTime < now)
			++_frameSkipped;
		++_tickCount;
		_prevTickGameTime  = _tickGameTime;
		_tickGameTime     += _tickDuration;
		_prevTickRealTime  = _tickRealTime;
		_tickRealTime     += _tickDuration;
		return EventType::Tick;
	} else {
		++_frameCount;
		_frameInterp = clamp(double(          now - _prevTickRealTime)
		                   / double(_tickRealTime - _prevTickRealTime), 0., 1.);
		_frameGameTime = lerp(_frameInterp, _prevTickGameTime, _tickGameTime);
		_frameRealTime = std::max(_frameRealTime + _frameDuration, now - _frameMargin);
		_frameSkipped = 0;
		return EventType::Frame;
	}
}


unsigned InterpLoop::tickCount() const {
	return _tickCount;
}


uint64 InterpLoop::tickTime() const {
	return _tickGameTime;
}


unsigned InterpLoop::frameCount() const {
	return _frameCount;
}


uint64  InterpLoop::frameTime() const {
	return _frameGameTime;
}


float InterpLoop::frameInterp() const {
	return _frameInterp;
}


uint64 InterpLoop::tickDuration() const {
	return _tickDuration;
}


void InterpLoop::setTickDuration(uint64 ns) {
	_tickDuration = ns;
}


uint64 InterpLoop::frameDuration() const {
	return _frameDuration;
}


void InterpLoop::setFrameDuration(uint64 ns) {
	_frameDuration = ns;
}


uint64 InterpLoop::maxFrameDuration() const {
	return _maxFrameDuration;
}


void InterpLoop::setMaxFrameDuration(uint64 ns) {
	_maxFrameDuration = ns;
}


uint64 InterpLoop::frameMargin() const {
	return _frameMargin;
}


void InterpLoop::setFrameMargin(uint64 ns) {
	_frameMargin = ns;
}


}

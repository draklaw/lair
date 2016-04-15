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

#include "lair/core/signal.h"


namespace lair
{


ConnectionRef::ConnectionRef()
    : _c(nullptr),
      _ownConnection(false) {
}

ConnectionRef::ConnectionRef(_Connection* connection)
    : _c(connection),
      _ownConnection(false) {
	if(_c) {
		++_c->refCount;
	}
}

ConnectionRef::ConnectionRef(const ConnectionRef& cr)
    : _c(cr._c),
      _ownConnection(false) {
	if(_c) {
		++_c->refCount;
	}
}

ConnectionRef::ConnectionRef(ConnectionRef&& cr)
    : _c(cr._c),
      _ownConnection(false) {
	cr._c = nullptr;
}

ConnectionRef::~ConnectionRef() {
	if(isValid()) {
		if(_ownConnection && isConnected()) {
			disconnect();
		}
		release();
	}
}

ConnectionRef& ConnectionRef::operator=(ConnectionRef rhs) {
	swap(*this, rhs);
	return *this;
}

bool ConnectionRef::operator==(const ConnectionRef& rhs) const {
	return _c == rhs._c;
}

bool ConnectionRef::operator!=(const ConnectionRef& rhs) const {
	return !(*this == rhs);
}

void ConnectionRef::release() {
	if(isValid()) {
		assert(_c->refCount > 0);
		--_c->refCount;
		if(_c->refCount == 0) {
			// Can not be connected here (cause Signal own a ref)
			delete _c;
		}
		_c = nullptr;
	}
}

bool ConnectionRef::isValid() const {
	return _c;
}

bool ConnectionRef::isConnected() const {
	return _c && _c->signal;
}

bool ConnectionRef::isOwning() const {
	return _ownConnection;
}

void ConnectionRef::own() {
	_ownConnection = true;
}

void ConnectionRef::disown() {
	_ownConnection = false;
}

ConnectionRef& ConnectionRef::track(SlotTracker& tracker) {
	lairAssert(!_c->tracker);
	_c->tracker = &tracker;
	_c->trackerNext = _c->tracker->_firstConn;
	if(_c->tracker->_firstConn.isValid()) {
		_c->tracker->_firstConn._get()->trackerPrev = *this;
	}
	_c->tracker->_firstConn = *this;
	return *this;
}

void ConnectionRef::disconnect() {
	if(isConnected()) {
		_Connection* conn = _c;
		if(conn->tracker) {
			_untrack(conn);
		}
		conn->signal->_disconnect(conn);
	}
}

_Connection* ConnectionRef::_get() {
	lairAssert(_c);
	return _c;
}

void ConnectionRef::_untrack(_Connection* conn) {
	lairAssert(conn->tracker);
	if(conn->trackerPrev.isValid()) {
		conn->trackerPrev._get()->trackerNext = conn->trackerNext;
	} else {
		conn->tracker->_firstConn = conn->trackerNext;
	}
	conn->tracker = nullptr;
	conn->trackerPrev.release();
	conn->trackerNext.release();
}

void swap(ConnectionRef& cr0, ConnectionRef& cr1) {
	std::swap(cr0._c, cr1._c);
	std::swap(cr0._ownConnection, cr1._ownConnection);
}


//---------------------------------------------------------------------------//


SlotTracker::SlotTracker()
    : _firstConn() {
}


SlotTracker::~SlotTracker() {
	disconnectAll();
}


void SlotTracker::disconnectAll() {
	while(_firstConn.isValid()) {
		_firstConn.disconnect();
	}
}


}

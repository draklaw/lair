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


#ifndef _LAIR_CORE_SIGNAL_H
#define _LAIR_CORE_SIGNAL_H


#include <utility>
#include <functional>

#include <lair/core/lair.h>


namespace lair
{


class _Connection;
class ConnectionRef;
class SlotTracker;
class SignalBase;


class ConnectionRef {
public:
	ConnectionRef();
	explicit ConnectionRef(_Connection* connection);
	ConnectionRef(const ConnectionRef& cr);
	ConnectionRef(ConnectionRef&& cr);
	~ConnectionRef();

	ConnectionRef& operator=(ConnectionRef rhs);

	bool operator==(const ConnectionRef& rhs) const;
	bool operator!=(const ConnectionRef& rhs) const;

	inline void release();

	bool isValid() const;
	bool isConnected() const;
	bool isOwning() const;

	void own();
	void disown();
	ConnectionRef& track(SlotTracker& tracker);

	void disconnect();

	_Connection* _get();
	void _untrack(_Connection* conn);

	inline friend void swap(ConnectionRef& cr0, ConnectionRef& cr1);

private:
	_Connection* _c;
	bool         _ownConnection;
};


struct _Connection {
	inline _Connection()
	    : function(),
	      refCount(0),
	      signal(),
	      signalPrev(),
	      signalNext(),
	      tracker(),
	      trackerPrev(),
	      trackerNext() {
	}

	Byte           function[sizeof(std::function<void()>)];
	size_t         refCount;

	SignalBase*    signal;
	ConnectionRef  signalPrev;
	ConnectionRef  signalNext;

	SlotTracker*   tracker;
	ConnectionRef  trackerPrev;
	ConnectionRef  trackerNext;
};


class SlotTracker {
public:
	SlotTracker()
	    : _firstConn() {
	}
	SlotTracker(const SlotTracker&) = delete;
	SlotTracker(SlotTracker&&)      = delete;
	~SlotTracker() {
		disconnectAll();
	}

	SlotTracker& operator=(const SlotTracker&) = delete;
	SlotTracker& operator=(SlotTracker&&)      = delete;

	void disconnectAll() {
		while(_firstConn.isValid()) {
			_firstConn.disconnect();
		}
	}

public:
	ConnectionRef _firstConn;
};


class SignalBase {
public:
	virtual void _disconnect(_Connection* conn) = 0;
};


template < typename... Args >
class Signal : public SignalBase {
public:
	typedef std::function<void(Args...)> Slot;

public:
	Signal()
	    : _firstConnection(),
	      _lastConnection() {
	}

	Signal(const Signal&) = delete;
	Signal(Signal&&)      = delete;
	~Signal() {
		disconnectAll();
	}

	Signal& operator=(const Signal&) = delete;
	Signal& operator=(Signal&&)      = delete;

	template < typename T >
	ConnectionRef connect(const T& slot) {
		_Connection* conn = new _Connection;
		new (&conn->function) Slot(slot);
		ConnectionRef ref(conn);

		conn->signal = this;
		conn->signalPrev = _lastConnection;
		if(_lastConnection.isValid()) {
			assert(_firstConnection.isValid());
			_lastConnection._get()->signalNext = ref;
		} else {
			assert(!_firstConnection.isValid());
			_firstConnection = ref;
		}
		_lastConnection = ref;
		return ref;
	}

	void disconnectAll() {
		while(_firstConnection.isValid()) {
			_firstConnection.disconnect();
		}
	}

	void operator()(Args... args) {
		ConnectionRef* conn = &_firstConnection;
		while(conn->isValid()) {
			_Connection* c = conn->_get();
			lairAssert(c->signal == this);
			(*reinterpret_cast<Slot*>(&c->function))(
			            std::forward<Args>(args)...);
			conn = &c->signalNext;
		}
	}

	virtual void _disconnect(_Connection* conn) {
		// Keep a ref so the object is not destroyed to soon !
		ConnectionRef cr = ConnectionRef(conn);
		if(conn->signalPrev.isValid()) {
			lairAssert(_firstConnection._get() != conn);
			conn->signalPrev._get()->signalNext = conn->signalNext;
		} else {
			lairAssert(_firstConnection._get() == conn);
			_firstConnection = conn->signalNext;
		}
		if(conn->signalNext.isValid()) {
			lairAssert(_lastConnection._get() != conn);
			conn->signalNext._get()->signalPrev = conn->signalPrev;
		} else {
			lairAssert(_lastConnection._get() == conn);
			_lastConnection = conn->signalPrev;
		}
		(*reinterpret_cast<Slot*>(&conn->function)).~Slot();
		conn->signal = nullptr;
		conn->signalPrev.release();
		conn->signalNext.release();
	}

protected:
	ConnectionRef _firstConnection;
	ConnectionRef _lastConnection;
};


//---------------------------------------------------------------------------//


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


}


#endif

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


#include <gtest/gtest.h>

#include <lair/core/signal.h>


using namespace lair;

TEST(SignalTest, ConnectionRef) {
	ConnectionRef ref;

	ASSERT_FALSE(ref.isValid());
	ASSERT_FALSE(ref.isConnected());
}

TEST(SignalTest, SignalBase) {
	unsigned counter = 0;
	auto f = [&counter]() { ++counter; };

	Signal<> sig;

	sig();
	ASSERT_EQ(0, counter);

	ConnectionRef c0 = sig.connect(f);
	ASSERT_EQ(0, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());

	sig();
	ASSERT_EQ(1, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());

	ConnectionRef c1 = sig.connect(f);
	ASSERT_EQ(1, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());
	ASSERT_TRUE(c1.isValid());
	ASSERT_TRUE(c1.isConnected());

	sig();
	ASSERT_EQ(3, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());
	ASSERT_TRUE(c1.isValid());
	ASSERT_TRUE(c1.isConnected());

	c1.disconnect();
	ASSERT_EQ(3, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());
	ASSERT_TRUE(c1.isValid());
	ASSERT_FALSE(c1.isConnected());

	sig();
	ASSERT_EQ(4, counter);
	ASSERT_TRUE(c0.isValid());
	ASSERT_TRUE(c0.isConnected());
	ASSERT_TRUE(c1.isValid());
	ASSERT_FALSE(c1.isConnected());
}

TEST(SignalTest, BlockConnect) {
	unsigned counter = 0;
	auto f = [&counter]() { ++counter; };

	Signal<> sig;

	{
		sig.connect(f);
	}

	sig();
	ASSERT_EQ(1, counter);
}

TEST(SignalTest, DisconnectAll) {
	unsigned counter = 0;
	auto f = [&counter]() { ++counter; };

	Signal<> sig;

	sig.connect(f);
	sig.connect(f);
	sig.connect(f);

	sig();
	ASSERT_EQ(3, counter);

	sig.disconnectAll();

	sig();
	ASSERT_EQ(3, counter);
}

TEST(SignalTest, ConnectionOwn) {
	unsigned counter = 0;
	auto f = [&counter]() { ++counter; };

	Signal<> sig;

	ConnectionRef c0;
	{
		ConnectionRef tmp = sig.connect(f);
		tmp.own();
		c0 = tmp;
		ASSERT_TRUE(tmp.isOwning());
		ASSERT_FALSE(c0.isOwning());

		sig();
		ASSERT_EQ(1, counter);
	}
	ASSERT_TRUE(c0.isValid());
	ASSERT_FALSE(c0.isConnected());

	sig();
	ASSERT_EQ(1, counter);
}

TEST(SignalTest, SlotTracker) {
	unsigned counter = 0;
	auto f = [&counter]() { ++counter; };

	Signal<> sig;

	ConnectionRef c0;
	{
		SlotTracker tracker;
		c0 = sig.connect(f).track(tracker);

		sig();
		ASSERT_EQ(1, counter);
	}
	ASSERT_TRUE(c0.isValid());
	ASSERT_FALSE(c0.isConnected());

	sig();
	ASSERT_EQ(1, counter);
}

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

#include <lair/ec/entity_manager.h>


using namespace lair;

TEST(EntityManagerTest, Constructor) {
	EntityManager em(nullptr, 4);

	ASSERT_EQ(1, em.nEntities());
	ASSERT_EQ(0, em.nZombieEntities());
	ASSERT_EQ(4, em.entityCapacity());

	ASSERT_TRUE(em.root().isValid());
	ASSERT_EQ(EntityRef(), em.root().parent());
	ASSERT_EQ(EntityRef(), em.root().nextSibling());
	ASSERT_EQ(EntityRef(), em.root().firstChild());
	ASSERT_STREQ("__root__", em.root().name());
}

TEST(EntityManagerTest, EntityTree) {
	EntityManager em(nullptr, 4);

	EntityRef root = em.root();
	EntityRef a = em.createEntity(root, "a");
	EntityRef b = em.createEntity(root, "b");
	EntityRef c = em.createEntity(a, "c");

	ASSERT_EQ(4, em.nEntities());
	ASSERT_EQ(0, em.nZombieEntities());
	ASSERT_EQ(4, em.entityCapacity());

	ASSERT_TRUE(root.isValid());
	ASSERT_TRUE(   a.isValid());
	ASSERT_TRUE(   b.isValid());
	ASSERT_TRUE(   c.isValid());

	ASSERT_EQ(EntityRef(), root.parent());
	ASSERT_EQ(root,           a.parent());
	ASSERT_EQ(root,           b.parent());
	ASSERT_EQ(a,              c.parent());

	ASSERT_EQ(EntityRef(), root.nextSibling());
	ASSERT_EQ(EntityRef(),    a.nextSibling());
	ASSERT_EQ(a,              b.nextSibling());
	ASSERT_EQ(EntityRef(),    c.nextSibling());

	ASSERT_EQ(b,           root.firstChild());
	ASSERT_EQ(c,              a.firstChild());
	ASSERT_EQ(EntityRef(),    b.firstChild());
	ASSERT_EQ(EntityRef(),    c.firstChild());

	ASSERT_STREQ("__root__", root.name());
	ASSERT_STREQ("a",           a.name());
	ASSERT_STREQ("b",           b.name());
	ASSERT_STREQ("c",           c.name());

	em.destroyEntity(b);

	ASSERT_EQ(3, em.nEntities());
	ASSERT_EQ(1, em.nZombieEntities());
	ASSERT_EQ(4, em.entityCapacity());

	ASSERT_TRUE (root.isValid());
	ASSERT_TRUE (   a.isValid());
	ASSERT_FALSE(   b.isValid());
	ASSERT_TRUE (   c.isValid());

	ASSERT_EQ(EntityRef(), root.parent());
	ASSERT_EQ(root,           a.parent());
	ASSERT_EQ(nullptr,        b._get()->parent);
	ASSERT_EQ(a,              c.parent());

	ASSERT_EQ(EntityRef(), root.nextSibling());
	ASSERT_EQ(EntityRef(),    a.nextSibling());
	ASSERT_EQ(nullptr,        b._get()->nextSibling);
	ASSERT_EQ(EntityRef(),    c.nextSibling());

	ASSERT_EQ(a,           root.firstChild());
	ASSERT_EQ(c,              a.firstChild());
	ASSERT_EQ(nullptr,        b._get()->firstChild);
	ASSERT_EQ(EntityRef(),    c.firstChild());

	ASSERT_STREQ("__root__", root.name());
	ASSERT_STREQ("a",           a.name());
	ASSERT_EQ(nullptr,          b._get()->name);
	ASSERT_STREQ("c",           c.name());

	{
		EntityRef tmp = b;
		ASSERT_EQ(1, em.nZombieEntities());
		b.release();
		ASSERT_EQ(1, em.nZombieEntities());
	}
	ASSERT_EQ(0, em.nZombieEntities());

	EntityRef d = em.createEntity(a, "d");

	ASSERT_EQ(4, em.nEntities());
	ASSERT_EQ(0, em.nZombieEntities());
	ASSERT_EQ(4, em.entityCapacity());

	ASSERT_NE(b, d);
	ASSERT_TRUE(d.isValid());
	ASSERT_EQ(a, d.parent());
	ASSERT_EQ(c, d.nextSibling());
	ASSERT_EQ(EntityRef(), d.firstChild());
	ASSERT_STREQ("d", d.name());

	EntityRef e = em.createEntity(c, "e");

	ASSERT_EQ(5, em.nEntities());
	ASSERT_EQ(8, em.entityCapacity());

	em.destroyEntity(a);

	ASSERT_EQ(1, em.nEntities());
	ASSERT_EQ(4, em.nZombieEntities());
	ASSERT_EQ(8, em.entityCapacity());

	ASSERT_TRUE (root.isValid());
	ASSERT_FALSE(   a.isValid());
	ASSERT_FALSE(   c.isValid());
	ASSERT_FALSE(   d.isValid());
	ASSERT_FALSE(   e.isValid());

	a = em.createEntity(root, "a");
	b = em.createEntity(root, "b");
	c = em.createEntity(root, "c");
	d = em.createEntity(root, "d");
	e = em.createEntity(root, "e");
	em.createEntity(root, "f");
	em.createEntity(root, "g");

	ASSERT_EQ(8, em.nEntities());
	ASSERT_EQ(0, em.nZombieEntities());
	ASSERT_EQ(8, em.entityCapacity());
}

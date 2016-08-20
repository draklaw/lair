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


#define BLOCK_SIZE 4

using namespace lair;


class EntityManagerTest : public ::testing::Test {
public:
	EntityManager* em;
	EntityRef root;
	EntityRef a;
	EntityRef b;
	EntityRef c;
	EntityRef d;
	EntityRef e;
	EntityRef f;

	EntityManagerTest()
		: em(nullptr) {
	}

	virtual void SetUp() {
		em = new EntityManager(noopLogger, BLOCK_SIZE);
	}

	virtual void TearDown() {
		root.release();
		a.release();
		b.release();
		c.release();
		d.release();
		e.release();
		f.release();
		delete em;
	}

	void buildTree() {
		root = em->root();
		a = em->createEntity(root, "a");
		b = em->createEntity(a,    "b");
		c = em->createEntity(root, "c");
		f = em->createEntity(a,    "f", 1);
		d = em->createEntity(a,    "d", 1);
		e = em->createEntity(d,    "e", 0);
	}
};

TEST_F(EntityManagerTest, Constructor) {
	ASSERT_EQ(1, em->nEntities());
	ASSERT_EQ(0, em->nZombieEntities());
	ASSERT_EQ(4, em->entityCapacity());

	ASSERT_TRUE(em->root().isValid());
	ASSERT_EQ(EntityRef(), em->root().parent());
	ASSERT_EQ(EntityRef(), em->root().nextSibling());
	ASSERT_EQ(EntityRef(), em->root().firstChild());
	ASSERT_STREQ("__root__", em->root().name());
}

TEST_F(EntityManagerTest, CreateEntity) {
	buildTree();

	EntityRef z;

	ASSERT_EQ(7, em->nEntities());
	ASSERT_EQ(0, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	ASSERT_TRUE (root.isValid());
	ASSERT_TRUE (   a.isValid());
	ASSERT_TRUE (   b.isValid());
	ASSERT_TRUE (   c.isValid());
	ASSERT_TRUE (   d.isValid());
	ASSERT_TRUE (   e.isValid());
	ASSERT_TRUE (   f.isValid());
	ASSERT_FALSE(   z.isValid());

	ASSERT_EQ(EntityRef(), root.parent());
	ASSERT_EQ(root,           a.parent());
	ASSERT_EQ(a,              b.parent());
	ASSERT_EQ(root,           c.parent());
	ASSERT_EQ(a,              d.parent());
	ASSERT_EQ(d,              e.parent());
	ASSERT_EQ(a,              f.parent());
	ASSERT_THROW(z.parent(), AssertionFailedError);

	ASSERT_EQ(EntityRef(), root.nextSibling());
	ASSERT_EQ(c,              a.nextSibling());
	ASSERT_EQ(d,              b.nextSibling());
	ASSERT_EQ(EntityRef(),    c.nextSibling());
	ASSERT_EQ(f,              d.nextSibling());
	ASSERT_EQ(EntityRef(),    e.nextSibling());
	ASSERT_EQ(EntityRef(),    f.nextSibling());
	ASSERT_THROW(z.nextSibling(), AssertionFailedError);

	ASSERT_EQ(a,           root.firstChild());
	ASSERT_EQ(b,              a.firstChild());
	ASSERT_EQ(EntityRef(),    b.firstChild());
	ASSERT_EQ(EntityRef(),    c.firstChild());
	ASSERT_EQ(e,              d.firstChild());
	ASSERT_EQ(EntityRef(),    e.firstChild());
	ASSERT_EQ(EntityRef(),    f.firstChild());
	ASSERT_THROW(z.firstChild(), AssertionFailedError);

	ASSERT_EQ(c,           root.lastChild());
	ASSERT_EQ(f,              a.lastChild());
	ASSERT_EQ(EntityRef(),    b.lastChild());
	ASSERT_EQ(EntityRef(),    c.lastChild());
	ASSERT_EQ(e,              d.lastChild());
	ASSERT_EQ(EntityRef(),    e.lastChild());
	ASSERT_EQ(EntityRef(),    f.lastChild());
	ASSERT_THROW(z.lastChild(), AssertionFailedError);

	ASSERT_STREQ("__root__", root.name());
	ASSERT_STREQ("a",           a.name());
	ASSERT_STREQ("b",           b.name());
	ASSERT_STREQ("c",           c.name());
	ASSERT_STREQ("d",           d.name());
	ASSERT_STREQ("e",           e.name());
	ASSERT_STREQ("f",           f.name());
}

TEST_F(EntityManagerTest, DestroyEntity) {
	buildTree();

	em->destroyEntity(d);

	ASSERT_EQ(5, em->nEntities());
	ASSERT_EQ(2, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	ASSERT_TRUE (root.isValid());
	ASSERT_TRUE (   a.isValid());
	ASSERT_TRUE (   b.isValid());
	ASSERT_TRUE (   c.isValid());
	ASSERT_FALSE(   d.isValid());
	ASSERT_FALSE(   e.isValid());
	ASSERT_TRUE (   f.isValid());

	ASSERT_EQ(EntityRef(), root.parent());
	ASSERT_EQ(root,           a.parent());
	ASSERT_EQ(a,              b.parent());
	ASSERT_EQ(root,           c.parent());
	ASSERT_EQ(nullptr,        d._get()->parent);
	ASSERT_EQ(nullptr,        e._get()->parent);
	ASSERT_EQ(a,              f.parent());
	ASSERT_THROW(d.parent(), AssertionFailedError);
	ASSERT_THROW(e.parent(), AssertionFailedError);

	ASSERT_EQ(EntityRef(), root.nextSibling());
	ASSERT_EQ(c,              a.nextSibling());
	ASSERT_EQ(f,              b.nextSibling());
	ASSERT_EQ(EntityRef(),    c.nextSibling());
	ASSERT_EQ(nullptr,        d._get()->nextSibling);
	ASSERT_EQ(nullptr,        e._get()->nextSibling);
	ASSERT_EQ(EntityRef(),    f.nextSibling());
	ASSERT_THROW(d.nextSibling(), AssertionFailedError);
	ASSERT_THROW(e.nextSibling(), AssertionFailedError);

	ASSERT_EQ(a,           root.firstChild());
	ASSERT_EQ(b,              a.firstChild());
	ASSERT_EQ(EntityRef(),    b.firstChild());
	ASSERT_EQ(EntityRef(),    c.firstChild());
	ASSERT_EQ(nullptr,        d._get()->firstChild);
	ASSERT_EQ(nullptr,        e._get()->firstChild);
	ASSERT_EQ(EntityRef(),    f.firstChild());
	ASSERT_THROW(d.firstChild(), AssertionFailedError);
	ASSERT_THROW(e.firstChild(), AssertionFailedError);

	ASSERT_EQ(c,           root.lastChild());
	ASSERT_EQ(f,              a.lastChild());
	ASSERT_EQ(EntityRef(),    b.lastChild());
	ASSERT_EQ(EntityRef(),    c.lastChild());
	ASSERT_EQ(nullptr,        d._get()->lastChild);
	ASSERT_EQ(nullptr,        e._get()->lastChild);
	ASSERT_EQ(EntityRef(),    f.lastChild());
	ASSERT_THROW(d.lastChild(), AssertionFailedError);
	ASSERT_THROW(e.lastChild(), AssertionFailedError);

	ASSERT_STREQ("__root__", root.name());
	ASSERT_STREQ("a",           a.name());
	ASSERT_STREQ("b",           b.name());
	ASSERT_STREQ("c",           c.name());
	ASSERT_EQ(nullptr,          d._get()->name);
	ASSERT_EQ(nullptr,          e._get()->name);
	ASSERT_STREQ("f",           f.name());
}

TEST_F(EntityManagerTest, ReleaseEntity) {
	buildTree();

	d.release();
	ASSERT_EQ(7, em->nEntities());
	ASSERT_EQ(0, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	em->destroyEntity(a);
	ASSERT_EQ(2, em->nEntities());
	ASSERT_EQ(4, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	a.release();
	ASSERT_EQ(2, em->nEntities());
	ASSERT_EQ(3, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	c.destroy();
	ASSERT_EQ(1, em->nEntities());
	ASSERT_EQ(3, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());
}

TEST_F(EntityManagerTest, RecycleEntity) {
	for(int i = 0; i < 8; ++i) {
		buildTree();

		a.destroy();
		b.destroy();
		c.destroy();
		d.destroy();
		e.destroy();
		f.destroy();
		ASSERT_EQ(1, em->nEntities());
		ASSERT_EQ(0, em->nZombieEntities());
		ASSERT_EQ(8, em->entityCapacity());
	}
}

TEST_F(EntityManagerTest, MoveEntity) {
	buildTree();

	em->moveEntity(d, root);

	ASSERT_EQ(7, em->nEntities());
	ASSERT_EQ(0, em->nZombieEntities());
	ASSERT_EQ(8, em->entityCapacity());

	ASSERT_TRUE (root.isValid());
	ASSERT_TRUE (   a.isValid());
	ASSERT_TRUE (   b.isValid());
	ASSERT_TRUE (   c.isValid());
	ASSERT_TRUE (   d.isValid());
	ASSERT_TRUE (   e.isValid());
	ASSERT_TRUE (   f.isValid());

	ASSERT_EQ(EntityRef(), root.parent());
	ASSERT_EQ(root,           a.parent());
	ASSERT_EQ(a,              b.parent());
	ASSERT_EQ(root,           c.parent());
	ASSERT_EQ(root,           d.parent());
	ASSERT_EQ(d,              e.parent());
	ASSERT_EQ(a,              f.parent());

	ASSERT_EQ(EntityRef(), root.nextSibling());
	ASSERT_EQ(c,              a.nextSibling());
	ASSERT_EQ(f,              b.nextSibling());
	ASSERT_EQ(d,              c.nextSibling());
	ASSERT_EQ(EntityRef(),    d.nextSibling());
	ASSERT_EQ(EntityRef(),    e.nextSibling());
	ASSERT_EQ(EntityRef(),    f.nextSibling());

	ASSERT_EQ(a,           root.firstChild());
	ASSERT_EQ(b,              a.firstChild());
	ASSERT_EQ(EntityRef(),    b.firstChild());
	ASSERT_EQ(EntityRef(),    c.firstChild());
	ASSERT_EQ(e,              d.firstChild());
	ASSERT_EQ(EntityRef(),    e.firstChild());
	ASSERT_EQ(EntityRef(),    f.firstChild());

	ASSERT_EQ(d,           root.lastChild());
	ASSERT_EQ(f,              a.lastChild());
	ASSERT_EQ(EntityRef(),    b.lastChild());
	ASSERT_EQ(EntityRef(),    c.lastChild());
	ASSERT_EQ(e,              d.lastChild());
	ASSERT_EQ(EntityRef(),    e.lastChild());
	ASSERT_EQ(EntityRef(),    f.lastChild());

	ASSERT_STREQ("__root__", root.name());
	ASSERT_STREQ("a",           a.name());
	ASSERT_STREQ("b",           b.name());
	ASSERT_STREQ("c",           c.name());
	ASSERT_STREQ("d",           d.name());
	ASSERT_STREQ("e",           e.name());
	ASSERT_STREQ("f",           f.name());
}

TEST_F(EntityManagerTest, EntityComponentList) {
	Component c0(nullptr, nullptr);
	Component c1(nullptr, nullptr);
	Component c2(nullptr, nullptr);

	_Entity e;
	e.reset();
	ASSERT_EQ(0, e._countComponents());

	e._addComponent(&c0);
	ASSERT_EQ(1, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c0));

	e._addComponent(&c1);
	ASSERT_EQ(2, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c0));
	ASSERT_TRUE(e._hasComponent(&c1));

	e._addComponent(&c2);
	ASSERT_EQ(3, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c0));
	ASSERT_TRUE(e._hasComponent(&c1));
	ASSERT_TRUE(e._hasComponent(&c2));

	e._removeComponent(&c1);
	ASSERT_EQ(2, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c0));
	ASSERT_TRUE(e._hasComponent(&c2));

	e._removeComponent(&c0);
	ASSERT_EQ(1, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c2));

	e._removeComponent(&c2);
	ASSERT_EQ(0, e._countComponents());

	e._addComponent(&c0);
	e._addComponent(&c1);
	e._updateComponent(&c1, &c2);
	std::swap(c1._nextComponent, c2._nextComponent);
	ASSERT_EQ(2, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c0));
	ASSERT_TRUE(e._hasComponent(&c2));

	e._updateComponent(&c0, &c1);
	std::swap(c0._nextComponent, c1._nextComponent);
	ASSERT_EQ(2, e._countComponents());
	ASSERT_TRUE(e._hasComponent(&c1));
	ASSERT_TRUE(e._hasComponent(&c2));
}

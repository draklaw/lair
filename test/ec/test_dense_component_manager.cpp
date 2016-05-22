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

#include <lair/core/json.h>

#include <lair/ec/entity_manager.h>
#include <lair/ec/dense_component_manager.h>


#define BLOCK_SIZE 4

using namespace lair;


size_t countComponents(EntityRef entity) {
	return entity._get()->_countComponents();
}

bool hasComponent(EntityRef entity, Component* target) {
	return entity._get()->_hasComponent(target);
}

template<typename Component>
class TestComponentManager;

enum {
	TEST0 = USER_COMPONENT_INDEX,
	TEST1,
};

template<int _Index>
class TestComponent : public Component {
public:
	typedef TestComponentManager<TestComponent<_Index>> Manager;

	enum {
		INDEX = _Index,
	};

public:
	TestComponent(Manager* manager, _Entity* entity);

public:
	int value;
};

template<typename Component>
class TestComponentManager : public DenseComponentManager<Component> {
public:
	typedef DenseComponentManager<Component> Base;
public:
	TestComponentManager(const std::string& name, size_t blockSize)
	    : DenseComponentManager<Component>(name, blockSize) {
	}

	virtual Component* addComponentFromJson(EntityRef entity, const Json::Value& json,
	                                                   const Path& cd=Path()) {
		Component* comp = Base::addComponent(entity);
		comp->value = json.get("value", 0).asInt();
		return comp;
	}

	virtual Component* cloneComponent(EntityRef base, EntityRef entity) {
		Component* comp = Base::addComponent(entity);
		comp->value = Base::getComponent(entity)->value;
		return comp;
	}
};

template<int _Index>
TestComponent<_Index>::TestComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity),
      value(0) {
}

typedef TestComponent<TEST0> Component0;
typedef TestComponent<TEST1> Component1;

typedef TestComponentManager<Component0> Manager0;
typedef TestComponentManager<Component1> Manager1;

bool cmpComponent(Component1* c0, Component1* c1) {
	return c0->value < c1->value;
}

class DenseComponentManagerTest : public ::testing::Test {
public:
	EntityManager* em;
	Manager0* manager0;
	Manager1* manager1;
	EntityRef root;
	EntityRef a;
	EntityRef b;
	EntityRef c;
	EntityRef d;
	EntityRef e;
	EntityRef f;
	Component0* compC0;
	Component0* compD0;
	Component0* compE0;
	Component1* compR1;
	Component1* compA1;
	Component1* compC1;
	Component1* compD1;
	Component1* compF1;

	DenseComponentManagerTest()
	    : em      (nullptr),
	      manager0(nullptr){
	}

	virtual void SetUp() {
		em = new EntityManager(noopLogger, BLOCK_SIZE);
		manager0 = new Manager0("test0", 4);
		manager1 = new Manager1("test1", 4);

		em->registerComponentManager(manager0);
	}

	virtual void TearDown() {
		root.release();
		a.release();
		b.release();
		c.release();
		d.release();
		e.release();
		f.release();
		delete manager0;
		delete em;
	}

	void buildTree() {
		root = em->root();
		a = em->createEntity(root, "a");
		b = em->createEntity(a,    "b");
		c = em->createEntity(root, "c");
		d = em->createEntity(a,    "d");
		e = em->createEntity(d,    "e");
		f = em->createEntity(a,    "f");
	}

	void addComponents() {
		compC0 = manager0->addComponent(c);
		compD0 = manager0->addComponent(d);
		compE0 = manager0->addComponent(e);

		compR1 = manager1->addComponent(root);
		compA1 = manager1->addComponent(a);
		compC1 = manager1->addComponent(c);
		compD1 = manager1->addComponent(d);
		compF1 = manager1->addComponent(f);

		compC0->value = 1;
		compD0->value = 2;
		compE0->value = 3;
		compR1->value = 42;
		compA1->value = 4;
		compC1->value = 5;
		compD1->value = 6;
		compF1->value = 7;
	}
};

TEST_F(DenseComponentManagerTest, InitShutdown) {
	buildTree();
}

TEST_F(DenseComponentManagerTest, AddComponent) {
	buildTree();
	addComponents();

	ASSERT_EQ(3, manager0->nComponents());
	ASSERT_EQ(0, manager0->nZombies());
	ASSERT_EQ(4, manager0->capacity());

	ASSERT_EQ(5, manager1->nComponents());
	ASSERT_EQ(0, manager1->nZombies());
	ASSERT_EQ(8, manager1->capacity());

	ASSERT_EQ(nullptr, manager0->getComponent(root));
	ASSERT_EQ(nullptr, manager0->getComponent(a));
	ASSERT_EQ(nullptr, manager0->getComponent(b));
	ASSERT_EQ(compC0,  manager0->getComponent(c));
	ASSERT_EQ(compD0,  manager0->getComponent(d));
	ASSERT_EQ(compE0,  manager0->getComponent(e));
	ASSERT_EQ(nullptr, manager0->getComponent(f));

	ASSERT_EQ(compR1,  manager1->getComponent(root));
	ASSERT_EQ(compA1,  manager1->getComponent(a));
	ASSERT_EQ(nullptr, manager1->getComponent(b));
	ASSERT_EQ(compC1,  manager1->getComponent(c));
	ASSERT_EQ(compD1,  manager1->getComponent(d));
	ASSERT_EQ(nullptr, manager1->getComponent(e));
	ASSERT_EQ(compF1,  manager1->getComponent(f));

	ASSERT_EQ(1, countComponents(root));
	ASSERT_EQ(1, countComponents(a));
	ASSERT_EQ(0, countComponents(b));
	ASSERT_EQ(2, countComponents(c));
	ASSERT_EQ(2, countComponents(d));
	ASSERT_EQ(1, countComponents(e));
	ASSERT_EQ(1, countComponents(f));

	ASSERT_TRUE(hasComponent(root, compR1));
	ASSERT_TRUE(hasComponent(a,    compA1));
	ASSERT_EQ(nullptr, b._get()->firstComponent);
	ASSERT_TRUE(hasComponent(c,    compC0));
	ASSERT_TRUE(hasComponent(c,    compC1));
	ASSERT_TRUE(hasComponent(d,    compD0));
	ASSERT_TRUE(hasComponent(d,    compD1));
	ASSERT_TRUE(hasComponent(e,    compE0));
	ASSERT_TRUE(hasComponent(f,    compF1));

	ASSERT_EQ(1, manager0->getComponent(c)->value);
	ASSERT_EQ(2, manager0->getComponent(d)->value);
	ASSERT_EQ(3, manager0->getComponent(e)->value);

	ASSERT_EQ(42, manager1->getComponent(root)->value);
	ASSERT_EQ(4,  manager1->getComponent(a)   ->value);
	ASSERT_EQ(5,  manager1->getComponent(c)   ->value);
	ASSERT_EQ(6,  manager1->getComponent(d)   ->value);
	ASSERT_EQ(7,  manager1->getComponent(f)   ->value);
}

TEST_F(DenseComponentManagerTest, RemoveComponent) {
	buildTree();
	addComponents();

	manager1->removeComponent(a);
	manager0->removeComponent(c);
	manager1->removeComponent(d);
	manager0->removeComponent(e);

	ASSERT_EQ(1, manager0->nComponents());
	ASSERT_EQ(2, manager0->nZombies());
	ASSERT_EQ(4, manager0->capacity());

	ASSERT_EQ(3, manager1->nComponents());
	ASSERT_EQ(2, manager1->nZombies());
	ASSERT_EQ(8, manager1->capacity());

	ASSERT_EQ(nullptr, manager0->getComponent(root));
	ASSERT_EQ(nullptr, manager0->getComponent(a));
	ASSERT_EQ(nullptr, manager0->getComponent(b));
	ASSERT_EQ(nullptr, manager0->getComponent(c));
	ASSERT_EQ(compD0,  manager0->getComponent(d));
	ASSERT_EQ(nullptr, manager0->getComponent(e));
	ASSERT_EQ(nullptr, manager0->getComponent(f));

	ASSERT_EQ(compR1,  manager1->getComponent(root));
	ASSERT_EQ(nullptr, manager1->getComponent(a));
	ASSERT_EQ(nullptr, manager1->getComponent(b));
	ASSERT_EQ(compC1,  manager1->getComponent(c));
	ASSERT_EQ(nullptr, manager1->getComponent(d));
	ASSERT_EQ(nullptr, manager1->getComponent(e));
	ASSERT_EQ(compF1,  manager1->getComponent(f));

	ASSERT_EQ(1, countComponents(root));
	ASSERT_EQ(0, countComponents(a));
	ASSERT_EQ(0, countComponents(b));
	ASSERT_EQ(1, countComponents(c));
	ASSERT_EQ(1, countComponents(d));
	ASSERT_EQ(0, countComponents(e));
	ASSERT_EQ(1, countComponents(f));

	ASSERT_TRUE(hasComponent(root, compR1));
	ASSERT_EQ(nullptr, a._get()->firstComponent);
	ASSERT_EQ(nullptr, b._get()->firstComponent);
	ASSERT_TRUE(hasComponent(c,    compC1));
	ASSERT_TRUE(hasComponent(d,    compD0));
	ASSERT_EQ(nullptr, e._get()->firstComponent);
	ASSERT_TRUE(hasComponent(f,    compF1));

	ASSERT_EQ(2, manager0->getComponent(d)->value);

	ASSERT_EQ(42, manager1->getComponent(root)->value);
	ASSERT_EQ(5,  manager1->getComponent(c)   ->value);
	ASSERT_EQ(7,  manager1->getComponent(f)   ->value);
}

TEST_F(DenseComponentManagerTest, CompactArray) {
	buildTree();
	addComponents();

	manager1->removeComponent(a);
	manager0->removeComponent(c);
	manager1->removeComponent(d);
	manager0->removeComponent(e);

	manager0->compactArray();
	manager1->compactArray();

	ASSERT_EQ(1, manager0->nComponents());
	ASSERT_EQ(0, manager0->nZombies());
	ASSERT_EQ(4, manager0->capacity());

	ASSERT_EQ(3, manager1->nComponents());
	ASSERT_EQ(0, manager1->nZombies());
	ASSERT_EQ(8, manager1->capacity());

	ASSERT_EQ(2, manager0->getComponent(d)->value);

	ASSERT_EQ(42, manager1->getComponent(root)->value);
	ASSERT_EQ(5,  manager1->getComponent(c)   ->value);
	ASSERT_EQ(7,  manager1->getComponent(f)   ->value);
}

TEST_F(DenseComponentManagerTest, Iterator) {
	buildTree();
	addComponents();

	Manager1::Iterator it  = manager1->begin();
	Manager1::Iterator end = manager1->end();

	ASSERT_EQ(42, (*it).value);
	++it;
	ASSERT_EQ(4,  it->value);
	ASSERT_EQ(5,  (++it)->value);
	it++;
	ASSERT_EQ(6,  (*(it++)).value);
	ASSERT_EQ(7,  (it++)->value);
	ASSERT_EQ(end, it);

	manager1->removeComponent(root);
	manager1->removeComponent(c);
	manager1->removeComponent(f);

	it  = manager1->begin();
	end = manager1->end();

	ASSERT_EQ(4,  (it++)->value);
	ASSERT_EQ(6,  (it++)->value);
	ASSERT_EQ(end, it);
}

TEST_F(DenseComponentManagerTest, SortArray) {
	buildTree();
	addComponents();

	int perm0[] = { 1, 2, 3, 4, 0 };
	int perm1[] = { 4, 0, 1, 2, 3 };
	int perm2[] = { 1, 4, 3, 2, 0 };

	int i = 0;
	for(Component1& comp: *manager1) {
		comp.value = perm0[i++];
	}
	manager1->sortArray(cmpComponent);
	i = 0;
	for(Component1& comp: *manager1) {
		ASSERT_EQ(i++, comp.value);
	}

	i = 0;
	for(Component1& comp: *manager1) {
		comp.value = perm1[i++];
	}
	manager1->sortArray(cmpComponent);
	i = 0;
	for(Component1& comp: *manager1) {
		ASSERT_EQ(i++, comp.value);
	}

	i = 0;
	for(Component1& comp: *manager1) {
		comp.value = perm2[i++];
	}
	manager1->sortArray(cmpComponent);
	i = 0;
	for(Component1& comp: *manager1) {
		ASSERT_EQ(i++, comp.value);
	}

	i = 0;
	for(Component1& comp: *manager1) {
		comp.value = perm2[i++];
		if((i % 2) == 1) {
			manager1->removeComponent(comp.entity());
		}
	}
	ASSERT_EQ(2, manager1->nComponents());
	ASSERT_EQ(3, manager1->nZombies());
	ASSERT_EQ(8, manager1->capacity());

	manager1->sortArray(cmpComponent);

	ASSERT_EQ(2, manager1->nComponents());
	ASSERT_EQ(0, manager1->nZombies());
	ASSERT_EQ(8, manager1->capacity());

	Manager1::Iterator it  = manager1->begin();
	Manager1::Iterator end = manager1->end();
	ASSERT_EQ(2, (it++)->value);
	ASSERT_EQ(4, (it++)->value);
	ASSERT_EQ(end, it);
}

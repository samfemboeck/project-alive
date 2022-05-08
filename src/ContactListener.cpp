#include "ContactListener.h"
#include "box2d/b2_contact.h"
#include "Cell.h"
#include "LeafCell.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	auto fix_a = contact->GetFixtureA();
	auto fix_b = contact->GetFixtureB();

	if (fix_a->GetUserData().pointer == 0 && fix_b->GetUserData().pointer != 0)
	{
		auto* leaf_b = dynamic_cast<LeafCell*>(reinterpret_cast<Cell*>(fix_b->GetUserData().pointer));
		if (leaf_b)
		{
			leaf_b->LightLevel += 1;
		}
	}
	else if (fix_b->GetUserData().pointer == 0 && fix_a->GetUserData().pointer != 0)
	{
		auto* leaf_a = dynamic_cast<LeafCell*>(reinterpret_cast<Cell*>(fix_a->GetUserData().pointer));
		if (leaf_a)
		{
			leaf_a->LightLevel += 1;
		}
	}
}

void ContactListener::EndContact(b2Contact* contact)
{	
	auto fix_a = contact->GetFixtureA();
	auto fix_b = contact->GetFixtureB();

	if (fix_a->GetUserData().pointer == 0 && fix_b->GetUserData().pointer != 0)
	{
		auto* leaf_b = dynamic_cast<LeafCell*>(reinterpret_cast<Cell*>(fix_b->GetUserData().pointer));
		if (leaf_b)
		{
			leaf_b->LightLevel -= 1;
		}
	}
	else if (fix_b->GetUserData().pointer == 0 && fix_a->GetUserData().pointer != 0)
	{
		auto* leaf_a = dynamic_cast<LeafCell*>(reinterpret_cast<Cell*>(fix_a->GetUserData().pointer));
		if (leaf_a)
		{
			leaf_a->LightLevel -= 1;
		}
	}
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
}

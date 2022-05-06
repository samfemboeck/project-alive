#pragma once

class Physics
{
public:
	static void update(float delta);
	static b2Body* create_body(const b2BodyDef* def);
};

#pragma once

class Physics
{
public:
	static void update(float delta);
	static b2Body* create_body(const b2BodyDef* def);
	static void destroy_body(b2Body* body);
	static void set_listener(b2ContactListener* listener);
};

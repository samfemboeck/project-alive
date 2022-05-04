module;
#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
export module Physics;

import QuickMaths;

static Vec2 gravity(0.0f, 0.0f);
static b2World world(gravity);	
static const float time_step = 1.0f / 60.0f;
static const int32 velocity_iterations = 6;
static int32 position_iterations = 2;

export class Physics
{
public:
	static void update(float delta)
	{
		static float accumulator = 0;
		accumulator += delta;

		while (accumulator > time_step)
		{
			world.Step(time_step, velocity_iterations, position_iterations);
			accumulator -= time_step;
		}
	}

	static b2Body* create_body(const b2BodyDef* def)
	{
		return world.CreateBody(def);
	}
};

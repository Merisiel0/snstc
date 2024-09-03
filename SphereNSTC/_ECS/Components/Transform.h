#pragma once

#include "BaseComponent.h"
#include "_utils/Math.h"

struct Transform : public ECS::BaseComponent {
	vec3 position{ 0,0,0 };
	quat rotation{ 0,0,0,1 };
	vec3 scale{ 1,1,1 };

	Transform() = default;
	Transform(vec3 position, quat rotation, vec3 scale)
		: position{ position }, rotation{ rotation }, scale{ scale } {
	}
	Transform(const Transform& t) : Transform(t.position, t.rotation, t.scale) {}

	mat4 getModelMatrix() const;
	void rotate(vec3 angles);
};
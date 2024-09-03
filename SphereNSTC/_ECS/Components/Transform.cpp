#include "Transform.h"

mat4 Transform::getModelMatrix() const {
	mat4 matrix = mat4(1.0f);
	matrix = translate(matrix, position);
	matrix = glm::rotate(matrix, angle(rotation), axis(rotation));
	matrix = glm::scale(matrix, scale);
	return matrix;
}

void Transform::rotate(vec3 angles) {
	rotation = glm::rotate(rotation, angles.x, { 1,0,0 });
	rotation = glm::rotate(rotation, angles.y, { 0,1,0 });
	rotation = glm::rotate(rotation, angles.z, { 0,0,1 });
}
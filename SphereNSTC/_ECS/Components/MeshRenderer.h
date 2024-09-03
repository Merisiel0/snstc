#pragma once

#include "_resources/Mesh.h"
#include "BaseComponent.h"

class Material;

struct MeshRenderer : public ECS::BaseComponent {
	Mesh* mesh;
	Material* material;

	MeshRenderer() = default;
	MeshRenderer(Mesh* mesh, Material* material) : mesh{ mesh }, material{ material } {};
	MeshRenderer(const MeshRenderer& mr) : MeshRenderer(mr.mesh, mr.material) {};
};
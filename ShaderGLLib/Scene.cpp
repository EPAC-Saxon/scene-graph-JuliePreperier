#include "Scene.h"
#include <stdexcept>

namespace sgl {

	const sgl::matrix SceneMatrix::GetLocalModel(const double dt) const
	{
		if (parent_ == nullptr) {
			return matrix_;
		}
		return matrix_ * parent_->GetLocalModel(dt);
	}

	const std::shared_ptr<sgl::Mesh> SceneMatrix::GetLocalMesh() const
	{
		return nullptr;
	}

	const sgl::matrix SceneMesh::GetLocalModel(const double dt) const
	{
		sgl::matrix m;
		m.IdentityMatrix();

		if (parent_ == nullptr) {
			return m;
		}
		return parent_->GetLocalModel(dt);
	}

	const std::shared_ptr<sgl::Mesh> SceneMesh::GetLocalMesh() const
	{
		return mesh_;
	}

	void SceneTree::AddNode(
		const std::shared_ptr<Scene>& node, 
		const std::shared_ptr<Scene>& parent /*= nullptr*/)
	{
		node->SetParent(parent);
		push_back(node);
	}

	const std::shared_ptr<Scene> SceneTree::GetRoot() const
	{
		bool isfounded = false;
		std::shared_ptr<Scene> root;
		for (auto& node : *this) {
			auto parent = node->GetParent();
			if (parent == nullptr && node->IsRoot() && !isfounded) {
				isfounded = true;
				root = node;
			}
			else {
				throw std::runtime_error("There is too many roots");
			}
		}
		return root;
	}

} // End namespace sgl.

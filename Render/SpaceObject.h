#pragma once
#include <cassert>
#include <vector>
#include "ISpaceObject.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Quaternionf;
		class SpaceObject : public virtual ISpaceObject, public std::enable_shared_from_this<SpaceObject>
		{
		public:
			SpaceObject(){}
			SpaceObject(const std::shared_ptr<ISpaceObject>& parent) : mParent(parent){}
			Transform& GetTransform()override { return mTransform; }
			std::shared_ptr<ISpaceObject> GetParent()const override { return mParent.lock(); }
			std::size_t GetChildrenCount()const override { return mChildren.size(); }
			void SetParent(const std::shared_ptr<ISpaceObject>& parent)override
			{
				if (parent.get() == this)
					return;
				if (auto p = mParent.lock())
				{
					p->RemoveChild(shared_from_this());
				}
				if (parent)
				{
					parent->AddChild(shared_from_this());
				}
				mParent = parent;
			}
			std::shared_ptr<ISpaceObject> GetChild(std::size_t index)const override
			{
				if (index >= GetChildrenCount())
					return nullptr;
				return mChildren[index];
			}
			bool AddChild(const std::shared_ptr<ISpaceObject>& child)override
			{
				if (!child)
					return false;

				if (child.get() == this)
					return false;

				for (const auto& c : mChildren)
				{
					if (c == child)
						return false;
				}

				if (auto parent = child->GetParent())
				{
					parent->RemoveChild(child);
				}

				mChildren.emplace_back(child);
				child->SetParent(shared_from_this());
				return true;
			}

			bool RemoveChild(const std::shared_ptr<ISpaceObject>& child)override
			{
				for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
				{
					if (*it == child)
					{
						mChildren.erase(it);
						child->SetParent(nullptr);
						return true;
					}
				}
				return false;
			}

			Transform GetGlobalTransform()const override
			{
				auto matrix(mTransform.LocalToGlobalMatrix4());
				auto object(shared_from_this());
				Vector3f globalScale(mTransform.GetScale());
				auto parent = object->GetParent();
				while (parent)
				{
					const auto& parentTransform = parent->GetTransform();
					matrix *= parentTransform.LocalToGlobalMatrix4();
					const auto& parentScale = parentTransform.GetScale();
					globalScale.x *= parentScale.x;
					globalScale.y *= parentScale.y;
					globalScale.z *= parentScale.z;
					parent = parent->GetParent();
				}
				auto globalPosition = Vector4f{0.f, 0.f, 0.f, 1.f} * matrix;
				auto globalForward = Vector4f{ 0.f, 0.f, 1.f, 0.f } * matrix;
				auto globalUp = Vector4f{ 0.f, 1.f, 0.f, 0.f } * matrix;
				Transform globalTransform(Vector3f{globalPosition.x, globalPosition.y, globalPosition.z}, 
					globalScale, Quaternionf::Identity());
				globalTransform.OrientTo(Vector3f{ globalForward.x, globalForward.y, globalForward.z }, Vector3f{globalUp.x, globalUp.y, globalUp.z});

				return globalTransform;
			}

		protected:
			Transform mTransform;
			std::weak_ptr<ISpaceObject> mParent;
			std::vector<std::shared_ptr<ISpaceObject>> mChildren;
		};
	}
}
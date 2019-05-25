#pragma once
#include <cassert>
#include <vector>
#include "ISpaceObject.h"

namespace Lightning
{
	namespace Render
	{
		class SpaceObject : public virtual ISpaceObject, public std::enable_shared_from_this<SpaceObject>
		{
		public:
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

			Matrix4f LocalToGlobalMatrix()const override
			{
				auto matrix(mTransform.LocalToGlobalMatrix4());
				auto object(shared_from_this());
				while (auto parent = object->GetParent())
				{
					matrix *= parent->GetTransform().LocalToGlobalMatrix4();
					object = parent;
				}
				return matrix;
			}

			Matrix4f GlobalToLocalMatrix()const override
			{
				auto matrix(mTransform.GlobalToLocalMatrix4());
				auto object(shared_from_this());
				while (auto parent = object->GetParent())
				{
					matrix *= parent->GetTransform().GlobalToLocalMatrix4();
					object = parent;
				}

				return matrix;
			}

		protected:
			Transform mTransform;
			std::weak_ptr<ISpaceObject> mParent;
			std::vector<std::shared_ptr<ISpaceObject>> mChildren;
		};
	}
}
#pragma once
#include <memory>
#include "Transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Transform;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Quaternionf;
		struct ISpaceObject
		{
			virtual ~ISpaceObject() = default;
			virtual Transform& GetTransform() = 0;
			virtual std::shared_ptr<ISpaceObject> GetParent()const = 0;
			virtual std::size_t GetChildrenCount()const = 0;
			virtual void SetParent(const std::shared_ptr<ISpaceObject>& parent) = 0;
			virtual std::shared_ptr<ISpaceObject> GetChild(std::size_t index)const = 0;
			virtual bool AddChild(const std::shared_ptr<ISpaceObject>& child) = 0;
			virtual bool RemoveChild(const std::shared_ptr<ISpaceObject>& child) = 0;
			virtual Transform GetGlobalTransform()const = 0;
			virtual void SetGlobalTransform(const Transform& transform) = 0;
			virtual void SetGlobalPosition(const Vector3f& position) = 0;
			virtual void SetGlobalRotation(const Quaternionf& rotation) = 0;
			virtual void SetGlobalScale(const Vector3f& scale) = 0;
		};
	}
}
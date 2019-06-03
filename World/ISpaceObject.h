#pragma once
#include <memory>
#include <cstdint>
#include <functional>
#include "Transform.h"

namespace Lightning
{
	namespace World
	{
		using Foundation::Math::Transform;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Quaternionf;
		enum class SpaceObjectTraversalPolocy
		{
			Sequential,
			Concurrent
		};

		struct ISpaceObject
		{
			virtual ~ISpaceObject() = default;
			virtual const std::uint64_t GetID()const = 0;
			virtual Transform& GetLocalTransform() = 0;
			virtual std::shared_ptr<ISpaceObject> GetParent()const = 0;
			virtual std::size_t GetChildrenCount()const = 0;
			//add a child to an object, thread safe
			virtual bool AddChild(const std::shared_ptr<ISpaceObject>& child) = 0;
			//remove a child of an object, thread safe
			virtual bool RemoveChild(const std::shared_ptr<ISpaceObject>& child) = 0;
			virtual void Traverse(std::function<void(const std::shared_ptr<ISpaceObject>& object)> visitor, SpaceObjectTraversalPolocy policy) = 0;
			virtual Transform GetGlobalTransform()const = 0;
			virtual void SetGlobalTransform(const Transform& transform) = 0;
			virtual void SetGlobalPosition(const Vector3f& position) = 0;
			virtual void SetGlobalRotation(const Quaternionf& rotation) = 0;
			virtual void SetGlobalScale(const Vector3f& scale) = 0;
		};
	}
}
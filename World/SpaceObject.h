#pragma once
#include <cassert>
#include <list>
#include <unordered_map>
#include "ISpaceObject.h"
#include "SpaceObjectManager.h"

namespace Lightning
{
	namespace World
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Quaternionf;

		class SpaceObjectBase : public virtual ISpaceObject
		{
		public:
			SpaceObjectBase() : mID(SpaceObjectManager::Instance()->GetNextSpaceObjectID())
			{
			}
			const std::uint64_t GetID()const override { return mID; }
		protected:
			friend class SpaceObjectManager;
			Transform mTransform;
			std::weak_ptr<ISpaceObject> mParent;
			std::list<std::shared_ptr<ISpaceObject>> mChildren;
			const std::uint64_t mID;
		};

		template<typename Derived>
		class SpaceObjectImpl : public SpaceObjectBase, public std::enable_shared_from_this<Derived>
		{
		public:
			SpaceObjectImpl(){}
			SpaceObjectImpl(const std::shared_ptr<ISpaceObject>& parent) : mParent(parent){}
			Transform& GetLocalTransform()override { return mTransform; }
			std::shared_ptr<ISpaceObject> GetParent()const override { return mParent.lock(); }
			std::size_t GetChildrenCount()const override { return mChildren.size(); }

			bool AddChild(const std::shared_ptr<ISpaceObject>& child)override
			{
				return SpaceObjectManager::Instance()->AddChild(shared_from_this(),std::dynamic_pointer_cast<SpaceObjectBase>(child));
			}

			bool RemoveChild(const std::shared_ptr<ISpaceObject>& child)override
			{
				return SpaceObjectManager::Instance()->RemoveChild(shared_from_this(), std::dynamic_pointer_cast<SpaceObjectBase>(child));
			}

			void Traverse(std::function<void(const std::shared_ptr<ISpaceObject>& object)> visitor, 
				SpaceObjectTraversalPolocy policy)override
			{
				//TODO : implement traversal based on policy setting
				visitor(shared_from_this());
				for (const auto& child : mChildren)
				{
					child->Traverse(visitor, policy);
				}
			}

			Transform GetGlobalTransform()const override
			{
				auto parent = GetParent();
				if (!parent)
				{
					return mTransform;
				}
				auto matrix = mTransform.LocalToGlobalMatrix4() * parent->GetGlobalTransform().LocalToGlobalMatrix4();

				//Convert Vector4{0, 0, 0, 1} with matrix yields the global position
				auto globalPosition = Vector3f{ matrix.GetCell(3, 0), matrix.GetCell(3, 1), matrix.GetCell(3, 2) };
				//Convert Vector4{0, 0, 1, 0} with matrix yields the global forward
				auto globalForward = Vector3f{ matrix.GetCell(2, 0), matrix.GetCell(2, 1), matrix.GetCell(2, 2) };
				//Convert Vector4{0, 1, 0, 0} with matrix yields the global up
				auto globalUp = Vector3f{ matrix.GetCell(1, 0), matrix.GetCell(1, 1), matrix.GetCell(1, 2) };
				//Convert Vector4{1, 0, 0, 0} with matrix yields the global right
				auto globalRight = Vector3f{ matrix.GetCell(0, 0), matrix.GetCell(0, 1), matrix.GetCell(0, 2) };

				Vector3f globalScale{ globalRight.Length(), globalUp.Length(), globalForward.Length() };

				Transform globalTransform;
				globalTransform.SetScale(globalScale);
				globalTransform.OrientTo(globalForward, globalUp);
				globalTransform.SetPosition(globalPosition);

				return globalTransform;
			}

			void SetGlobalTransform(const Transform& transform)override
			{
				auto parent = GetParent();
				if (!parent)
				{
					mTransform = transform;
					return;
				}
				auto parentGlobalTransform = parent->GetGlobalTransform();
				SetGlobalScale(transform.GetScale(), parentGlobalTransform);
				SetGlobalRotation(transform.GetRotation(), parentGlobalTransform);
				SetGlobalPosition(transform.GetPosition(), parentGlobalTransform);
			}

			void SetGlobalPosition(const Vector3f& position)override
			{
				auto parent = GetParent();
				if (!parent)
				{
					mTransform.SetPosition(position);
					return;
				}
				SetGlobalPosition(position, parent->GetGlobalTransform());
			}

			void SetGlobalRotation(const Quaternionf& rotation)override
			{
				auto parent = GetParent();
				if (!parent)
				{
					mTransform.SetRotation(rotation);
					return;
				}
				SetGlobalRotation(rotation, parent->GetGlobalTransform());
			}

			void SetGlobalScale(const Vector3f& scale)override
			{
				auto parent = GetParent();
				if (!parent)
				{
					mTransform.SetScale(scale);
					return;
				}
				SetGlobalScale(scale, parent->GetGlobalTransform());
			}
		protected:
			void SetGlobalPosition(const Vector3f& position, const Transform& parentGlobalTransform)
			{
				auto localPosition = Vector4f{ position.x, position.y, position.z, 1.0f } * parentGlobalTransform.GlobalToLocalMatrix4();
				mTransform.SetPosition(Vector3f{ localPosition.x, localPosition.y, localPosition.z });
			}
			void SetGlobalRotation(const Quaternionf& rotation, const Transform& parentGlobalTransform)
			{
				auto globalForward = rotation * Vector3f::forward();
				auto globalUp = rotation * Vector3f::up();
				auto localForward = Vector4f{ globalForward.x, globalForward.y, globalForward.z, 0.f } * parentGlobalTransform.GlobalToLocalMatrix4();
				auto localUp = Vector4f{ globalUp.x, globalUp.y, globalUp.z, 0.f } * parentGlobalTransform.GlobalToLocalMatrix4();
				mTransform.OrientTo(Vector3f{localForward.x, localForward.y, localForward.z}, Vector3f{ localUp.x, localUp.y, localUp.z });
			}
			void SetGlobalScale(const Vector3f& scale, const Transform& parentGlobalTransform)
			{
				auto matrix = mTransform.LocalToGlobalMatrix4() * parentGlobalTransform.LocalToGlobalMatrix4();
				auto globalForward = Vector4f{ 0.f, 0.f, 1.f, 0.f } * matrix;
				auto globalUp = Vector4f{ 0.f, 1.f, 0.f, 0.f } * matrix;
				auto globalRight = Vector4f{ 1.f, 0.f, 0.f, 0.f } * matrix;

				mTransform.SetScale(Vector3f{ scale.x / globalRight.Length(), scale.y / globalUp.Length(), scale.z / globalForward.Length() });
			}
		};

		template<typename Interface, typename Implementation>
		class SpaceObject : public Interface, public SpaceObjectImpl<Implementation>
		{
		public:
			SpaceObject() : SpaceObjectImpl<Implementation>(){}
			SpaceObject(const std::shared_ptr<ISpaceObject>& parent) : SpaceObjectImpl<Implementation>(parent){}
			static_assert(std::is_base_of<ISpaceObject, Interface>::value, "Interface must be a subinterface of ISpaceObject");
		};


	}
}
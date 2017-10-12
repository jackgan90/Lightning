#pragma once
#include <memory>
#include "hashableobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::HashableObject;
		enum FillMode
		{
			FILLMODE_WIREFRAME,
			FILLMODE_SOLID
		};

		enum CullMode
		{
			CULLMODE_NONE,
			CULLMODE_FRONT,
			CULLMODE_BACK
		};

		enum FrontFaceWindingOrder
		{
			COUNTER_CLOCKWISE,
			CLOCKWISE
		};

		struct RasterizerStateConfiguration
		{
			RasterizerStateConfiguration() :fillMode(FILLMODE_SOLID), cullMode(CULLMODE_BACK), frontFaceWindingOrder(COUNTER_CLOCKWISE)
			{

			}
			FillMode fillMode;
			CullMode cullMode;
			FrontFaceWindingOrder frontFaceWindingOrder;
		};

		class IRasterizerState : public HashableObject
		{
		public:
			virtual ~IRasterizerState() = default;
			virtual const RasterizerStateConfiguration& GetConfiguration() = 0;
			virtual bool SetConfiguration(const RasterizerStateConfiguration& config) = 0;
			virtual bool SetFillMode(FillMode mode) = 0;
			virtual FillMode GetFillMode()const = 0;
			virtual bool SetCullMode(CullMode mode) = 0;
			virtual CullMode GetCullMode()const = 0;
			virtual bool SetFrontFaceWindingOrder(FrontFaceWindingOrder order) = 0;
			virtual FrontFaceWindingOrder GetFrontFaceWindingOrder()const = 0;
		};
		typedef std::shared_ptr<IRasterizerState> RasterizerStatePtr;

		class RasterizerState : public IRasterizerState
		{
		public:
			RasterizerState();
			const RasterizerStateConfiguration& GetConfiguration()override;
			bool SetConfiguration(const RasterizerStateConfiguration& config)override;
			bool SetFillMode(FillMode mode)override;
			FillMode GetFillMode()const override;
			bool SetCullMode(CullMode mode)override;
			CullMode GetCullMode()const override;
			bool SetFrontFaceWindingOrder(FrontFaceWindingOrder order)override;
			FrontFaceWindingOrder GetFrontFaceWindingOrder()const override;
		protected:
			size_t CalculateHashInternal()override;
			RasterizerStateConfiguration m_config;
		};
	}
}
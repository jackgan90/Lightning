#pragma once
#include <memory>

namespace LightningGE
{
	namespace Renderer
	{
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

		class IRasterizerState
		{
		public:
			virtual ~IRasterizerState(){}
			virtual const RasterizerStateConfiguration& GetConfiguration() = 0;
			virtual bool SetConfiguration(const RasterizerStateConfiguration& config) = 0;
			virtual bool SetFillMode(FillMode mode) = 0;
			virtual FillMode GetFillMode()const = 0;
			virtual bool SetCullMode(CullMode mode) = 0;
			virtual CullMode GetCullMode()const = 0;
			virtual bool SetFrontFaceWindingOrder(FrontFaceWindingOrder order) = 0;
			virtual FrontFaceWindingOrder GetFrontFaceWindingOrder()const = 0;
		};
		using RasterizerStatePtr = std::shared_ptr<IRasterizerState>;
	}
}
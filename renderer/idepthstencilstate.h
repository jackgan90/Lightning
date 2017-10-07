#pragma once
#include <memory>
#include "rendererexportdef.h"
#include "hashableobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::HashableObject;
		enum CmpFunc
		{
			NEVER,
			LESS,
			EQUAL,
			LESS_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_EQUAL,
			ALWAYS
		};

		enum StencilOp
		{
			KEEP,
			ZERO,
			REPLACE,
			INCREASE_CLAMP,
			DECREASE_CLAMP,
			INVERT,
			INCREASE_WRAP,
			DECREASE_WRAP
		};

		struct StencilFace
		{
			StencilFace() : cmpFunc(ALWAYS), passOp(KEEP), failOp(KEEP), depthFailOp(KEEP)
			{
			}
			CmpFunc cmpFunc;
			StencilOp passOp;
			StencilOp failOp;
			StencilOp depthFailOp;
		};

		struct DepthStencilConfiguration
		{
			DepthStencilConfiguration() : depthTestEnable(true), depthWriteEnable(true), depthCmpFunc(LESS)
				,stencilEnable(false), stencilRef(0), stencilReadMask(0xff), stencilWriteMask(0xff)
				,frontFace(), backFace()
			{

			}
			//depth config
			bool depthTestEnable;
			bool depthWriteEnable;
			CmpFunc depthCmpFunc;
			//stencil config
			bool stencilEnable;
			unsigned char stencilRef;
			unsigned char stencilReadMask;
			unsigned char stencilWriteMask;
			StencilFace frontFace;
			StencilFace backFace;
		};
		class LIGHTNINGGE_RENDERER_API IDepthStencilState : public HashableObject
		{
		public:
			virtual ~IDepthStencilState(){}
			virtual const DepthStencilConfiguration& GetConfiguration() = 0;
			virtual bool SetConfiguration(const DepthStencilConfiguration& configuration) = 0;
			virtual bool EnableDepthTest(bool enable) = 0;
			virtual bool EnableStenciltest(bool enable) = 0;
			virtual bool EnableDepthStencilTest(bool enable) = 0;
			virtual bool SetStencilRef(const unsigned char ref) = 0;
			//TODO basically there's still lots of methods need to add here,when it's necessary I will put them
		};
		typedef std::shared_ptr<IDepthStencilState> DepthStencilStatePtr;

		class DepthStencilState : public IDepthStencilState
		{
		public:
			const DepthStencilConfiguration& GetConfiguration()override;
			bool SetConfiguration(const DepthStencilConfiguration& configuration)override;
			bool EnableDepthTest(bool enable)override;
			bool EnableStenciltest(bool enable)override;
			bool EnableDepthStencilTest(bool enable)override;
			bool SetStencilRef(const unsigned char ref)override;
		protected:
			size_t CalculateHashInternal()override;
			DepthStencilConfiguration m_config;
		};
	}
}
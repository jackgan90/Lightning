#pragma once
#include "IDevice.h"
#include "IShader.h"
#include "ISerializer.h"
#include "ShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		class ShaderSerializer : public Loading::ISerializer
		{
		public:
			ShaderSerializer(ShaderType type, const std::string& path, 
				const ShaderMacros& macros, IShaderCallback* callback);
			INTERFACECALL ~ShaderSerializer()override;
			void INTERFACECALL Serialize(char** buffer)override;
			void INTERFACECALL Deserialize(Foundation::IFile* file, Loading::ISerializeBuffer* buffer)override;
			void INTERFACECALL Dispose()override;
		private:
			ShaderType mType;
			std::string mPath;
			ShaderMacros mMacros;
			IShaderCallback* mFinishCallback;
			IShader* mShader;
		};
	}
}
#pragma once
#include "idevice.h"
#include "ishader.h"
#include "ISerializer.h"

namespace Lightning
{
	namespace Render
	{
		class ShaderSerializer : public Loading::ISerializer
		{
		public:
			ShaderSerializer(ShaderType type, const std::string& path, 
				const ShaderDefine& defineMap, ShaderLoadFinishHandler handler);
			void Serialize(char** buffer)override;
			void Deserialize(const Foundation::SharedFilePtr& file, char* buffer)override;
			void Dispose()override;
		private:
			ShaderType mType;
			std::string mPath;
			ShaderDefine mDefineMap;
			ShaderLoadFinishHandler mFinishHandler;
		};
	}
}
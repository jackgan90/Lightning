#include <boost/functional/hash.hpp>
#include "logger.h"
#include "filesystemfactory.h"
#include "filesystem.h"
#include "d3d12shadermanager.h"
#include "d3d12shader.h"
#include "enginealgo.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSystemFactory;
		using Foundation::FileSystemPtr;
		using Foundation::FilePtr;
		using Foundation::FileAccess;
		using Foundation::logger;
		using Foundation::LogLevel;
		ShaderPtr D3D12ShaderManager::GetShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Utility::CalculateHash(shaderName, defineMap.GetAllDefine());
			boost::hash_combine(hash, type);
			auto it = m_shaders.find(hash);
			if (it != m_shaders.end())
				return it->second;
			FileSystemPtr fs = FileSystemFactory::FileSystem();
			FilePtr shaderFile = fs->FindFile(shaderName, FileAccess::ACCESS_READ);
			if (!shaderFile)
				return nullptr;
			D3D12Shader* pShader = new D3D12Shader(type);
			bool success = pShader->Compile(shaderFile, defineMap);
			if (!success)
			{
				std::string compileErrorLog = pShader->GetCompileErrorLog();
				logger.Log(LogLevel::Error, compileErrorLog);
				return nullptr;
			}
			logger.Log(LogLevel::Info, "Succeeded in compiling shader:%s", pShader->GetName().c_str());
			m_shaders[hash] = ShaderPtr(pShader);
			return m_shaders[hash];
		}
	}
}
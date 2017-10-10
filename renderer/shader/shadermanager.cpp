#include <algorithm>
#include <boost/functional/hash.hpp>
#include "shadermanager.h"
#include "enginealgo.h"
#include "filesystemfactory.h"
#include "filesystem.h"
#include "logger.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Foundation::FileSystemFactory;
		using Foundation::FilePtr;
		using Foundation::FileAccess;
		using Foundation::logger;
		using Foundation::LogLevel;

		ShaderManager::ShaderManager(const FileSystemPtr& fs)
		{
			m_fs = fs;
		}

		ShaderPtr ShaderManager::GetShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap)
		{
			std::size_t hash = Shader::Hash(type, shaderName, defineMap);
			auto it = m_shaders.find(hash);
			if (it != m_shaders.end())
				return it->second;
			FilePtr shaderFile = m_fs->FindFile(shaderName, FileAccess::ACCESS_READ);
			if (!shaderFile)
				return nullptr;
			ShaderPtr pShader = CreateConcreteShader(type);
			bool success = pShader->Compile(shaderFile, defineMap);
			if (!success)
			{
				std::string compileErrorLog = pShader->GetCompileErrorLog();
				logger.Log(LogLevel::Error, compileErrorLog);
				pShader.reset();
				return pShader;
			}
			logger.Log(LogLevel::Info, "Succeeded in compiling shader:%s", pShader->GetName().c_str());
			m_shaders[hash] = pShader;
			return m_shaders[hash];
		}
	}
}
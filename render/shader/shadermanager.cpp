#include <algorithm>
#include <boost/functional/hash.hpp>
#include "shadermanager.h"
#include "enginealgo.h"
#include "filesystemfactory.h"
#include "filesystem.h"
#include "logger.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::FileSystemFactory;
		using Foundation::SharedFilePtr;
		using Foundation::FilePointerType;
		using Foundation::FileAccess;
		using Foundation::FileAnchor;
		using Foundation::logger;
		using Foundation::LogLevel;
		using Foundation::StackAllocator;
		ShaderManager::ShaderManager(const SharedFileSystemPtr& fs)
		{
			m_fs = fs;
			m_compileAllocator = std::make_unique<StackAllocator<true, 16, 8192>>();
		}

		SharedShaderPtr ShaderManager::CreateShaderFromFile(ShaderType type, const std::string& shaderFileName, const ShaderDefine& defineMap)
		{
			auto hash = Shader::Hash(type, shaderFileName, defineMap);
			auto it = m_shaders.find(hash);
			if (it != m_shaders.end())
				return it->second;
			SharedFilePtr shaderFile = m_fs->FindFile(shaderFileName, FileAccess::READ);
			if (!shaderFile)
			{
				logger.Log(LogLevel::Error, "Shader file is missing!filename:%s", shaderFileName.c_str());
				return SharedShaderPtr();
			}
			auto size = shaderFile->GetSize();
			if (size <= 0)
			{
				logger.Log(LogLevel::Error, "Shader file is empty!file:", shaderFile->GetPath().c_str());
				return SharedShaderPtr();
			}
			shaderFile->SetFilePointer(FilePointerType::Read, FileAnchor::Begin, 0);
			char* buffer = ALLOC_ARRAY(m_compileAllocator, size+1, char);
			auto readSize = shaderFile->Read(buffer, size);
			if (readSize < size)
			{
				logger.Log(LogLevel::Error, "Failed to read shader file.Shader name:%s, file size:%d, read size:%d", shaderFile->GetPath().c_str(), size, readSize);
				DEALLOC(m_compileAllocator, buffer);
				return SharedShaderPtr();
			}
			buffer[size] = 0;
			SharedShaderPtr pShader = CreateConcreteShader(type, shaderFileName, buffer, defineMap);
			if(pShader)
			{
				logger.Log(LogLevel::Info, "Succeeded in compiling shader:%s", pShader->GetName().c_str());
				m_shaders[hash] = pShader;
				return m_shaders[hash];
			}
			return pShader;
		}

		SharedShaderPtr ShaderManager::GetShader(size_t shaderHash)
		{
			auto it = m_shaders.find(shaderHash);
			if (it != m_shaders.end())
				return it->second;
			return SharedShaderPtr();
		}

	}
}
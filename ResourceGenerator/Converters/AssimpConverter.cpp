#include "AssimpConverter.h"
#include "assimp/postprocess.h"

namespace Lightning
{
	namespace Tools
	{
		AssimpConverter::AssimpConverter(const std::string& inputPath, const std::string& outputPath)
			: mScene(nullptr)
		{
			mScene = mImporter.ReadFile(inputPath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
			if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
			{
			}
		}

		void AssimpConverter::Convert()
		{

		}

		void AssimpConverter::VisitNode(aiNode* node, aiScene* scene)
		{

		}

		void AssimpConverter::VisitMesh(aiMesh* mesh, aiScene* scene)
		{

		}
	}
}
#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/mesh.h"

namespace Lightning
{
	namespace Tools
	{
		class AssimpConverter
		{
		public:
			AssimpConverter(const std::string& inputPath, const std::string& outputPath);
			void Convert();
		private:
			void VisitNode(aiNode* node, aiScene* scene);
			void VisitMesh(aiMesh* mesh, aiScene* scene);
			Assimp::Importer mImporter;
			const aiScene* mScene;
			std::string mOutputPath;
		};
	}
}
#pragma once
#include <vector>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "Assets/MeshAsset.h"

namespace Lightning
{
	namespace Tools
	{
		class AssimpConverter
		{
		public:
			AssimpConverter(const std::string& inputFilePath, const std::string& outputDirectory);
			bool Convert();
		private:
			struct MeshMetadata
			{
				Asset::MeshHeader header;
				aiMesh* mesh;
			};
			void VisitNode(aiNode* node);
			MeshMetadata CreateMeshMetaData(aiMesh* mesh);
			Assimp::Importer mImporter;
			const aiScene* mScene;
			std::string mOutputDirectory;
			std::string mInputFilePath;
			Asset::MeshesHeader mMeshesHeader;
			std::vector<MeshMetadata> mMeshMetadatas;
		};
	}
}
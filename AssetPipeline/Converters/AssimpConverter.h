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
			AssimpConverter();
			bool Convert(const std::string& inputFilePath, const std::string& outputDirectory);
		private:
			struct MeshMetadata
			{
				Asset::MeshHeader header;
				aiMesh* mesh;
			};
			void VisitNode(aiNode* node);
			MeshMetadata CreateMeshMetaData(aiMesh* mesh);
			void FillMeshHeader(Asset::MeshHeader& header, const aiMesh* mesh, std::uint32_t& meshDataOffset);
			void CopyMeshData(const Asset::MeshHeader& header, const aiMesh* mesh, std::uint8_t* buffer);
			Assimp::Importer mImporter;
			const aiScene* mScene;
			Asset::MeshesHeader mMeshesHeader;
			std::vector<MeshMetadata> mMeshMetadatas;
		};
	}
}
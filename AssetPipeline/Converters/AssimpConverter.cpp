#include <iostream>
#include "Assets/AssetIdentity.h"
#include "AssimpConverter.h"
#include "assimp/postprocess.h"

namespace Lightning
{
	namespace Tools
	{
		template<typename T, std::size_t N>
		constexpr std::size_t array_size(T (&arr)[N])
		{
			return N;
		}

		AssimpConverter::AssimpConverter(const std::string& inputFilePath, const std::string& outputDirectory)
			: mScene(nullptr), mInputFilePath(inputFilePath), mOutputDirectory(outputDirectory)
		{
		}

		bool AssimpConverter::Convert()
		{
			mScene = mImporter.ReadFile(mInputFilePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
			if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
			{
				std::cerr << "Failed to read file : " << mInputFilePath << std::endl;
				return false;
			}
			mMeshesHeader.Magic = Asset::MESH_IDENTITY;
			mMeshesHeader.Version = Asset::MESH_VERSION;
			mMeshesHeader.NumberOfMeshes = 0;
			VisitNode(mScene->mRootNode);
			static constexpr auto MeshHeaderSize = sizeof(Asset::MeshHeader);
			std::uint32_t meshDataOffset{ static_cast<std::uint32_t>(sizeof(Asset::MeshesHeader) + MeshHeaderSize * mMeshMetadatas.size())};
			for (auto i = 0;i < mMeshMetadatas.size(); ++i)
			{
				auto& metadata = mMeshMetadatas[i];
				auto& header = metadata.header;
				auto mesh = metadata.mesh;

				if (mesh->mVertices)
				{
					++header.VertexStreams;
					header.Positions.Components = 3;
					header.Positions.Offset = meshDataOffset;
					header.VerticesCount = mesh->mNumVertices;
					meshDataOffset += header.Positions.Components * sizeof(float) * header.VerticesCount;
				}

				if (mesh->mNormals)
				{
					++header.VertexStreams;
					header.Normals.Components = 3;
					header.Normals.Offset = meshDataOffset;
					meshDataOffset += header.Normals.Components * sizeof(float) * header.VerticesCount;
				}

				if (mesh->mTangents)
				{
					++header.VertexStreams;
					header.Tangents.Components = 3;
					header.Tangents.Offset = meshDataOffset;
					meshDataOffset += header.Tangents.Components * sizeof(float) * header.VerticesCount;
				}

				if (mesh->mBitangents)
				{
					++header.VertexStreams;
					header.Bitagents.Components = 3;
					header.Bitagents.Offset = meshDataOffset;
					meshDataOffset += header.Bitagents.Components * sizeof(float) * header.VerticesCount;
				}

				std::size_t miscStreamIndex{ 0 };
				for (auto i = 0;i < array_size(mesh->mColors); ++i)
				{
					if (mesh->mColors[i])
					{
						++header.VertexStreams;
						auto& miscStream = header.MiscStreams[miscStreamIndex++];
						miscStream.Components = 4;
						miscStream.Offset = meshDataOffset;
						meshDataOffset += miscStream.Components * sizeof(float) * header.VerticesCount;
					}
				}

				for (auto i = 0;i < array_size(mesh->mTextureCoords); ++i)
				{
					if (mesh->mTextureCoords[i])
					{
						++header.VertexStreams;
						auto& miscStream = header.MiscStreams[miscStreamIndex++];
						miscStream.Components = mesh->mNumUVComponents[i];
						miscStream.Offset = meshDataOffset;
						meshDataOffset += miscStream.Components * sizeof(float) * header.VerticesCount;
					}
				}

				//Assimp ensure the index of a mesh is always present
				header.IndexSize = 4;
				header.IndexOffset = meshDataOffset;
				header.IndexCount = 0;
				if (mesh->mNumFaces > 0)
				{
					header.IndexCount = mesh->mNumFaces * mesh->mFaces[0].mNumIndices;
					meshDataOffset += header.IndexCount * header.IndexSize;
				}
			}

			std::uint8_t* buffer = new std::uint8_t[meshDataOffset];
			auto pMeshesHeader = reinterpret_cast<Asset::MeshesHeader*>(buffer);
			pMeshesHeader->Magic = mMeshesHeader.Magic;
			pMeshesHeader->Version = mMeshesHeader.Version;
			pMeshesHeader->NumberOfMeshes = mMeshesHeader.NumberOfMeshes;

			for (const auto& metadata : mMeshMetadatas)
			{
				const auto mesh = metadata.mesh;
				const auto& meshHeader = metadata.header;
				if (mesh->mVertices)
				{

				}
			}


			delete[] buffer;
			return true;
		}

		void AssimpConverter::VisitNode(aiNode* node)
		{
			mMeshesHeader.NumberOfMeshes += node->mNumMeshes;
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				auto mesh = mScene->mMeshes[node->mMeshes[i]];
				mMeshMetadatas.emplace_back(CreateMeshMetaData(mesh));
			}

			for (unsigned int i = 0;i < node->mNumChildren; ++i)
			{
				VisitNode(node->mChildren[i]);
			}
		}

		AssimpConverter::MeshMetadata AssimpConverter::CreateMeshMetaData(aiMesh* mesh)
		{
			MeshMetadata meshMetadata;
			//FIXME : Now all meshes are triangulated,so primitive type field is not used.
			//I may change the value of PrimitiveType in the future should demand arise.
			meshMetadata.mesh = mesh;
			auto& header = meshMetadata.header;
			header.PrimitiveType = 0;
			header.VertexStreams = 0;

			return meshMetadata;
		}
	}
}
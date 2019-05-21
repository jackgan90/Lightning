#include <iostream>
#include <fstream>
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

		AssimpConverter::AssimpConverter()
			: mScene(nullptr)
		{
		}

		bool AssimpConverter::Convert(const std::string& inputFilePath, const std::string& outputDirectory)
		{
			mScene = mImporter.ReadFile(inputFilePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
			if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
			{
				std::cerr << "Failed to read file : " << inputFilePath << std::endl;
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
				FillMeshHeader(header, mesh, meshDataOffset);
			}

			std::uint8_t* buffer = new std::uint8_t[meshDataOffset];
			std::memcpy(buffer, &mMeshesHeader, sizeof(mMeshesHeader));

			for (auto i = 0;i < mMeshMetadatas.size(); ++i)
			{
				const auto mesh = mMeshMetadatas[i].mesh;
				const auto& meshHeader = mMeshMetadatas[i].header;
				//copy header 
				std::memcpy(buffer + sizeof(mMeshesHeader) + i * sizeof(meshHeader), &meshHeader, sizeof(meshHeader));
				//copy vertex streams
				CopyMeshData(meshHeader, mesh, buffer);
			}

			std::string fileName(mScene->GetShortFilename(inputFilePath.c_str()));
			auto pos = fileName.rfind(".");
			if (pos != std::string::npos)
			{
				fileName = fileName.substr(0, pos);
			}
			fileName += ".mesh";
			std::string outputFilePath{ outputDirectory + "\\" + fileName };
			
			std::fstream fs;
			fs.open(outputFilePath, std::ofstream::out | std::ofstream::binary);
			fs.write(reinterpret_cast<char*>(buffer), meshDataOffset);
			fs.close();

			//delete[] buffer;
			return true;
		}

		void AssimpConverter::FillMeshHeader(Asset::MeshHeader& header, const aiMesh* mesh, std::uint32_t& meshDataOffset)
		{
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
			//FIXME : here I assume all the primitives of this mesh have the same primitive type
			//so I can use the first face's mNumIndices to represent all of the primitives' number of indices
			if (mesh->mNumFaces > 0)
			{
				header.IndexCount = mesh->mNumFaces * mesh->mFaces[0].mNumIndices;
				meshDataOffset += header.IndexCount * header.IndexSize;
			}
		}

		void AssimpConverter::CopyMeshData(const Asset::MeshHeader& header, const aiMesh* mesh, std::uint8_t* buffer)
		{
			//copy vertex streams
			if (mesh->mVertices)
			{
				std::memcpy(buffer + header.Positions.Offset, mesh->mVertices,
					header.Positions.Components * sizeof(float) * header.VerticesCount);
			}
			if (mesh->mNormals)
			{
				std::memcpy(buffer + header.Normals.Offset, mesh->mNormals,
					header.Normals.Components * sizeof(float) * header.VerticesCount);
			}
			if (mesh->mTangents)
			{
				std::memcpy(buffer + header.Tangents.Offset, mesh->mTangents,
					header.Tangents.Components * sizeof(float) * header.VerticesCount);
			}
			if (mesh->mBitangents)
			{
				std::memcpy(buffer + header.Bitagents.Offset, mesh->mBitangents,
					header.Bitagents.Components * sizeof(float) * header.VerticesCount);
			}
			std::size_t miscStreamIndex{ 0 };
			for (auto i = 0;i < array_size(mesh->mColors); ++i)
			{
				if (mesh->mColors[i])
				{
					auto& miscStream = header.MiscStreams[miscStreamIndex++];
					std::memcpy(buffer + miscStream.Offset, mesh->mColors[i],
						miscStream.Components * sizeof(float) * header.VerticesCount);
				}
			}

			for (auto i = 0;i < array_size(mesh->mTextureCoords); ++i)
			{
				if (mesh->mTextureCoords[i])
				{
					auto& miscStream = header.MiscStreams[miscStreamIndex++];
					std::memcpy(buffer + miscStream.Offset, mesh->mTextureCoords[i],
						miscStream.Components * sizeof(float) * header.VerticesCount);
				}
			}
			//copy index stream
			if (header.IndexCount > 0)
			{
				auto indexOffset = header.IndexOffset;
				for (unsigned int i = 0;i < mesh->mNumFaces; ++i)
				{
					aiFace face = mesh->mFaces[i];
					auto size = static_cast<std::uint32_t>(face.mNumIndices * sizeof(unsigned int));
					std::memcpy(buffer + indexOffset, face.mIndices, size);
					indexOffset += size;
				}
			}
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
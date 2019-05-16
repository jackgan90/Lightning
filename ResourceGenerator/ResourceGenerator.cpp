#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

int main(int argc, char** argv)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile("E:\\assimp\\test\\models\\3D\\box.uc",
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return -1;
	}
	std::cout << "Success import file " << std::endl;
	return 0;
}

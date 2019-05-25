#include <iostream>
#include "Converters/AssimpConverter.h"
#include "SpaceObject.h"

int main(int argc, char** argv)
{
	Lightning::Tools::AssimpConverter converter;
	std::string inputFile("E:\\assimp\\test\\models\\FBX\\spider.fbx");
	std::string outputDirectory("D:\\Lightning_res");
	auto result = converter.Convert(inputFile, outputDirectory);
	if (result)
	{
		std::cout << "Succeed in converting " << inputFile << std::endl;
	}
	return 0;
}

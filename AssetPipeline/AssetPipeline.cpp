#include <iostream>
#include "Converters/AssimpConverter.h"

int main(int argc, char** argv)
{
	Lightning::Tools::AssimpConverter converter;
	std::string inputFile("H:\\assimp\\test\\models\\FBX\\spider.fbx");
	std::string outputDirectory("F:\\Lightning_res");
	auto result = converter.Convert(inputFile, outputDirectory);
	if (result)
	{
		std::cout << "Succeed in converting " << inputFile << std::endl;
	}
	return 0;
}

#include <iostream>
#include "Converters/AssimpConverter.h"
#include "SpaceObject.h"

/*
using Lightning::Foundation::Math::Matrix4f;
using Lightning::Foundation::Math::Vector3f;
using Lightning::Foundation::Math::Vector4f;
using Lightning::Foundation::Math::Transform;
using Lightning::Render::SpaceObject;

Matrix4f GetMatrix(const std::shared_ptr<SpaceObject>& object)
{
	Matrix4f matrix(object->GetTransform().GetMatrix());
	auto p = object->GetParent();
	while (p)
	{
		matrix *= p->GetTransform().GetMatrix();
		p = p->GetParent();
	}

	return matrix;
}*/

int main(int argc, char** argv)
{
	/*
	auto o1 = std::make_shared<SpaceObject>();
	auto o2 = std::make_shared<SpaceObject>();
	auto o3 = std::make_shared<SpaceObject>();
	
	o2->SetParent(o1);
	o3->SetParent(o2);

	o1->GetTransform().SetRotation(Transform::RandomRotation());
	o1->GetTransform().SetScale(Vector3f{ 2.0f, 3.0f, 1.0f });
	o1->GetTransform().SetPosition(Vector3f{ 1.0f, 2.0f, 3.0f });
	//o1->GetTransform().OrientTo(Vector3f{ 0.f, 0.f, 1.f }, Vector3f{ 0.f, -1.f, 0.f });

	//auto globalTransform1 = o1->GetGlobalTransform();
	//auto globalUp1 = globalTransform1.GetRotation() * Vector3f { 0.f, 1.f, 0.f };
	//auto globalRight1 = globalTransform1.GetRotation() * Vector3f { 1.f, 0.f, 0.f };
	//auto globalForward1 = globalTransform1.GetRotation() * Vector3f { 0.f, 0.f, 1.f };

	o2->GetTransform().SetRotation(Transform::RandomRotation());
	o2->GetTransform().SetScale(Vector3f{ 1.f, 5.f, 5.f });
	o2->GetTransform().SetPosition(Vector3f{ 4.f, 5.f, 6.f });

	//auto globalTransform2 = o2->GetGlobalTransform();

	auto globalTransform3 = o3->GetGlobalTransform();
	o3->SetGlobalTransform(globalTransform3);

	auto globalTransform3New = o3->GetGlobalTransform();

	o3->SetGlobalPosition(globalTransform3New.GetPosition());
	o3->SetGlobalRotation(globalTransform3New.GetRotation());
	o3->SetGlobalScale(globalTransform3New.GetScale());

	auto globalTransform3New1 = o3->GetGlobalTransform();*/

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

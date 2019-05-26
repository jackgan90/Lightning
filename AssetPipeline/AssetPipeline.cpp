#include <iostream>
#include "Converters/AssimpConverter.h"
#include "SpaceObject.h"

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
}

int main(int argc, char** argv)
{
	auto o1 = std::make_shared<SpaceObject>();
	auto o2 = std::make_shared<SpaceObject>();
	auto o3 = std::make_shared<SpaceObject>();
	auto o4 = std::make_shared<SpaceObject>();
	
	o2->SetParent(o1);
	o3->SetParent(o2);
	o4->SetParent(o3);

	o1->GetTransform().SetRotation(Transform::RandomRotation());
	o1->GetTransform().SetScale(Vector3f{ 1.0f, 2.0f, 3.0f });
	//o1->GetTransform().SetPosition(Vector3f{ 1.0f, 1.0f, 1.0f });

	o2->GetTransform().SetRotation(Transform::RandomRotation());
	o2->GetTransform().SetScale(Vector3f{ .5f, 3.f, .5f });
	//o2->GetTransform().SetPosition(Vector3f{ -.5f, -2.f, 3.f });

	o3->GetTransform().SetRotation(Transform::RandomRotation());

	//auto matrix = GetMatrix(o4);

	//auto pos = Vector4f{ 0.f, 0.f, 0.f, 1.f } *matrix;

	auto localTransform3 = o3->GetTransform();
	auto globalTransform3 = o3->GetGlobalTransform();
	auto v0 = o3->GetTransform().GetRotation() * Vector3f { 1.0f, 0.0f, 0.0f };
	auto normalizedV0 = v0.Normalized();
	//auto globalPosition3 = globalTransform3.GetPosition();
	//globalPosition3.x += 10;
	//globalPosition3.y += 10;
	//globalPosition3.z += 10;
	//globalTransform3.SetPosition(globalPosition3);
	o3->SetGlobalTransform(globalTransform3);
	auto v1 = o3->GetTransform().GetRotation() * Vector3f { 1.0f, 0.0f, 0.0f };
	auto normalizedV1 = v1.Normalized();

	auto globalTransform3New = o3->GetGlobalTransform();

	o3->SetGlobalPosition(globalTransform3New.GetPosition());
	o3->SetGlobalRotation(globalTransform3New.GetRotation());
	o3->SetGlobalScale(globalTransform3New.GetScale());

	auto globalTransform3New1 = o3->GetGlobalTransform();

	auto v2 = o3->GetTransform().GetRotation() * Vector3f { 1.0f, 0.0f, 0.0f };
	auto normalizedV2 = v2.Normalized();

	auto r = v2.Dot(v0);

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

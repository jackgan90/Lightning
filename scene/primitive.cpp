#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include "math/vector.h"
#include "primitive.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::DegreesToRadians;
		Primitive::Primitive():mFirstDraw(true)
		{
		}

		Primitive::~Primitive()
		{

		}

		void Primitive::Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData)
		{
			if (mFirstDraw)
			{
				mFirstDraw = false;
				UpdateRenderItem();
			}
			if (sceneRenderData.camera)
			{
				mRenderItem.viewMatrix = sceneRenderData.camera->GetViewMatrix();
				mRenderItem.projectionMatrix = sceneRenderData.camera->GetProjectionMatrix();
			}
			renderer.Draw(mRenderItem);
		}

		void Primitive::UpdateRenderItem()
		{
			Render::VertexComponent comp;
			comp.format = Render::RenderFormat::R32G32B32_FLOAT;
			comp.instanceStepRate = 1;
			comp.isInstance = false;
			comp.offset = 0;
			comp.semanticIndex = 0;
			comp.semanticItem = { Render::RenderSemantics::POSITION, "POSITION" };
			std::vector<Render::VertexComponent> comps;
			comps.push_back(comp);
			mRenderItem.geometry = std::make_shared<Render::Geometry>();
			auto pDevice = Renderer::Instance()->GetDevice();
			auto vbSize = GetVertexBufferSize();
			auto ibSize = GetIndexBufferSize();
			mRenderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(vbSize, comps);
			mRenderItem.geometry->ib = pDevice->CreateIndexBuffer(ibSize, Render::IndexType::UINT16);
			
			
			auto vertices = GetVertices();

			auto indices = GetIndices();

			mRenderItem.geometry->vbs[0]->SetBuffer(vertices, vbSize);
			mRenderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(indices), ibSize);
			std::fill(std::begin(mRenderItem.geometry->vbs_dirty), std::end(mRenderItem.geometry->vbs_dirty), false);
			mRenderItem.geometry->vbs_dirty[0] = true;
			mRenderItem.geometry->ib_dirty = true;
			mRenderItem.geometry->primType = Render::PrimitiveType::TRIANGLE_LIST;
			
			mRenderItem.material = std::make_shared<Render::Material>();
			mRenderItem.material->RequireSemantic(Render::RenderSemantics::WVP);
			auto device = Renderer::Instance()->GetDevice();
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::VERTEX));
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::FRAGMENT));
			mRenderItem.transform = Render::Transform(Render::Vector3f({ 0.0f, 0.0f, 0.0f }), 
				GetScale());
		}

		
		//Cube
		Cube::CubeDataSource::CubeDataSource()
		{
			auto pVecs = new Vector3f[8];
			pVecs[0] = { 0.5f, 0.5f, 0.5f };	//right top front v0
			pVecs[1] = { 0.5f, 0.5f, -0.5f };	//right top back v1
			pVecs[2] = { 0.5f, -0.5f, 0.5f };	//right bottom front v2
			pVecs[3] = { 0.5f, -0.5f, -0.5f };	//right bottom back v3
			pVecs[4] = { -0.5f, 0.5f, 0.5f };	//left top front v4
			pVecs[5] = { -0.5f, 0.5f, -0.5f };	//left top back v5
			pVecs[6] = { -0.5f, -0.5f, 0.5f };	//left bottom front v6
			pVecs[7] = { -0.5f, -0.5f, -0.5f };	//left bottom back v7
			vertices = reinterpret_cast<float*>(pVecs);
			std::uint16_t index_data[] = {
				//right face
				0, 2, 3, 0, 3, 1,
				//front face
				4, 6, 2, 4, 2, 0,
				//left face
				5, 7, 6, 5, 6, 4,
				//back face
				1, 3, 7, 1, 7, 5,
				//top face
				5, 4, 0, 5, 0, 1,
				//bottom face
				6, 7, 3, 6, 3, 2
			};
			indices = new std::uint16_t[36];
			std::memcpy(indices, index_data, 36 * sizeof(std::uint16_t));
		}

		Cube::CubeDataSource Cube::sPrototype;
		Cube::Cube(float width, float height, float thickness) : 
			mWidth(width), mHeight(height), mThickness(thickness)
		{
			assert(mWidth > 0 && mHeight > 0 && mThickness > 0 && "The size of the cube must be greater than 0!");
		}

		Cube::~Cube()
		{
		}
		//Cube

		//Cylinder
		Cylinder::CylinderDataSource::CylinderDataSource()
		{
			static const auto CirclePointCount = 360 / AngularUnit;
			Vector3f *positions = new Vector3f[VertexCount];
			indices = new std::uint16_t[IndexCount];
			std::memset(indices, 0, sizeof(std::uint16_t) * IndexCount);
			
			Vector3f *topCenter = &positions[0];
			topCenter->x = 0;
			topCenter->y = static_cast<float>(1.0 / 2.0);
			topCenter->z = 0;

			Vector3f *bottomCenter = &positions[VertexCount - 1];
			bottomCenter->x = 0;
			bottomCenter->y = static_cast<float>(-1.0 / 2.0);
			bottomCenter->z = 0;
			int j = 0;
			
			for (int i = 0;i < CirclePointCount;++i)
			{
				//upper circle
				auto *vt = &positions[i + 1];
				auto radians = DegreesToRadians(i * AngularUnit);
				*vt = *topCenter + 0.5f * (Vector3f::right() * std::cos(radians) + Vector3f::back() * std::sin(radians));

				auto *vb = &positions[i + 1 + CirclePointCount];
				*vb = *vt + Vector3f::down() * 1.0f;

				auto isLast = i == CirclePointCount - 1;
				
				//top triangle
				indices[j++] = 0;
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2);
				else
					indices[j++] = static_cast<std::uint16_t>(1);
					

				//bottom triangle
				indices[j++] = static_cast<std::uint16_t>(VertexCount - 1);
				indices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				if (!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);

				//side triangle 0
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
				{
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
					indices[j++] = static_cast<std::uint16_t>(i + 2);
				}
				else
				{
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);
					indices[j++] = static_cast<std::uint16_t>(1);
				}

				//side triangle 1
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				indices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				if(!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);
			}
			vertices = reinterpret_cast<float*>(positions);
		}

		Cylinder::CylinderDataSource Cylinder::sPrototype;
		Cylinder::Cylinder(float height, float radius) :mHeight(height), mRadius(radius)
		{
			assert(height > 0 && radius > 0 && "height and radius of a cylinder must be positive!");
		}

		Cylinder::~Cylinder()
		{
		}
		//Cylinder

	}
}
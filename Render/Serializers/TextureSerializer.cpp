#include "TextureSerializer.h"
#include "FreeImage.h"
#include "Logger.h"
#include "Renderer.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		class FreeImageBuffer : public ISerializeBuffer
		{
		public:
			FreeImageBuffer(const std::shared_ptr<ISerializeBuffer>& rawBuffer, const std::string& path) 
				:mRawBuffer(rawBuffer), mBitmap(nullptr), mPath(path)
			{
				assert(rawBuffer && "raw buffer must not be nullptr!");
			}

			void GetTextureDescriptor(TextureDescriptor& descriptor)
			{
				descriptor.format = RenderFormat::R8G8B8A8_UNORM;
				descriptor.arraySize = 1;
				descriptor.depth = 1;
				descriptor.height = FreeImage_GetHeight(mBitmap);
				descriptor.multiSampleCount = 1;
				descriptor.multiSampleQuality = 1;
				descriptor.numberOfMipmaps = 1;
				descriptor.dimension = TEXTURE_DIMENSION_2D;
				descriptor.width = FreeImage_GetWidth(mBitmap);
			}

			bool Init()
			{
				assert(mBitmap == nullptr && "Already init!");

				FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
				BYTE* data = reinterpret_cast<BYTE*>(mRawBuffer->GetBuffer());
				auto bufferSize = mRawBuffer->GetBufferSize();
				auto stream = FreeImage_OpenMemory(data, (DWORD)bufferSize);
				if (!stream)
					return false;
				fif = FreeImage_GetFileTypeFromMemory(stream, 0);
				if (fif == FIF_UNKNOWN)
					fif = FreeImage_GetFIFFromFilename(mPath.c_str());
				if (fif == FIF_UNKNOWN)
				{
					FreeImage_CloseMemory(stream);
					return false;
				}
				auto bitmap = FreeImage_LoadFromMemory(fif, stream);
				if (!bitmap)
					return false;
				FreeImage_CloseMemory(stream);
				//If target format is specified,caller already knows the format of this texture
				//if the real texture format doesn't match the specified format,a conversion must be performed.
				auto bpp = FreeImage_GetBPP(bitmap);
				{
					//TODO : optimize performance
					if (bpp == 32)		//already 32 bit,ignore conversion
					{
						mBitmap = bitmap;
					}
					else
					{
						mBitmap = FreeImage_ConvertTo32Bits(bitmap);
						FreeImage_Unload(bitmap);
					}
				}
				return true;
			}

			~FreeImageBuffer()override
			{
				if (mBitmap)
				{
					FreeImage_Unload(mBitmap);
				}
			}

			char* GetBuffer()override
			{
				return reinterpret_cast<char*>(FreeImage_GetBits(mBitmap));
			}

			std::size_t GetBufferSize()const override
			{
				return FreeImage_GetMemorySize(mBitmap);
			}
		private:
			std::shared_ptr<ISerializeBuffer> mRawBuffer;
			FIBITMAP* mBitmap;
			std::string mPath;
		};

		TextureSerializer::TextureSerializer(const std::string path, ResourceAsyncCallback<ITexture> callback):
			mPath(path), mFinishCallback(callback), mTexture(nullptr)
		{

		}

		TextureSerializer::~TextureSerializer()
		{
			if (mFinishCallback)
			{
				mFinishCallback(mTexture);
			}
			if (mTexture)
				mTexture->Release();
		}


		void TextureSerializer::Serialize(char** buffer)
		{
			
		}

		void TextureSerializer::Deserialize(Foundation::IFile* file, const std::shared_ptr<ISerializeBuffer>& buffer)
		{
			auto fiBuffer = std::make_shared<FreeImageBuffer>(buffer, file->GetPath());
			if (fiBuffer->Init())
			{
				TextureDescriptor descriptor;
				fiBuffer->GetTextureDescriptor(descriptor);
				auto device = Renderer::Instance()->GetDevice();
				mTexture = device->CreateTexture(descriptor, fiBuffer);
			}
		}
	}
}
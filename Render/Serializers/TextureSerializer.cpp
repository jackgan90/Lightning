#include "TextureSerializer.h"
#include "FreeImage.h"
#include "Logger.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		using Loading::ISerializeBuffer;
		class FreeImageBuffer : public ISerializeBuffer
		{
		public:
			FreeImageBuffer(ISerializeBuffer* rawBuffer, const std::string& path) 
				:mRawBuffer(rawBuffer), mBitmap(nullptr), mPath(path)
			{
				assert(rawBuffer && "raw buffer must not be nullptr!");
				mRawBuffer->AddRef();
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
				descriptor.type = TEXTURE_TYPE_2D;
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

			~FreeImageBuffer()
			{
				if (mBitmap)
				{
					FreeImage_Unload(mBitmap);
				}
				if (mRawBuffer)
				{
					mRawBuffer->Release();
				}
			}

			char* GetBuffer()override
			{
				return reinterpret_cast<char*>(FreeImage_GetBits(mBitmap));
			}

			std::size_t GetBufferSize()const
			{
				return FreeImage_GetMemorySize(mBitmap);
			}
		private:
			ISerializeBuffer* mRawBuffer;
			FIBITMAP* mBitmap;
			std::string mPath;
		};

		TextureSerializer::TextureSerializer(const std::string path, ITextureLoadCallback* callback):
			mPath(path), mFinishCallback(callback)
		{

		}


		void TextureSerializer::Serialize(char** buffer)
		{
			
		}

		void TextureSerializer::Deserialize(Foundation::IFile* file, ISerializeBuffer* buffer)
		{
			auto fiBuffer = NEW_REF_OBJ(FreeImageBuffer, buffer, file->GetPath().c_str());
			if (fiBuffer->Init())
			{
				TextureDescriptor descriptor;
				fiBuffer->GetTextureDescriptor(descriptor);
				auto device = Renderer::Instance()->GetDevice();
				auto texture = device->CreateTexture(descriptor, fiBuffer);
				if (mFinishCallback)
				{
					mFinishCallback->operator()(texture);
				}
				fiBuffer->Release();
			}
			else
			{
				if (mFinishCallback)
				{
					mFinishCallback->operator()(nullptr);
				}
			}
		}

		void TextureSerializer::Dispose()
		{
			delete this;
		}
	}
}
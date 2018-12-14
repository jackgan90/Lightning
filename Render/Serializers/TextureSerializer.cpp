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
			FreeImageBuffer(ISerializeBuffer* rawBuffer, const std::string& path, RenderFormat targetFormat)
				:mRawBuffer(rawBuffer), mBitmap(nullptr), mPath(path), mTargetFormat(targetFormat)
			{
				assert(rawBuffer && "raw buffer must not be nullptr!");
				mRawBuffer->AddRef();
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
				if (mTargetFormat != RenderFormat::UNDEFINED)
				{
					//TODO : convert to target format
					mBitmap = bitmap;
				}
				else
				{
					//TODO : optimize performance
					if (bpp == 32)		//already 32 bit,ignore conversion
					{
						mBitmap = bitmap;
					}
					else
					{
						mBitmap = FreeImage_ConvertTo32Bits(bitmap);
						if (mBitmap)
						{
							FreeImage_Unload(bitmap);
						}
					}
					mTargetFormat = RenderFormat::R8G8B8A8_UNORM;
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

			FIBITMAP* GetBitmap() { return mBitmap; }

			RenderFormat GetRenderFormat()
			{
				return mTargetFormat;
			}

			std::uint16_t GetWidth()
			{
				return FreeImage_GetWidth(mBitmap);
			}

			std::uint16_t GetHeight()
			{
				return FreeImage_GetHeight(mBitmap);
			}
		private:
			ISerializeBuffer* mRawBuffer;
			FIBITMAP* mBitmap;
			std::string mPath;
			RenderFormat mTargetFormat;
		};

		TextureSerializer::TextureSerializer(const TextureDescriptor& descriptor, const std::string path,
			TextureLoadFinishHandler finishHandler):
			mDescriptor(descriptor), mPath(path), mFinishHandler(finishHandler)
		{

		}


		void TextureSerializer::Serialize(char** buffer)
		{
			
		}

		void TextureSerializer::Deserialize(Foundation::IFile* file, ISerializeBuffer* buffer)
		{
			auto fiBuffer = NEW_REF_OBJ(FreeImageBuffer, buffer, file->GetPath().c_str(), mDescriptor.format);
			if (fiBuffer->Init())
			{
				mDescriptor.width = fiBuffer->GetWidth();
				mDescriptor.height = fiBuffer->GetHeight();
				mDescriptor.format = fiBuffer->GetRenderFormat();
				auto device = Renderer::Instance()->GetDevice();
				auto texture = device->CreateTexture(mDescriptor, fiBuffer);
				if (mFinishHandler)
				{
					mFinishHandler(texture);
				}
				fiBuffer->Release();
			}
			else
			{
				if (mFinishHandler)
				{
					mFinishHandler(nullptr);
				}
			}
		}

		void TextureSerializer::Dispose()
		{
			delete this;
		}
	}
}
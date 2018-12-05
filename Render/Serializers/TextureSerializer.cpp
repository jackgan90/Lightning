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
				:mRawBuffer(rawBuffer), mBitmap(nullptr), mStream(nullptr), mPath(path)
			{
				assert(rawBuffer && "raw buffer must not be nullptr!");
				mRawBuffer->AddRef();
			}

			bool Init()
			{
				assert(mStream == nullptr && "Already init!");
				assert(mBitmap == nullptr && "Already init!");

				FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
				BYTE* data = reinterpret_cast<BYTE*>(mRawBuffer->GetBuffer());
				auto bufferSize = mRawBuffer->GetBufferSize();
				mStream = FreeImage_OpenMemory(data, (DWORD)bufferSize);
				if (!mStream)
					return false;
				fif = FreeImage_GetFileTypeFromMemory(mStream, 0);
				if (fif == FIF_UNKNOWN)
					fif = FreeImage_GetFIFFromFilename(mPath.c_str());
				if (fif == FIF_UNKNOWN)
				{
					FreeImage_CloseMemory(mStream);
					mStream = nullptr;
					return false;
				}
				mBitmap = FreeImage_LoadFromMemory(fif, mStream);
				if (mBitmap)
				{
					return true;
				}
				return false;
			}

			~FreeImageBuffer()
			{
				if (mBitmap)
				{
					FreeImage_Unload(mBitmap);
				}
				if (mStream)
				{
					FreeImage_CloseMemory(mStream);
				}
				if (mRawBuffer)
				{
					mRawBuffer->Release();
				}
			}

			char* GetBuffer()override
			{
				return reinterpret_cast<char*>(mBitmap->data);
			}

			std::size_t GetBufferSize()const
			{
				return 0;
			}
		private:
			ISerializeBuffer* mRawBuffer;
			FIBITMAP* mBitmap;
			FIMEMORY* mStream;
			std::string mPath;
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
			auto fiBuffer = NEW_REF_OBJ(FreeImageBuffer, buffer, file->GetPath().c_str());
			if (fiBuffer->Init())
			{
				auto device = Renderer::Instance()->GetDevice();
				auto texture = device->CreateTexture(mDescriptor, fiBuffer);
				if (mFinishHandler)
				{
					mFinishHandler(texture);
				}
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
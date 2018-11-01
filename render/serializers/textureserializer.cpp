#include "textureserializer.h"
#include "FreeImage.h"
#include "logger.h"

namespace Lightning
{
	namespace Render
	{
		TextureSerializer::TextureSerializer(const TextureDescriptor& descriptor, const std::string path,
			TextureLoadFinishHandler finishHandler):
			mDescriptor(descriptor), mPath(path), mFinishHandler(finishHandler)
		{

		}

		void TextureSerializer::Serialize(char** buffer)
		{

		}

		void TextureSerializer::Deserialize(const Foundation::SharedFilePtr& file, char* buffer)
		{
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
			BYTE* data = reinterpret_cast<BYTE*>(buffer);
			auto bufferSize = file->GetSize();
			auto stream = FreeImage_OpenMemory(data, bufferSize);
			fif = FreeImage_GetFileTypeFromMemory(stream, 0);
			if (fif == FIF_UNKNOWN)
				fif = FreeImage_GetFIFFromFilename(file->GetPath().c_str());
			static const SharedTexturePtr EmptyTexturePtr;
			if (fif == FIF_UNKNOWN)
			{
				if (mFinishHandler)
				{
					mFinishHandler(EmptyTexturePtr);
				}
				FreeImage_CloseMemory(stream);
				return;
			}
			FIBITMAP* dib = FreeImage_LoadFromMemory(fif, stream);
			if (dib)
			{
				FreeImage_Unload(dib);
			}
			else
			{

			}
			FreeImage_CloseMemory(stream);
		}

		void TextureSerializer::Dispose()
		{
			delete this;
		}
	}
}
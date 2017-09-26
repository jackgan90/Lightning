#include "iapplication.h"


namespace LightningGE
{
	namespace App
	{
		bool Application::Start()
		{
			bool result = true;
			result &= InitRenderContext();
			return result;
		}
	}
}
#pragma once
#include "IEnvironment.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Foundation
	{
		class Environment : public IEnvironment, public Singleton<Environment>
		{
		public:
			void INTERFACECALL InitLoaderIOThreadID()override;
			bool INTERFACECALL IsInLoaderIOThread()const override;
		private:
			friend class Singleton<Environment>;
			std::thread::id mLoaderIOThreadID;
		};
	}
}
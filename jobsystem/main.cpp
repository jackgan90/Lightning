#include<cstdlib>
#include <iostream>
#include "jobsystem.h"

int main(int argc, char** argv)
{
	JobSystem::JobSystem::Instance().Run();
	return 0;
}
#include "ms_utilities.h"

namespace ModelStudio
{

	utilities::utilities(void)
	{
	}

	utilities::~utilities(void)
	{
	}

	void utilities::setStartTime()
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);
	}

	void utilities::setEndTime()
	{
		QueryPerformanceCounter(&end);
	}

	double utilities::getDeltaTime()
	{
		return (double)(end.QuadPart-start.QuadPart)/(double)freq.QuadPart;
	}
};
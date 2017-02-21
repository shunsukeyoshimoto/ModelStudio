#pragma once
#include <windows.h>


namespace ModelStudio
{
	class utilities
	{
	private:
		LARGE_INTEGER freq;
		LARGE_INTEGER start;
		LARGE_INTEGER end;
	public:
		utilities(void);
		~utilities(void);
		void setStartTime();
		void setEndTime();
		double getDeltaTime();
	};
};
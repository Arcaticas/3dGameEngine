#include "Sound.h"


eae6320::cResult eae6320::Sound::Play(const char* stuff)
{
	auto result = Results::Success;

	size_t newsize = strlen(stuff) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, stuff, _TRUNCATE);

	PlaySound(wcstring, NULL, SND_FILENAME | SND_ASYNC);

	return result;
}

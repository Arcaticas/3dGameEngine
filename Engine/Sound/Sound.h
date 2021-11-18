
#ifndef EAE6320_SOUND_H
#define EAE6320_SOUND_H

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ole32.lib")
#include <Engine/Results/Results.h>
#include <Windows.h>

#include <Audioclient.h>
#include <mmdeviceapi.h>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

namespace eae6320
{
	namespace Sound
	{
		//Depreciated 
		DWORD Load(UINT32 audioFrames, UINT32 position, const char* wavPath, WAVEFORMATEX* i_wavFormat, unsigned char*& o_buffer);
		cResult SetWavFormat(const char* i_wavPath, WAVEFORMATEX* o_wavFormat);
		cResult PlayAudioStream(const char* wavPath);
		//

		cResult Play(const char* wavPath);
		cResult PlayLoop(const char* wavPath);
		cResult StopAllSound();
	}
}
#endif

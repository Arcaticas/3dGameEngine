#pragma warning( disable : 4244 ) 
#include "Sound.h"
#include <Engine/Logging/Logging.h>
#include <fstream>
#include <Engine/Platform/Platform.h>



const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);


DWORD eae6320::Sound::Load(UINT32 audioFrames, UINT32 position, const char* i_wavPath, WAVEFORMATEX* i_wavFormat, unsigned char*& o_buffer)
{
	

	eae6320::Platform::sDataFromFile binaryData;
	eae6320::Platform::LoadBinaryFile(i_wavPath, binaryData);

	auto currentOffset = reinterpret_cast<uintptr_t>(binaryData.data);
	const auto finalOffset = currentOffset + binaryData.size;


	

	//Takes you to the size of the formating chuck
	currentOffset += 16;
	UINT32 formatSize = *reinterpret_cast<UINT32*>(currentOffset);
	//Move past the formatting data
	currentOffset += 4;
	currentOffset += formatSize;
	//Check for FILE header chunk
	if (*reinterpret_cast<UINT32*>(currentOffset) == 1414744396)
	{
		//Move to size of list
		currentOffset += 4;
		UINT32 listSize = *reinterpret_cast<UINT32*>(currentOffset);
		//Move past size
		currentOffset += 4;
		//Move past the list chunk
		currentOffset += listSize;
	}

	//Check for DATA header chunk
	if (*reinterpret_cast<UINT32*>(currentOffset) == 1635017060)
	{
		//Move past the DATA header and data size
		currentOffset += 8;
		//You are now pointing to the top of the sound data
	}
	else
	{
		return Results::Failure;
	}
	UINT32 audioFrameSize = i_wavFormat->nChannels * i_wavFormat->wBitsPerSample;

	currentOffset += position * audioFrameSize;


	if (finalOffset< currentOffset)
	{
		return AUDCLNT_BUFFERFLAGS_SILENT;
	}

	unsigned char* ret = new unsigned char[audioFrames * audioFrameSize];

	memcpy(ret, (void*)currentOffset, audioFrames);

	o_buffer = ret;
		

	return 0;
}

eae6320::cResult eae6320::Sound::SetWavFormat(const char* i_wavPath, WAVEFORMATEX* o_wavFormat)
{
	auto result = Results::Success;

	eae6320::Platform::sDataFromFile binaryData;
	eae6320::Platform::LoadBinaryFile(i_wavPath, binaryData);

	auto currentOffset = reinterpret_cast<uintptr_t>(binaryData.data);

	currentOffset += 20;
	o_wavFormat->wFormatTag = *reinterpret_cast<WORD*>(currentOffset);
	currentOffset += 2;
	o_wavFormat->nChannels = *reinterpret_cast<WORD*>(currentOffset);
	currentOffset += 2;
	o_wavFormat->nSamplesPerSec = *reinterpret_cast<DWORD*>(currentOffset);
	currentOffset += 10;
	o_wavFormat->wBitsPerSample = *reinterpret_cast<WORD*>(currentOffset);
	o_wavFormat->nBlockAlign = (o_wavFormat->nChannels * o_wavFormat->wBitsPerSample) / 8;
	o_wavFormat->nAvgBytesPerSec = o_wavFormat->nSamplesPerSec * o_wavFormat->nBlockAlign;
	o_wavFormat->cbSize = 0;

	return result;
}

eae6320::cResult eae6320::Sound::PlayAudioStream(const char* i_wavPath)
{
	auto result = Results::Success;

	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioRenderClient* pRenderClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	UINT32 bufferFrameCount = 0;
	UINT32 numFramesAvailable = 0;
	UINT32 numFramesPadding = 0;
	unsigned char* pData;
	DWORD flags = 0;

	//Get a reference to the IMMDeviceEnumerator interface
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	//Get a reference to the default audio output
	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	// Get the audio client
	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	//Retrieves the stream format that the audio engine uses
	hr = pAudioClient->GetMixFormat(&pwfx);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	//Creates audio stream in the audio client
	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	if (hr != S_OK)
	{
		return Results::Failure;
	}



	//Set WAVEFORMATEX parameters - basically, your digital audio processing settings
	eae6320::Sound::SetWavFormat(i_wavPath, pwfx);

	//Retrieves the size (maximum capacity) of the endpoint buffer of the audio client and assigns it to bufferFrameCount
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	//Gets a service - in this case the IID_IAudioRenderClient - which enables a client to write output data to a rendering endpoint buffer i.e. play sound
	hr = pAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)&pRenderClient);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	// Grab the entire buffer for the initial fill operation and sends it to the render client
	hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	//Keeps track of how far into the file we have read
	UINT32 currentOffset = 0;

	// Load the initial data into the shared buffer
	flags = eae6320::Sound::Load(bufferFrameCount, currentOffset, i_wavPath, pwfx, pData);
	currentOffset += bufferFrameCount;

	hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	// Calculate the actual duration of the allocated buffer
	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start playing

	while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
	{
		// Sleep for half the buffer duration.
		Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

		// See how much buffer space is available.
		hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
		

		numFramesAvailable = bufferFrameCount - numFramesPadding;

		// Grab all the available space in the shared buffer.
		hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
		

		// Get next 1/2-second of data from the audio source.
		flags = eae6320::Sound::Load(numFramesAvailable, currentOffset, i_wavPath, pwfx, pData);
		currentOffset += numFramesAvailable;

		hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
		
		if (hr != S_OK)
		{
			return Results::Failure;
		}
	}
	//Wait for things to finish
	Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

	//Stop playing
	hr = pAudioClient->Stop();
	if (hr != S_OK)
	{
		return Results::Failure;
	}

	return result;
}

















eae6320::cResult eae6320::Sound::Play(const char* i_wavPath)
{
	auto result = Results::Success;

	size_t newsize = strlen(i_wavPath) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, i_wavPath, _TRUNCATE);

	PlaySound(wcstring, NULL, SND_FILENAME | SND_ASYNC);

	return result;
}

eae6320::cResult eae6320::Sound::PlaySync(const char* i_wavPath)
{
	auto result = Results::Success;

	size_t newsize = strlen(i_wavPath) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, i_wavPath, _TRUNCATE);

	PlaySound(wcstring, NULL, SND_FILENAME);
	return result;
}

eae6320::cResult eae6320::Sound::PlayLoop(const char* i_wavPath)
{
	auto result = Results::Success;

	size_t newsize = strlen(i_wavPath) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, i_wavPath, _TRUNCATE);

	PlaySound(wcstring, NULL, SND_LOOP | SND_ASYNC);

	return result;
}

eae6320::cResult eae6320::Sound::StopAllSound()
{
	auto result = Results::Success;

	PlaySound(NULL, 0, 0);

	return result;
}

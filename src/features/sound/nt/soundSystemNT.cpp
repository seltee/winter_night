#include "features/sound/nt/soundSystemNT.h"
#include "features/sound/nt/soundHelperNT.h"
#include "features/logger/logger.h"
#include "core/platform.h"

using namespace wne;

#if defined(OS_WINDOWS)

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#define FREQ (44100)
#define FREQ_PART (FREQ / 10)
#define CHANNELS_AMOUNT 2
#define BUFFER_SIZE (FREQ_PART * 2 * sizeof(float))

BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext);

SoundSystemNT::SoundSystemNT()
{
}

bool SoundSystemNT::setup(void *hWnd)
{
    LPCGUID lpGuid = nullptr;
    /*
    if (deviceName.length() > 0)
    {
        auto list = getWindowsDevicesList();
        for (auto &item : list)
        {
            if (item.description == deviceName)
            {
                lpGuid = item.lpGuid;
                break;
            }
        }
    }
    */

    HRESULT result = DirectSoundCreate8(lpGuid, &lpds, NULL);
    if (result == DS_OK)
    {
        lpds->SetCooperativeLevel((HWND)hWnd, DSSCL_PRIORITY);

        DSBUFFERDESC dsbd;
        ZeroMemory(&dsbd, sizeof(dsbd));
        dsbd.dwSize = sizeof(dsbd);
        dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
        LPDIRECTSOUNDBUFFER pPrimaryBuffer;
        lpds->CreateSoundBuffer(&dsbd, &pPrimaryBuffer, NULL);

        bool result = setupPinPongBuffers();
        if (result)
        {
            setupBuffer(FREQ_PART);
            return result;
        }
    }
    else
    {
        Logger::log << getAudioError("Audio setup failed", result).c_str() << endl;
    }
    return false;
}

std::shared_ptr<SoundSystemNT> SoundSystemNT::create(void *hWnd)
{
    auto soundSystem = std::make_shared<SoundSystemNT>();
    soundSystem->setup(hWnd);
    return soundSystem;
}

void SoundSystemNT::updateBuffers()
{
    unsigned long playPos;
    pDXBuffer->GetCurrentPosition(&playPos, NULL);

    if (bFirstPartPlaying && playPos > BUFFER_SIZE)
    {
        bFirstPartPlaying = false;
        writeOffset = 0;
        fillBuffer();
    }

    if (!bFirstPartPlaying && playPos < BUFFER_SIZE)
    {
        bFirstPartPlaying = true;
        writeOffset = BUFFER_SIZE;
        fillBuffer();
    }
}

void SoundSystemNT::provideBuffer(float *data, uint sampleCount)
{
    void *pLockedBuffer = NULL;
    unsigned long lockedBufferSize = 0;
    HRESULT hr = pDXBuffer->Lock(writeOffset, sampleCount * sizeof(float) * 2, &pLockedBuffer, &lockedBufferSize, NULL, NULL, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(pLockedBuffer, data, lockedBufferSize);
        pDXBuffer->Unlock(pLockedBuffer, lockedBufferSize, NULL, 0);
    }
}

/*
std::vector<AudioDeviceWindows> SoundSystemNT::getWindowsDevicesList()
{
    std::vector<AudioDeviceWindows> list;
    DirectSoundEnumerateW(DSEnumCallback, &list);
    return list;
}
*/

bool SoundSystemNT::setupPinPongBuffers()
{
    // Initialize WAVEFORMATEX structure
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = CHANNELS_AMOUNT;
    wfx.nSamplesPerSec = FREQ;
    wfx.wBitsPerSample = sizeof(float) * 8;
    wfx.nBlockAlign = sizeof(float) * CHANNELS_AMOUNT;
    wfx.nAvgBytesPerSec = FREQ * sizeof(float) * CHANNELS_AMOUNT;
    wfx.cbSize = 0;

    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = BUFFER_SIZE * 2;
    dsbd.lpwfxFormat = &wfx;

    HRESULT result = lpds->CreateSoundBuffer(&dsbd, &pDXBuffer, NULL);
    if (result != DS_OK)
    {
        Logger::log << getAudioError("Unable to initialize sound ", result).c_str() << endl;
        return false;
    }

    result = pDXBuffer->Play(0, 0, DSBPLAY_LOOPING);
    if (result != DS_OK)
    {
        Logger::log << getAudioError("Failed to loop buffer ", result).c_str() << endl;
        return false;
    }

    return true;
}

BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext)
{
    std::vector<AudioDeviceWindows> *list = (std::vector<AudioDeviceWindows> *)lpContext;
    list->push_back({convertToString(lpcstrDescription), lpGuid});
    return true;
}

#endif

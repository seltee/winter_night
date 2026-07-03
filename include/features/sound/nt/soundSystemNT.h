#pragma once
#include <memory>
#include <vector>
#include "features/sound/soundSystem.h"
#include "core/core.h"

#if defined(OS_WINDOWS)
#include <windows.h>
#include <dsound.h>
namespace wne
{
    class WNE_API SoundSystemNT : public SoundSystem
    {
    public:
        SoundSystemNT();
        bool setup(void *hWnd);
        static std::shared_ptr<SoundSystemNT> create(void *hWnd);

    protected:
        void setupBuffer(uint sampleCount);
        void updateBuffers() override final;
        void provideBuffer(float *data, uint sampleCount) override final;
        // std::vector<AudioDeviceWindows> getWindowsDevicesList();
        bool setupPinPongBuffers();

        LPDIRECTSOUND8 lpds = nullptr;
        IDirectSoundBuffer *pDXBuffer = nullptr;

        bool bFirstPartPlaying = true;
        uint writeOffset = 0;

        std::vector<float> buffer;
        uint sampleCount = 0;
    };
}

#endif
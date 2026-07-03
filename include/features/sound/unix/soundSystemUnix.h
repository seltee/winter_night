#pragma once
#include "features/sound/soundSystem.h"
#include "core/core.h"
#include <memory>
#include <vector>

#if defined(OS_LINUX)
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/param/props.h>
#include <spa/pod/builder.h>
#include <spa/utils/result.h>

namespace wne
{
    class WNE_API SoundSystemUnix : public SoundSystem
    {
    public:
        SoundSystemUnix();
        ~SoundSystemUnix();
        bool setup();
        static std::shared_ptr<SoundSystemUnix> create();

        void fillNextBuffer(float *buffer, uint32 samplesCount);

        inline pw_stream *getStream()
        {
            return stream_;
        }

    protected:
        void updateBuffers() override final;

        static void onProcess(void *userdata);

        pw_main_loop *loop_ = nullptr;
        pw_context *context_ = nullptr;
        pw_core *core_ = nullptr;
        pw_stream *stream_ = nullptr;

        std::thread audioThread;

        std::vector<uint8> buffer;
    };
}

#endif
#include "features/sound/unix/soundSystemUnix.h"
#include "features/logger/logger.h"
#include <cmath>

using namespace wne;

#define FREQ (44100)
#define FREQ_PART (FREQ / 15)
#define CHANNELS_AMOUNT 2
#define BUFFER_SIZE (FREQ_PART * 2 * sizeof(float))

SoundSystemUnix::SoundSystemUnix()
{
}

SoundSystemUnix::~SoundSystemUnix()
{
    if (core_)
        pw_core_disconnect(core_);
    if (context_)
        pw_context_destroy(context_);
    if (loop_)
    {
        pw_main_loop_quit(loop_);
        pw_main_loop_destroy(loop_);
    }
    audioThread.join();
}

bool SoundSystemUnix::setup()
{
    pw_init(nullptr, nullptr);

    loop_ = pw_main_loop_new(nullptr);
    if (!loop_)
    {
        Logger::log << "Unable to create PipeWire loop" << endl;
        return false;
    }

    context_ = pw_context_new(pw_main_loop_get_loop(loop_), nullptr, 0);
    if (!context_)
    {
        Logger::log << "Unable to create PipeWire context" << endl;
        return false;
    }

    core_ = pw_context_connect(context_, nullptr, 0);
    if (!core_)
    {
        Logger::log << "Unable to create PipeWire core" << endl;
        return false;
    }

    static const pw_stream_events StreamEvents =
        {
            .version = PW_VERSION_STREAM_EVENTS,
            .process = onProcess};

    buffer.resize(BUFFER_SIZE);
    uint8 *bufferData = buffer.data();
    uint32 bufferSize = buffer.size();

    spa_pod_builder builder = SPA_POD_BUILDER_INIT(bufferData, bufferSize);

    const spa_pod *params;

    auto paramsInitData = SPA_AUDIO_INFO_RAW_INIT(
            .format = SPA_AUDIO_FORMAT_F32_LE,
            .rate = FREQ,
            .channels = 2);

    params = spa_format_audio_raw_build(
        &builder,
        SPA_PARAM_EnumFormat,
        &paramsInitData);

    stream_ =
        pw_stream_new_simple(
            pw_main_loop_get_loop(loop_),
            "Audio",
            pw_properties_new(
                PW_KEY_MEDIA_TYPE, "Audio",
                PW_KEY_MEDIA_CATEGORY, "Playback",
                PW_KEY_MEDIA_ROLE, "Game",
                nullptr),
            &StreamEvents,
            this);

    auto result = pw_stream_connect(
        stream_,
        PW_DIRECTION_OUTPUT,
        PW_ID_ANY,
        (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT |
                          PW_STREAM_FLAG_MAP_BUFFERS |
                          PW_STREAM_FLAG_RT_PROCESS),
        &params,
        1);

    if (result != 0)
    {
        Logger::log << "Unable to initialize PipeWire, error " << result << endl;
        return false;
    }

    audioThread = std::thread([this]
                              { pw_main_loop_run(loop_); });
    Logger::log << "Sound Wire initialization is done" << endl;
    return true;
}

std::shared_ptr<SoundSystemUnix> SoundSystemUnix::create()
{
    auto soundSystem = std::make_shared<SoundSystemUnix>();
    soundSystem->setup();
    return soundSystem;
}

void SoundSystemUnix::fillNextBuffer(float *buffer, uint32 samplesCount)
{
    fillBuffer(buffer, samplesCount);
}

void SoundSystemUnix::updateBuffers()
{
}

void SoundSystemUnix::onProcess(void *userdata)
{
    SoundSystemUnix *soundSystem = static_cast<SoundSystemUnix *>(userdata);
    pw_stream *stream = soundSystem->getStream();

    pw_buffer *b = pw_stream_dequeue_buffer(stream);
    if (!b)
        return;

    spa_buffer *buf = b->buffer;

    float *data = static_cast<float *>(buf->datas[0].data);

    const uint32 maxSamples = buf->datas[0].maxsize / (sizeof(float) * 2);
    const uint32 samplesCount = std::min(
        static_cast<uint32>(FREQ_PART),
        maxSamples);

    soundSystem->fillNextBuffer(data, samplesCount);

    spa_data *d = &buf->datas[0];
    d->chunk->offset = 0;
    d->chunk->stride = sizeof(float) * 2;
    d->chunk->size = samplesCount * sizeof(float) * 2;

    pw_stream_queue_buffer(stream, b);
}

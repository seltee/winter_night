#include "features/sound/soundSource.h"
#include "features/sound/sound.h"
#include "features/logger/logger.h"
#include <cmath>

using namespace wne;

SoundSource::SoundSource(Sound &sound)
{
    this->sound = &sound;
    this->flagIsPlaying = true;
    soundSampleShift = 0;

    if (sound.isStreaming())
    {
        flagIsStreamed = true;
        streamData = sound.createStreamData();
    }
}

SoundSource::SoundSource(Sound &sound, bool isLooping)
{
    this->sound = &sound;
    this->flagIsPlaying = true;
    this->flagIsLooping = isLooping;
    soundSampleShift = 0;

    if (sound.isStreaming())
    {
        flagIsStreamed = true;
        streamData = sound.createStreamData();
    }
}

SoundSource::~SoundSource()
{
    if (streamData)
    {
        sound->destroyStreamData(streamData);
    }
}

void SoundSource::addToBuffer(float *buffer, uint amountOfSamples)
{
    if (!flagIsPlaying)
        return;

    if (sound->isStreaming())
        addToBufferStream(buffer, amountOfSamples);
    else
        addToBufferData(buffer, amountOfSamples);
}

void SoundSource::addToBufferStream(float *buffer, uint amountOfSamples)
{
    if (!streamData)
    {
        flagIsPlaying = false;
        return;
    }

    bool isStereo = !flagIs3d;
    streamReader.resize(amountOfSamples * (isStereo ? 2 : 1));
    uint samplesRead = sound->fillStreamData(streamData, streamReader.data(), amountOfSamples, isStereo);
    uint floatsToPlay = samplesRead * 2;

    float volL = 1.0f;
    float volR = 1.0f;

    if (flagIs3d)
    {
        Vector3 direction = position - listener->getPosition();
        Vector3 dirNormal = normalize(direction);
        float dist2 = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;

        if (dist2 > maxDistance2)
            return;
        float distVolume = 1.0f - ((maxDistance2 == 0) ? 0.0f : dist2 / maxDistance2);

        Vector3 right = listener->getRightVector();
        float rightValue = -dot(right, dirNormal) * 0.75f;

        if (rightValue > 0)
            volR -= rightValue;
        else
            volL += rightValue;

        volR = std::max(volR * distVolume, 0.0f);
        volL = std::max(volL * distVolume, 0.0f);

        for (uint i = 0; i < floatsToPlay; i += 2)
        {
            float val = streamReader[i >> 1];
            buffer[i] += val * volL;
            buffer[i + 1] += val * volR;
        }
    }
    else
    {
        for (uint i = 0; i < floatsToPlay; i += 2)
        {
            buffer[i] += streamReader[i] * volL;
            buffer[i + 1] += streamReader[i + 1] * volR;
        }
    }

    if (samplesRead == 0)
    {
        if (flagIsLooping)
        {
            sound->destroyStreamData(streamData);
            streamData = sound->createStreamData();
        }
        else
        {
            flagIsPlaying = 0;
        }
    }
}

void SoundSource::addToBufferData(float *buffer, uint amountOfSamples)
{
    uint samplesToPlay = std::min(amountOfSamples, sound->getSampleCount() - soundSampleShift);
    uint samplesToPlay2 = samplesToPlay * 2;
    float *data = sound->getData();

    float volL = 1.0f;
    float volR = 1.0f;

    if (flagIs3d)
    {
        Vector3 direction = position - listener->getPosition();
        Vector3 dirNormal = normalize(direction);
        float dist2 = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;

        if (dist2 > maxDistance2)
            return;
        float distVolume = 1.0f - ((maxDistance2 == 0) ? 0.0f : dist2 / maxDistance2);

        Vector3 right = listener->getRightVector();
        float rightValue = -dot(right, dirNormal) * 0.75f;

        if (rightValue > 0)
            volR -= rightValue;
        else
            volL += rightValue;

        volR = std::max(volR * distVolume, 0.0f);
        volL = std::max(volL * distVolume, 0.0f);
    }

    if (sound->isStereo())
    {
        uint soundSampleShift2 = soundSampleShift * 2;
        for (uint i = 0; i < samplesToPlay2; i += 2)
        {
            buffer[i] += data[i + soundSampleShift2] * volL;
            buffer[i + 1] += data[i + soundSampleShift2 + 1] * volR;
        }
    }
    else
    {
        for (uint i = 0; i < samplesToPlay2; i += 2)
        {
            float val = data[(i >> 1) + soundSampleShift];
            buffer[i] += val * volL;
            buffer[i + 1] += val * volR;
        }
    }

    soundSampleShift += samplesToPlay;
    if (soundSampleShift >= sound->getSampleCount())
    {
        soundSampleShift = 0;
        if (flagIsLooping)
        {
            // todo should start filling buffer with repeating content
            // addToBuffer(float *buffer, uint amountOfSamples)
        }
        else
        {
            flagIsPlaying = false;
        }
    }
}

void SoundSource::turn3d(std::shared_ptr<Positionable> listener, const Vector3 &position, float maxDistance)
{
    this->flagIs3d = true;
    this->listener = std::move(listener);
    this->position = position;
    this->maxDistance = maxDistance;
    this->maxDistance2 = maxDistance * maxDistance;
}
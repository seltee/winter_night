#include "features/sound/soundSystem.h"
#include "features/logger/logger.h"

using namespace wne;

std::shared_ptr<SoundSystem> SoundSystem::create()
{
    return std::make_shared<SoundSystem>();
}

std::shared_ptr<SoundSource> SoundSystem::playSound(Sound *sound, bool loop)
{
    auto source = std::make_shared<SoundSource>(sound, loop);
    sources.push_back(source);
    return source;
}

std::shared_ptr<SoundSource> SoundSystem::playSound3d(Sound *sound, const Vector3 source, std::shared_ptr<Positionable> listener, bool loop)
{
    return nullptr;
}

std::shared_ptr<Sound> SoundSystem::loadSound(const char *path)
{
    return std::make_shared<Sound>(this, path, true, false);
}

std::shared_ptr<Sound> SoundSystem::loadMusic(const char *path)
{
    return std::make_shared<Sound>(this, path, true, true);
}

void SoundSystem::update()
{
    /*
    auto it = sources.begin();
        while (it != sources.end())
        {
            (*it)->process(delta);
            if ((*it)->isDestroyed())
            {
                delete (*it);
                it = sources.erase(it);
            }
            else
                ++it;
        }
        */

    updateBuffers();
}

void SoundSystem::updateBuffers()
{
}

void SoundSystem::provideBuffer(float *data, uint sampleCount)
{
}

void SoundSystem::setupBuffer(uint sampleCount)
{
    this->sampleCount = sampleCount;
    buffer.resize(sampleCount * 2);
}

void SoundSystem::fillBuffer()
{
    float *data = buffer.data();

    memset(data, 0, sampleCount * sizeof(float) * 2);

    for (auto &it : sources)
    {
        if (it->isPlaying())
        {
            if (it->is3d())
            {
            }
            else
            {
                it->addToBuffer(data, sampleCount);
            }
            /*
            if (it->is3dPositioned())
            {
                // Check if source is in sound range
                Vector3 localPosition = it->getPosition() - vPosition;
                float maxDistance = it->getMaxDistance();
                float distance = glm::length(localPosition);

                if (distance > maxDistance)
                    continue;

                // Calc left/right relations
                Vector3 vSide = glm::cross(vDirection, vUpDirection);

                float sideProjection = glm::dot(vSide, glm::normalize(localPosition));
                float mainValue = 1.0f - fabsf(sideProjection);
                float lVolume, rVolume;
                if (sideProjection < 0.0f)
                {
                    lVolume = mainValue;
                    rVolume = mainValue + fabsf(sideProjection);
                }
                else
                {
                    lVolume = mainValue + fabsf(sideProjection);
                    rVolume = mainValue;
                }

                // Volume based on distance
                float distanceVolume = 1.0f;
                float refDistance = it->getReferenceDistance();

                if (distance > refDistance)
                {
                    distanceVolume = 1.0f - (distance - refDistance) / (maxDistance - refDistance);
                }
                lVolume *= distanceVolume;
                rVolume *= distanceVolume;

                it->fillBuffer(pShiftBuffer, FREQ_PART);
                for (int i = 0; i < BUFFER_SIZE / 4; i += 2)
                {
                    pfDataBuffer[i] += pfShiftBuffer[i] * lVolume;
                    pfDataBuffer[i + 1] += pfShiftBuffer[i + 1] * rVolume;
                }
            }
            else
            {
                it->fillBuffer(pShiftBuffer, FREQ_PART);
                for (int i = 0; i < BUFFER_SIZE / 4; i += 2)
                {
                    pfDataBuffer[i] += pfShiftBuffer[i];
                    pfDataBuffer[i + 1] += pfShiftBuffer[i + 1];
                }
            }
            */
        }
    }

    // Limiter
    float fullSize = sampleCount * 2;
    for (uint i = 0; i < fullSize; i++)
    {
        data[i] = fminf(fmaxf(data[i], -1.0f), 1.0f);
    }

    provideBuffer(data, sampleCount);
}
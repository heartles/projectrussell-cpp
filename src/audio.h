#pragma once
#ifndef SRC_AUDIO_H
#define SRC_AUDIO_H

#include <mutex>
#include <array>
#include <memory>

#include <portaudio.h>
#include <sndfile.h>

#include "pool.h"
#include "common.h"

class Sound
{
	std::vector<float> _samples;
	int _channels;
	int _sampleRate;
	friend class AudioManager;

public:
	inline Sound(std::vector<float> &&samples, int channels, int sampleRate)
		: _samples(samples), _channels(channels), _sampleRate(sampleRate) {}

	inline Sound() {}
};

STRONG_TYPEDEF(ActiveSoundHandle, uint64_t)
STRONG_TYPEDEF_EQUALITY(ActiveSoundHandle, uint64_t)


class ActiveSound
{
	friend class AudioManager;
	ActiveSoundHandle _id;
	const Sound *_sound;

	int _readHead = 0;
public:

	inline ActiveSound(const Sound *snd, ActiveSoundHandle id)
		: _sound(snd), _id(id) {}

	ActiveSound() = default;
	ActiveSound(const ActiveSound &) = default;
	ActiveSound(ActiveSound &&) = default;

	ActiveSound &operator=(const ActiveSound &other) = default;
	ActiveSound &operator=(ActiveSound &&) = default;
	
	inline const struct Sound *Playing() const
	{
		return _sound;
	}

	inline ActiveSoundHandle ID() const
	{
		return _id;
	}
	
	float Volume = 1.0f;
	bool Active = true;
	bool Loop = false;
};

struct SoundRenderResult
{
	int SamplesRendered;
	int WriteHeadAdvanced;
};

template<typename T, size_t N>
class RingBuffer
{
	using vec_t = std::vector<T>;
	vec_t _buf = vec_t(N);

public:
	const size_t Size = N;
	
	T &operator[](int i)
	{
		return _buf[i % Size];
	}

	T operator[](int i) const
	{
		return _buf[i % Size];
	}

	typename vec_t::iterator begin()
	{
		return _buf.begin();
	}
	
	typename vec_t::iterator end()
	{
		return _buf.end();
	}

	typename vec_t::const_iterator begin() const
	{
		return _buf.begin();
	}

	typename vec_t::const_iterator end() const
	{
		return _buf.end();
	}

	size_t size() const 
	{
		return Size;
	}
};

class AudioManager
{	
	PaStream *_stream;
	std::mutex _lock{};
	std::vector<ActiveSound> _playingSounds;

	int outputSamplesInternal(void *output, uint64_t count);
	static const size_t bufSize = 44100 * 2;
	RingBuffer<float, bufSize> _buf{};
	mutable int _readHead = 0;
	int _writeHead = 0;

	mutable uint64_t _nextSoundID = 0;

	static const int framesPerSecond = 44100;
	static const int framesPerGfxFrame = framesPerSecond / 60;
	static const int channels = 2;
	static const int samplesPerSecond = framesPerSecond * channels;
	static const int samplesPerGfxFrame = framesPerGfxFrame * channels;

	static const int minWriteAheadSamples = samplesPerGfxFrame;
	static const int maxWriteAheadSamples = samplesPerGfxFrame * 4;
	
	std::vector<float> _tmpBuf = std::vector<float>(maxWriteAheadSamples);


public:
	class DEBUG_Renderer;
	AudioManager();
	~AudioManager();

	// The ActiveSoundHandle is only valid as long as the sound is
	// still playing. Once the sound stops playing, it is removed from the
	// pool. Attempts to access the ActiveSound through its handle after
	// it is removed from the pool 
	ActiveSoundHandle PlaySound(const Sound *);
	
	// Renders sound into the buffer.
	// Only to be called by engine code.
	SoundRenderResult RenderSound(float dt);

	// These pointers may be invalidated at the end of every update call,
	// and the validity of these pointers are undefined during the Render
	// call.
	// TODO: If a sound's active flag is not set, it will not be deallocated 
	// because the sound has not finished playing. Should we clean house every
	// level?
	ActiveSound *Get(ActiveSoundHandle);
};

#endif // SRC_AUDIO_H

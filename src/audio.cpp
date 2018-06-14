#include "audio.h"

#include <exception>

#include "common.h"
#include "math.h"

AudioManager::AudioManager()
{
	auto err = Pa_Initialize();
	if (err != paNoError)
		throw std::runtime_error{ Pa_GetErrorText(err) };

	PaStreamParameters params;
	params.channelCount = 2;
	params.device = Pa_GetDefaultOutputDevice();
	
	// handle default device not available
	if (params.device == paNoDevice) {
		int count = Pa_GetDeviceCount();
	}

	assert(params.device != paNoDevice);
	params.hostApiSpecificStreamInfo = nullptr;
	params.sampleFormat = paFloat32;
	const PaDeviceInfo *inf = Pa_GetDeviceInfo(params.device);
	assert(inf);
	params.suggestedLatency = inf->defaultLowOutputLatency;
	Log("Running with Latency " + std::to_string(params.suggestedLatency) + 
		" at sample rate " + std::to_string(inf->defaultSampleRate));

	err = Pa_OpenStream(&_stream,
		nullptr, 
		&params, 
		44100, // sample rate
		paFramesPerBufferUnspecified, // frames per buffer
		0,
		[](const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		-> int {
			auto manager = static_cast<AudioManager *>(userData);
			//timeInfo->
			return manager->outputSamplesInternal(output, frameCount);
		},
		this);
	if (err != paNoError)
		throw std::runtime_error{ Pa_GetErrorText(err) };

	err = Pa_StartStream(_stream);
	if (err != paNoError)
		throw std::runtime_error{ Pa_GetErrorText(err) };
}

AudioManager::~AudioManager()
{
	auto err = Pa_StopStream(_stream);
	if (err != paNoError)
		Log(Pa_GetErrorText(err));

	err = Pa_CloseStream(_stream);
	if (err != paNoError)
		Log(Pa_GetErrorText(err));

	err = Pa_Terminate();
	if (err != paNoError)
		Log(Pa_GetErrorText(err));
}

int
AudioManager::outputSamplesInternal(void *o, uint64_t count)
{
	float *output = static_cast<float *>(o);

	for (int i = 0; i < count * channels; i++) {
		*output++ = _buf[_readHead];
		_buf[_readHead++] = 0;
	}

	return paContinue;
}

SoundRenderResult
AudioManager::RenderSound(float dt)
{
	int samplesToWrite = static_cast<int>(_tmpBuf.size());
	assert(_tmpBuf.size() <= INT32_MAX);
		
	auto minTarget = _readHead + minWriteAheadSamples;
	auto maxTarget = _readHead + maxWriteAheadSamples;
	auto target = std::clamp(_writeHead + int(samplesPerSecond * dt) + 20, minTarget, maxTarget);

	auto advance = target - _writeHead;

	for (int u = 0; u < _playingSounds.size(); u++) {
		auto &sound = _playingSounds[u];
		int write = 0;
		auto &samples = sound._sound->_samples;
		int read = sound._readHead;

		if (sound.Active)
		for (int i = 0; i < samplesToWrite; i++) {
			_tmpBuf[write++] += samples[read++] * sound.Volume;

			if (read >= samples.size()) {
				if (sound.Loop)
					read = 0;
				else {
					sound.Active = false;
					sound._readHead = 0;
					_playingSounds.erase(_playingSounds.begin() + u);
					u--;
					goto nextSound;
				}
			}
		}

		sound._readHead += advance;
		if (sound._readHead > samples.size())
			sound._readHead = 0;

	nextSound:;
	}

	for (int i = 0; i < samplesToWrite; i++) {
		_buf[_writeHead + i] = _tmpBuf[i];
		_tmpBuf[i] = 0;
	}

	_writeHead += advance;

	return { samplesToWrite, advance };
}

ActiveSound * AudioManager::Get(ActiveSoundHandle id)
{
	for (auto &v : _playingSounds) {
		if (v._id == id)
			return &v;
	}

	return nullptr;
}

ActiveSoundHandle
AudioManager::PlaySound(const Sound *snd)
{
	assert(snd);
	auto s = ActiveSound{
		snd,
		ActiveSoundHandle(_nextSoundID++)
	};

	_playingSounds.push_back(s);
	return s._id;
}

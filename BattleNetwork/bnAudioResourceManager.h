#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include "bnAudioType.h"

// For more authentic retro experience, decrease available channels.
#define NUM_OF_CHANNELS 3

class AudioResourceManager
{
public:
	static AudioResourceManager& GetInstance();

	void LoadAllSources();
	int Play(AudioType type);
	int Stream(std::string path, bool loop = false);

	AudioResourceManager();
	~AudioResourceManager();

private:
	sf::Sound* channels;
	sf::SoundBuffer* sources;
	sf::Music stream;
};


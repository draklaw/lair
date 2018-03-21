/*
 *  Copyright (C) 2015, 2016 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <cstdlib>
#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>

#include <lair/sys_sdl2/sys_module.h>

#include "lair/sys_sdl2/audio_module.h"


#define LAIR_AM_MAX_CHANNELS    32
#define LAIR_AM_DEFAULT_VOLUME  (.5)


namespace lair {


Sound::Sound()
    : _chunk(nullptr),
      _volume(LAIR_AM_DEFAULT_VOLUME) {
}


Sound::Sound(Mix_Chunk* chunk)
    : _chunk(chunk),
      _volume(LAIR_AM_DEFAULT_VOLUME) {
	Mix_VolumeChunk(_chunk, _volume * MIX_MAX_VOLUME);
}


Sound::Sound(Sound&& other)
    : _chunk(other._chunk),
      _volume(other._volume) {
	other._chunk = nullptr;
}


Sound::~Sound() {
	if(_chunk) {
		Mix_FreeChunk(_chunk);
	}
}


Sound& Sound::operator=(Sound&& other) {
	swap(other);
	return *this;
}


void Sound::set(Mix_Chunk* chunk) {
	if(_chunk) {
		Mix_FreeChunk(_chunk);
	}

	_chunk = chunk;
	Mix_VolumeChunk(_chunk, _volume * MIX_MAX_VOLUME);
}


void Sound::swap(Sound& other) {
	std::swap(_chunk,  other._chunk);
	std::swap(_volume, other._volume);
}


void Sound::setVolume(float volume) {
	_volume = volume;
	Mix_VolumeChunk(_chunk, _volume * MIX_MAX_VOLUME);
}

//---------------------------------------------------------------------------//


Music::Music()
	: _track(nullptr) {
}


Music::Music(Mix_Music *track)
	: _track(track) {
}


Music::Music(Music&& other)
	: _track(other._track) {
	other._track = nullptr;
}


Music::~Music() {
	if(_track) {
		Mix_FreeMusic(_track);
	}
}


Music& Music::operator=(Music&& other) {
	swap(other);
	return *this;
}


void Music::set(Mix_Music* music) {
	if(_track) {
		Mix_FreeMusic(_track);
	}

	_track = music;
}


void Music::swap(Music& other) {
	std::swap(_track, other._track);
}

//---------------------------------------------------------------------------//


AudioModule::AudioModule(MasterLogger* logger, LogLevel level)
    : _logger("audio_sdl2", logger, level),
      _initialized(false) {
}


void AudioModule::initialize() {
	log().log("Initialize SDL_mixer...");

	SDL_InitSubSystem(SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_OGG);

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)) {
		log().error("Failed to initialize SDL_mixer backend");
	}
	Mix_AllocateChannels(LAIR_AM_MAX_CHANNELS);

	_initialized = true;
}


void AudioModule::shutdown() {
	Mix_Quit();

	_initialized = false;
}


int AudioModule::playSound(AssetSP sound, int loops, int channel) {
	if(!sound)
		return -1;

	auto aspect = sound->aspect<SoundAspect>();
	if(!aspect)
		return -1;

	aspect->warnIfInvalid(log());
	if(!aspect->isValid())
		return -1;

	return playSound(aspect->get(), loops, channel);
}


int AudioModule::playSound(const Sound& sound, int loops, int channel) {
	if(sound.isValid()) {
		return Mix_PlayChannel(channel, sound.chunk(), loops);
	}
	return -1;
}


void AudioModule::stopSound(int channel) {
	if(channel != -1) {
		Mix_HaltChannel(channel);
	}
}


void AudioModule::playMusic(AssetSP music) {
	if(!music)
		return;

	auto aspect = music->aspect<MusicAspect>();
	if(!aspect)
		return;

	aspect->warnIfInvalid(log());
	if(!aspect->isValid())
		return;

	playMusic(aspect->get());
}


void AudioModule::playMusic(const Music& music) {
	if(music.isValid()) {
		Mix_PlayMusic(music.track(), -1);
	}
}


void AudioModule::stopMusic() {
	Mix_HaltMusic();
}


void AudioModule::setMusicVolume(float volume) {
	Mix_VolumeMusic(volume * MIX_MAX_VOLUME);
}

//---------------------------------------------------------------------------//


SoundLoader::SoundLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void SoundLoader::commit() {
	SoundAspectSP aspect = std::static_pointer_cast<SoundAspect>(_aspect);
	aspect->_get() = std::move(_sound);
	Loader::commit();
}


void SoundLoader::loadSyncImpl(Logger& log) {
	SDL_RWops* rw = sdlRwFromFile(file());
	if(!rw) {
		log.error("Failed to load sound \"", asset()->logicPath(), "\": Invalid file");
		return;
	}

	Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, true);
	if(chunk) {
		_sound.set(chunk);
	}
	else {
		log.error("Failed to load sound \"", asset()->logicPath(), "\": ", Mix_GetError());
	}
}

//---------------------------------------------------------------------------//


MusicLoader::MusicLoader(LoaderManager* manager, AspectSP aspect)
    : Loader(manager, aspect) {
}


void MusicLoader::commit() {
	MusicAspectSP aspect = std::static_pointer_cast<MusicAspect>(_aspect);
	aspect->_get() = std::move(_music);
	Loader::commit();
}


void MusicLoader::loadSyncImpl(Logger& log) {
	SDL_RWops* rw = sdlRwFromFile(file());
	if(!rw) {
		log.error("Failed to load music \"", asset()->logicPath(), "\": Invalid file");
		return;
	}
	Mix_Music* track = Mix_LoadMUS_RW(rw, true);
	if(track) {
		_music.set(track);
	}
	else {
		log.error("Failed to load music \"", asset()->logicPath(), "\": ", Mix_GetError());
	}
}


}

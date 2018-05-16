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


#ifndef _LAIR_SYS_SDL2_AUDIO_MODULE_H_
#define _LAIR_SYS_SDL2_AUDIO_MODULE_H_

#include <string>
#include <unordered_map>

#include <SDL_mixer.h>

#include <lair/core/lair.h>
#include <lair/core/path.h>

#include <lair/asset/loader.h>

#include <lair/asset/asset_manager.h>


namespace lair {


class Sound {
public:
	Sound();
	Sound(Mix_Chunk* chunk);
	Sound(const Sound&)  = delete;
	Sound(      Sound&& other);
	~Sound();

	Sound& operator=(const Sound&)  = delete;
	Sound& operator=(      Sound&& other);

	void set(Mix_Chunk* chunk);
	void swap(Sound& other);

	inline bool isValid() const { return _chunk; }
	inline Mix_Chunk* chunk() const { return _chunk; }

	inline float volume() const { return _volume; }
	void setVolume(float volume);

private:
	Mix_Chunk* _chunk;
	float      _volume;
};

typedef std::shared_ptr<Sound> SoundSP;
typedef std::weak_ptr  <Sound> SoundWP;


class Music {
public:
	Music();
	Music(Mix_Music* track);
	Music(const Music&)  = delete;
	Music(      Music&& other);
	~Music();

	Music& operator=(const Music&)  = delete;
	Music& operator=(      Music&& other);

	void set(Mix_Music* music);
	void swap(Music& other);

	inline bool isValid() const { return _track; }
	inline Mix_Music* track() const { return _track; }

public:
	Mix_Music* _track;
};

typedef std::shared_ptr<Music> MusicSP;
typedef std::weak_ptr  <Music> MusicWP;


class AudioModule {
public:
	AudioModule(MasterLogger* logger = nullptr, LogLevel level = LogLevel::Log);

	void initialize();
	void shutdown();

	int  playSound(AssetSP sound, int loops = 0, int channel = -1);
	int  playSound(const Sound& sound, int loops = 0, int channel = -1);
	void stopSound(int channel);

	void playMusic(AssetSP music);
	void playMusic(const Music& music);
	void stopMusic();
	void setMusicVolume(float volume);

	inline Logger& log() { return _logger; }

private:
	Logger   _logger;
	bool     _initialized;
};


typedef GenericAspect<Sound>         SoundAspect;
typedef std::shared_ptr<SoundAspect> SoundAspectSP;
typedef std::weak_ptr  <SoundAspect> SoundAspectWP;

typedef GenericAspect<Music>         MusicAspect;
typedef std::shared_ptr<MusicAspect> MusicAspectSP;
typedef std::weak_ptr  <MusicAspect> MusicAspectWP;


class SoundLoader : public Loader {
public:
	typedef SoundAspect Aspect;

public:
	SoundLoader(LoaderManager* manager, AspectSP aspect);
	virtual ~SoundLoader() = default;

	virtual void commit();

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Sound _sound;
};

class MusicLoader : public Loader {
public:
	typedef MusicAspect Aspect;

	virtual void commit();

public:
	MusicLoader(LoaderManager* manager, AspectSP aspect);
	virtual ~MusicLoader() = default;

protected:
	virtual void loadSyncImpl(Logger& log);

protected:
	Music _music;
};


}

#endif


#include <cstdlib>
#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>

#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>



class AudioManager {
    public:
        std::vector<std::string> FMODResultsList;

        AudioManager() {
            checkFMODException(FMOD::System_Create(&system), "initFMOD(): didn\'t create audio system correctly!\n");
            checkFMODException(system->init(100, FMOD_INIT_NORMAL, 0), "initFMOD(): didn\'t initialize audio system correctly!\n");
            checkFMODException(system->getMasterChannelGroup(&master), "initFMOD(): didn\'t get the master channel group!\n");

            // create channel groups for each category
            for (int i; i < CATEGORY_COUNT; ++i) {
                system->createChannelGroup(0, &groups[i]);
                master->addGroup(groups[i]);
            }

            // setup modes for each category
            modes[CATEGORY_SFX] = FMOD_DEFAULT;
            modes[CATEGORY_SONG] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;

            // seed random number generator for setSFXs
            srand(time(0));
        }

        ~AudioManager() {
            // release sounds in each category
            SoundMap::iterator iter;
            for (int i = 0; i < CATEGORY_COUNT; ++i) {
                for (iter = sounds[i].begin(); iter != sounds[i].end(); ++iter) {
                    iter->second->release();
                }
                sounds[i].clear();
            }

            // release system
            system->release();
            system = nullptr;
        }

        void loadSong(const std::string& path) {
            load(CATEGORY_SONG, path);
        }

        void loadSFX(const std::string& path) {
            load(CATEGORY_SFX, path);
        }

        void update(float elapsed) {
            const float fadeTime = 1.0f; // in seconds
            if (currentSong != 0 && fade == FADE_IN) {
                float volume;
                currentSong->getVolume(&volume);
                float nextVolume = volume + elapsed / fadeTime;
                if (nextVolume >= 1.0f) {
                    currentSong->setVolume(1.0f);
                    fade = FADE_NONE;
                } else {
                    currentSong->setVolume(nextVolume);
                }
            } else if (currentSong != 0 && fade == FADE_OUT) {
                float volume;
                currentSong->getVolume(&volume);
                float nextVolume = volume - elapsed / fadeTime;
                if (nextVolume <= 0.0f) {
                    currentSong->stop();
                    currentSong = 0;
                    currentSongPath.clear();
                    fade = FADE_NONE;
                } else {
                    currentSong->setVolume(nextVolume);
                }
            } else if (currentSong == 0 && !nextSongPath.empty()) {
                playSong(nextSongPath);
                nextSongPath.clear();
            }
            system->update();
        }

        void togglePaused(FMOD::Channel* channel) {
            bool paused;
            channel->getPaused(&paused);
            channel->setPaused(!paused);
        }

        void playSong(const std::string& path) {;
            // ignore if this song is already is playing
            if (currentSongPath == path) return;

            // if a song is playing stop them and set this as the next song
            if (currentSong != 0) {
                stopSongs();
                nextSongPath = path;
                return;
            }

            // find the song in the corresponding sound map
            SoundMap::iterator sound = sounds[CATEGORY_SONG].find(path);
            if (sound == sounds[CATEGORY_SONG].end()) return;

            // start playing song with volume set to 0 and fade in
            currentSongPath = path;
            system->playSound(sound->second, 0, true, &currentSong);
            currentSong->setChannelGroup(groups[CATEGORY_SONG]);
            currentSong->setVolume(0.0f);
            currentSong->setPaused(false);
            fade = FADE_IN;
        }

        void playSFX(const std::string& path, float minVolume, float maxVolume, float minPitch, float maxPitch) {
            SoundMap::iterator sound = sounds[CATEGORY_SFX].find(path);

            if (sound == sounds[CATEGORY_SFX].end()) {
                FMODResultsList.push_back("playSFX(): couldn\'t find SFX " + path + "\n");
                return;
            }

            float volume = RandomBetween(minVolume, maxVolume);
            float pitch = RandomBetween(minPitch, maxPitch);

            // Play the sound effect with these initial values
            FMOD::Channel* channel;
            checkFMODException(
                system->playSound(sound->second, 0, true, &channel),
                "playSFX(): couldn\'t play sound effect!\n"
            );

            channel ->setChannelGroup(groups[CATEGORY_SFX]);
            channel->setVolume(volume);

            float frequency;
            channel->getFrequency(&frequency);
            channel->setFrequency(changeSemitone(frequency, pitch));
            channel->setPaused(false);
        }

        void stopSFXs() {
            groups[CATEGORY_SFX]->stop();
        }

        void stopSongs() {
            if (currentSong != 0) fade = FADE_OUT;
            nextSongPath.clear();
        }

        void setMasterVolume(float volume) {
            master->setVolume(volume);
        }

        void setSFXsVolume(float volume) {
            groups[CATEGORY_SFX]->setVolume(volume);
        }

        void setSongsVolume(float volume) {
            groups[CATEGORY_SONG]->setVolume(volume);
        }
    private:
        typedef std::map<std::string, FMOD::Sound*> SoundMap;
        enum Category { CATEGORY_SFX, CATEGORY_SONG, CATEGORY_COUNT };

        FMOD::System* system = 0;
        FMOD::ChannelGroup* master;
        FMOD::ChannelGroup* groups[CATEGORY_COUNT];
        SoundMap sounds[CATEGORY_COUNT];
        FMOD_MODE modes[CATEGORY_COUNT];

        FMOD::Channel* currentSong;
        std::string currentSongPath;
        std::string nextSongPath;

        enum FadeState { FADE_NONE, FADE_IN, FADE_OUT };
        FadeState fade;

        float musicVolume = 1.0f;
        float sfxVolume = 1.0f;
        float speechVolume = 1.0f;

        void load(Category type, const std::string& path) {
            if (sounds[type].find(path) != sounds[type].end()) return;

            FMOD::Sound* sound;
            system->createSound(path.c_str(), modes[type], 0, &sound);
            sounds[type].insert(std::make_pair(path, sound));
        }

        void checkFMODException(FMOD_RESULT result, std::string error_str) {
            if (result != FMOD_OK) {
                error_str.append(FMOD_ErrorString(result));
                FMODResultsList.push_back(error_str);
            };
        }

        float changeOctave(double frequency, double variation) {
            static float octave_ratio = 2.0f;
            return frequency * pow(octave_ratio, variation);
        }

        float changeSemitone(double frequency, double variation) {
            static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
            return frequency * pow(semitone_ratio, variation);
        }

        float RandomBetween(float min, float max) {
            if (min == max) return min;
            float n = (float)rand() / (float)RAND_MAX;
            return min + n * (max-min);
        }
};

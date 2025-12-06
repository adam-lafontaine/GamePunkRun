#pragma once

#include "../io/audio.hpp"
#include "../io/filesystem.hpp"
#include "../util/numeric.hpp"
#include "../alloc_type/alloc_type.hpp"

#include "sdl_include.hpp"

#include <SDL3_mixer/SDL_mixer.h>


#define ASSERT_AUDIO
#define LOG_AUDIO


#ifndef NDEBUG

#ifdef LOG_AUDIO
#define audio_log(...) SDL_Log(__VA_ARGS__)
#else
#define audio_log(...)
#endif

#ifdef ASSERT_AUDIO
#define audio_assert(condition) SDL_assert(condition)
#else
#define audio_assert(...)
#endif

#else

#define audio_log(...)
#define audio_assert(...)

#endif


/* helpers */

namespace audio
{
    namespace num = numeric;


    static bool has_extension(cstr filename, const char* ext)
    {
        auto file_length = span::strlen(filename);
        auto ext_length = span::strlen(ext);

        return !span::strcmp(&filename[file_length - ext_length], ext);
    }


    static bool is_valid_audio_file(cstr filename)
    {
        return 
            //has_extension(filename, ".mp3") || 
            //has_extension(filename, ".MP3") ||
            has_extension(filename, ".ogg") || 
            has_extension(filename, ".OGG") ||
            has_extension(filename, ".wav") ||
            has_extension(filename, ".WAV");
    }


    static f32 get_music_volume()
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;
        
        auto i_volume = Mix_VolumeMusic(-1);        

        return (f32)(i_volume - MIN) / (MAX - MIN);

        return 0.0f;
    }


    static f32 get_sound_channel_volume(int track_channel)
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        auto i_volume = Mix_Volume(track_channel, -1);

        return (f32)(i_volume - MIN) / (MAX - MIN);

        return 0.0f;
    }


    static f32 set_sound_channel_volume(int track_channel, f32 volume)
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        volume = num::clamp(volume, 0.0f, 1.0f);

        auto i_volume = num::round_to_signed<int>(volume * (MAX - MIN));
        if (i_volume != Mix_Volume(track_channel, -1))
        {
            Mix_Volume(track_channel, i_volume);
        }
        
        return get_sound_channel_volume(track_channel);
    }  


    static f32 set_sound_volume_all_channels(f32 volume)
    {
        return set_sound_channel_volume(-1, volume);
    }
}


/* internal */

namespace audio
{      
    using music_p = Mix_Music*;
    using sound_p = Mix_Chunk*;


    constexpr int MAX_SOUND_TRACKS = 16;


    static Sound* sound_tracks[MAX_SOUND_TRACKS] = { 0 };
    static int n_sound_tracks = 0;

    static Music* music_track = nullptr;
    static int n_music_tracks = 0;

    static bool audio_initialized = false;


    static void reset_music(Music& music)
    {
        music.id = -1;
        music.data_ = 0;
        music.is_on = 0;
        music.is_paused = 0;
    }


    static void reset_sound(Sound& sound)
    {
        sound.id = -1;
        sound.data_ = 0;
        sound.is_on = 0;
    }


    static void sound_finished_cb(int track_channel)
    {
        if (track_channel >= 0 && track_channel < MAX_SOUND_TRACKS && sound_tracks[track_channel])
        {
            sound_tracks[track_channel]->is_on = false;
        }
    }


    static void set_sound_id(Sound& sound, sound_p data)
    {
        audio_assert(data && " *** no sound data *** ");

        sound.data_ = (void*)data;
        sound.is_on = false;

        sound.id = n_sound_tracks++;
    }


    static void play_sound_track_once(Sound& sound)
    {
        audio_assert(sound.data_ && " *** no sound data *** ");
        
        constexpr int N_REPEATS = 0;

        auto track_channel = sound.id < 0 ? -1 : sound.id;

        track_channel = Mix_PlayChannel(track_channel, (sound_p)sound.data_, N_REPEATS);
        if (sound.id < 0)
        {
            sound.id = track_channel;
            sound_tracks[track_channel] = &sound;
        }
        
        sound.is_on = true;
    }


    static void play_sound_track_loop(Sound& sound)
    {
        audio_assert(sound.data_ && " *** no sound data *** ");

        constexpr int FOREVER = -1;

        auto track_channel = sound.id < 0 ? -1 : sound.id;

        track_channel = Mix_PlayChannel(track_channel, (sound_p)sound.data_, FOREVER);
        if (sound.id < 0)
        {
            sound.id = track_channel;
            sound_tracks[track_channel] = &sound;
        }
        
        sound.is_on = true;
    }


    static void set_music_id(Music& music, music_p data)
    {
        audio_assert(data && " *** no music data *** ");

        music.data_ = (void*)data;
        music.is_on = false;
        music.is_paused = false;

        music.id = n_music_tracks++;
    }


    static bool is_current_music_track(Music const& music)
    {
        return music_track && music_track->id == music.id;
    }


    static void stop_music_track()
    {
        if ((!music_track) || (!music_track->is_on))
        {
            return;
        }

        Mix_HaltMusic();
        music_track->is_on = false;
        music_track->is_paused = false;
    }


    static void play_music_track(Music& music)
    {
        audio_assert(music.data_ && " *** no music data *** ");

        constexpr int FOREVER = -1;

        if (Mix_PlayMusic((music_p)music.data_, FOREVER))
        {
            music.is_on = true;
            music.is_paused = false;
            
            music_track = &music;
        }
    }


    static void fade_out_music_track(u32 fade_ms)
    {
        if ((!music_track) || (!music_track->is_on))
        {
            return;
        }

        Mix_FadeOutMusic((int)fade_ms);
        music_track->is_on = false;
        music_track->is_paused = false;
    }


    static void fade_in_music_track(Music& music, u32 fade_ms)
    {
        audio_assert(music.data_ && " *** no music data *** ");

        constexpr int FOREVER = -1;

        if (Mix_FadeInMusic((music_p)music.data_, FOREVER, (int)fade_ms))
        {
            music.is_on = true;
            music.is_paused = false;
            
            music_track = &music;
        }
    }


    static bool is_initialized()
    {
        return audio_initialized;
    }
}


/* api */

namespace audio
{
    void destroy_music(Music& music)
    {
        if (music.data_)
        {
            mem::untag((u8*)music.data_);
            Mix_FreeMusic((music_p)music.data_);
        }

        if (is_current_music_track(music))
        {
            music_track = nullptr;
        }

        reset_music(music);
    }


    void destroy_sound(Sound& sound)
    {
        if (sound.data_)
        {
            mem::untag((u8*)sound.data_);
            Mix_FreeChunk((sound_p)sound.data_);
        }        

        reset_sound(sound);
    }


    bool init_audio()
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            sdl::print_error("Init Audio");
            return false;
        }

        Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);

        SDL_AudioDeviceID device_id = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;

        SDL_AudioSpec spec{};
        spec.channels = MIX_DEFAULT_CHANNELS,
        spec.format = MIX_DEFAULT_FORMAT,
        spec.freq = 44100;

        if (!Mix_OpenAudio(device_id, &spec))
        {
            sdl::print_error("Mix_OpenAudio()");
            return false;
        }

        Mix_ChannelFinished(sound_finished_cb);

        audio_initialized = true;
        //set_master_volume(0.5f);

        return true;
    }


    void stop_audio()
    {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
    }


    void close_audio()
    {
        stop_audio();
        Mix_CloseAudio();
        Mix_Quit();

        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        audio_initialized = false;
    }


    bool load_music_from_file(cstr music_file_path, Music& music)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");
        
        auto is_valid_file = is_valid_audio_file(music_file_path);
        audio_assert(is_valid_file && " *** invalid music file *** ");

        reset_music(music);

        if (!is_valid_file)
        {
            audio_log("Invalid music file: %s\n", music_file_path);
            return false;
        }

        auto size = fs::file_size(music_file_path);
        if (!size)
        {
            audio_log("Music file size zero: %s\n", music_file_path);
            return false;
        }

        auto data = Mix_LoadMUS(music_file_path);
        if (!data)
        {
            sdl::print_error("Load Music");
            return false;
        }

        mem::tag((u8*)data, size, fs::get_file_name(music_file_path));

        set_music_id(music, data);

        return true;
    }


    bool load_sound_from_file(cstr sound_file_path, Sound& sound)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        auto is_valid_file = is_valid_audio_file(sound_file_path);
        audio_assert(is_valid_file && " *** invalid music file *** ");

        reset_sound(sound);

        if (!is_valid_file)
        {
            audio_log("Invalid sound file: %s\n", sound_file_path);
            return false;
        }

        auto size = fs::file_size(sound_file_path);
        if (!size)
        {
            audio_log("Sound file size zero: %s\n", sound_file_path);
            return false;
        }

        sound_p data = Mix_LoadWAV(sound_file_path);        
        if (!data)
        {
            sdl::print_error("Load Sound");
            return false;
        }

        mem::tag((u8*)data, size, fs::get_file_name(sound_file_path));

        set_sound_id(sound, data);

        return true;
    }


    bool load_music_from_bytes(ByteView const& bytes, Music& music, cstr tag)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        audio_assert(bytes.data && " *** no bytes data *** ");
        audio_assert(bytes.length && " *** no bytes length *** ");

        // Does not copy data

        reset_music(music);

        if (!bytes.data || !bytes.length)
        {
            return false;
        }

        auto rw = SDL_IOFromConstMem((void*)bytes.data, (int)bytes.length);
        if (!rw)
        {
            sdl::print_error("SDL_IOFromConstMem()");
            return false;
        }

        auto data = Mix_LoadMUS_IO(rw, 1);
        if (!data)
        {
            sdl::print_error("SDL_IOFromConstMem()");
            return false;
        }

        mem::tag((u8*)data, bytes.length, tag);

        set_music_id(music, data);

        return true;
    }


    bool load_sound_from_bytes(ByteView const& bytes, Sound& sound, cstr tag)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        audio_assert(bytes.data && " *** no bytes data *** ");
        audio_assert(bytes.length && " *** no bytes length *** ");

        // Does not copy data

        reset_sound(sound);

        if (!bytes.data || !bytes.length)
        {
            return false;
        }

        auto rw = SDL_IOFromConstMem((void*)bytes.data, (int)bytes.length);
        if (!rw)
        {
            sdl::print_error("SDL_IOFromConstMem()");
            return false;
        }

        auto data = Mix_LoadWAV_IO(rw, 1);
        if (!data)
        {
            sdl::print_error("Mix_LoadWAV_IO()");
            return false;
        }

        mem::tag((u8*)data, bytes.length, tag);

        set_sound_id(sound, data);

        return true;
    }


    f32 set_music_volume(f32 volume)
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        audio_assert(is_initialized() && " *** audio not initialized *** ");

        volume = num::clamp(volume, 0.0f, 1.0f);

        auto i_volume = num::round_to_signed<int>(volume * (MAX - MIN));
        if (i_volume != Mix_VolumeMusic(-1))
        {
            Mix_VolumeMusic(i_volume);
        }
        
        return get_music_volume();
    }


    f32 set_sound_volume(f32 volume)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        return set_sound_volume_all_channels(volume);
    }


    f32 set_sound_volume(Sound& sound, f32 volume)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        return set_sound_channel_volume(sound.id, volume);
    }
    

    void play_music(Music& music)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        if (music.is_on)
        {
            return;
        }

        play_music_track(music);
    }


    void toggle_pause_music()
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");
        audio_assert(music_track && " *** music_track not set *** ");

        auto& music = *music_track;

        if (Mix_PausedMusic() == 1)
        {
            Mix_ResumeMusic();
            music.is_paused = false;
        }
        else
        {
            Mix_PauseMusic();
            music.is_paused = true;
        }
    }


    void stop_music()
    {
        stop_music_track();
    }


    void fade_in_music(Music& music, u32 fade_ms)
    {
        fade_in_music_track(music, fade_ms);
    }


    void fade_out_music(u32 fade_ms)
    {
        fade_out_music_track(fade_ms);
    }


    void play_sound(Sound& sound)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        play_sound_track_once(sound);
    }


    void play_sound_loop(Sound& sound)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");
        
        play_sound_track_loop(sound);
    }


    void stop_sound(Sound& sound)
    {
        audio_assert(is_initialized() && " *** audio not initialized *** ");

        if (!sound.is_on)
        {
            return;
        }

        Mix_HaltChannel(sound.id);
        sound.is_on = false;
    }


    void stop_sound()
    {
        Mix_HaltChannel(-1);
    }
   
}
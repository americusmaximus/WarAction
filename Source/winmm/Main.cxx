/*
Copyright (c) 2024 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Audio.hxx"
#include "State.hxx"
#include "Module.hxx"

#define WINMM_MODULE_PATH "\\WINMM.DLL"
#define MUSIC_PATH "\\MUSIC"
#define MAX_MODULE_PATH_SIZE (MAX_PATH * 2)

// NOTE: Replacement for the original static variable initialization.
VOID Initialize()
{
    AudioState.Volume = MAX_AUDIO_VOLUME;

    AudioState.Context.First = DEFAULT_TRACK_INDEX;
    AudioState.Context.Last = DEFAULT_TRACK_INDEX;

    State.Volume = DEFAULT_VOLUME;
    State.TimeFormat = MCI_FORMAT_TMSF;

    State.Tracks.Current = DEFAULT_TRACK_INDEX;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
    if (reason != DLL_PROCESS_ATTACH) { return TRUE; }

    Initialize();

    CHAR path[MAX_MODULE_PATH_SIZE];
    GetSystemDirectoryA(path, MAX_MODULE_PATH_SIZE);

    strcat_s(path, MAX_MODULE_PATH_SIZE, WINMM_MODULE_PATH);

    ModuleState.Module = LoadLibraryA(path);

    if (ModuleState.Module == NULL) { return FALSE; }

    ModuleState.TimeGetTime = (LPTIMEGETTIME)GetProcAddress(ModuleState.Module, METHOD_TIMEGETTIME_NAME);

    GetModuleFileNameA(instance, State.ModuleName, MAX_MODULE_NAME_LENGTH);

    ZeroMemory(&State.Tracks.Tracks, sizeof(SoundTrack) * MAX_SOUND_TRACK_COUNT);

    InitializeCriticalSection(&State.Mutex);

    InitializeAudio();

    {
        CHAR* last = strrchr(State.ModuleName, '\\');

        if (last) { *last = NULL; }

        strncat(State.ModuleName, MUSIC_PATH, MAX_MODULE_NAME_LENGTH - 1);
    }

    {
        UINT position = 0;

        for (INT x = 0; x < MAX_SOUND_TRACK_COUNT; x++)
        {
            snprintf(State.Tracks.Tracks[x].Name, MAX_PATH, "%s\\Track%02d.ogg", State.ModuleName, x);

            State.Tracks.Tracks[x].Length = (UINT)(1000.0f * AcquireAudioLength(State.Tracks.Tracks[x].Name));

            State.Tracks.Tracks[x].Position = position;

            if (State.Tracks.Tracks[x].Length < 4000)
            {
                State.Tracks.Tracks[x].Name[0] = NULL;

                // Missing tracks are 4 seconds of data.
                position = position + 4000;
            }
            else
            {
                if (State.Tracks.Current == DEFAULT_TRACK_INDEX) { State.Tracks.Current = x; }

                State.Tracks.Count = x;

                State.Tracks.Max = State.Tracks.Max + 1;

                position = position + State.Tracks.Tracks[x].Length;
            }
        }
    }

    return TRUE;
}
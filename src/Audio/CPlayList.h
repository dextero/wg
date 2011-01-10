#ifndef AUDIO_CPLAYLIST_H_
#define AUDIO_CPLAYLIST_H_

#include <vector>
#include <string>
#include "../ResourceManager/CMusic.h"

namespace Audio
{
    struct CPlayList
    {
    private:
        std::vector<System::Resource::CMusic*> mSongs;
        bool mRandomizable;
        void AddSong(System::Resource::CMusic* newSound);
    public:
        CPlayList(): mRandomizable(true) {}
        bool Load(const std::wstring &filename);
        bool IsRandomizable() const { return mRandomizable; }

        friend class CMusicPlayer;
    };
}

#endif


#ifndef _CGAMEOPTIONS_H_
#define _CGAMEOPTIONS_H_

#include "Utils/CSingleton.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>

#define gGameOptions CGameOptions::GetSingleton()

class CGameOptions: public CSingleton<CGameOptions>
{
private:
	//src/CGameOptions.cpp:187: warning: comparison between signed and unsigned integer expressions
	static const unsigned int PLAYERS_CNT = 2;

	struct keyBinding
	{
		std::string controls;
		std::string name;
		int			key;
        float       lock;

        std::vector<std::pair<std::string, int> > overrides;
	};

	unsigned int mVersion;
    unsigned int mWidth, mHeight, mBPP;
    bool mFullscreen, mVSync, m3DSound, mSmooth, mShaders;
    float mSoundVolume;
    float mMusicVolume;
	std::string mLocaleLang;
	std::string mControls[PLAYERS_CNT];
	std::vector<keyBinding> mKeyBindings[PLAYERS_CNT];

public:
	static std::string mModDir; // todo
private:
    std::string mUserDir;

    // < < nazwa_schematu, klawisz >, < platforma, nadpisywany_klawisz > >
    std::map<std::pair<std::string, int>, std::pair<std::string, int> > mOverrides;

    struct controlScheme
    {
        int player;
        std::string name;
        std::string desc;
        std::string menuDesc;
        int flags;
        std::vector<keyBinding> bindings;

        controlScheme(
            int _player,
            const std::string& _name,
            const std::string& _desc,
            const std::string& _menuDesc,
            int _flags):
                player(_player),
                name(_name),
                desc(_desc),
                menuDesc(_menuDesc),
                flags(_flags) {}

        std::string Serialize()
        {
            std::stringstream out;
            out << "\t<controlsScheme player=\"" << player << "\" name=\"" << name << "\" desc=\"" << desc << "\" menuDesc=\"" << menuDesc << "\" flags=\"" << flags << "\">\n";
            for (std::vector<keyBinding>::iterator it = bindings.begin(); it != bindings.end(); ++it)
            {
                out << "\t\t<keyBinding name=\"" << it->name << "\" key=\"" << it->key << "\"";
                if (it->lock != -1.f)
                    out << " lock=\"" << it->lock << "\"";

                if (it->overrides.size())
                {
                    out << ">\n";
                    for (std::vector<std::pair<std::string, int> >::iterator it2 = it->overrides.begin(); it2 != it->overrides.end(); ++it2)
                        out << "\t\t\t<override platform=\"" << it2->first << "\" key=\"" << it2->second << "\" />\n";
                    out << "\t\t</keyBinding>\n";
                }
                else
                    out << " />\n";
            }
            out << "\t</controlsScheme>\n";

            return out.str();
        }
    };

    std::vector<controlScheme> mDefControlSchemes[PLAYERS_CNT];
public:
    enum ControlsFlags {
        cfMouseLook          = 1,
        cfMousecast          = 2,
        cfShowOnFirstGame    = 4,
        cfSeparateSeq        = 8,   // AbiX-* + Slot-* zamiast Abi-*
        cfAbsolute           = 16,
        cfPointNClick        = 32   // poruszanie postacia point&click
    };

    CGameOptions();
    ~CGameOptions();

    bool LoadOptions();
    void UpdateWindow();
    void SaveOptions();

    // getters & setters:
    unsigned int GetWidth() { return mWidth; };
    unsigned int GetHeight() { return mHeight; };
    unsigned int GetBitsPerPixel() { return mBPP; };
    bool IsFullscreen() { return mFullscreen;};
	bool IsVSync() { return mVSync; };

    void SetWidth(unsigned int width) { mWidth = width; };
    void SetHeight(unsigned int height) { mHeight = height; };
    void SetBitsPerPixel(unsigned int bpp) { mBPP = bpp; };
    void SetFullscreen(bool fs) { mFullscreen = fs; };

	bool GetSmooth() { return mSmooth; }
	void SetSmooth(bool s);

	void SetVSync(bool vsync);

    inline float GetMusicVolume() { return mMusicVolume; }
    void SetMusicVolume(float music);

    inline float GetSoundVolume() { return mSoundVolume; }
    void SetSoundVolume(float sound);

    inline bool Get3DSound() { return m3DSound; }
    void Set3DSound(bool sound) { m3DSound = sound; }

    inline bool GetShaders() { return mShaders; }
    void SetShaders(bool shaders) { mShaders = shaders; }
    
    inline const std::string & GetControls(int player=0) { return mControls[player]; };
    void SetControls(const std::string & controls, int player=0);
    void SetControls(size_t controlsId, int player = 0);
    void SetUserControlsFlags(int player, int flags);

    void SetKeyBinding( const std::string& controls, const std::string& name, int key, int player=0, float lock=-2, bool dontAddToUserBinds = false );
	int GetKeyBinding( const std::string& controls, const std::string& name, int player=0 );

    // dodaje tez do BindManagera
    size_t AddControlScheme( int player, const std::string& name, const std::string& desc, const std::string& menuDesc, int flags );
    // NIE dodaje do BindManagera
    void AddControlSchemeBind( int player, size_t id, const std::string& bindName, int key, float lock, std::vector<std::pair<std::string, int> >& overrides );

	static inline void SetModDir (const std::string& mod) { mModDir = mod; }
	static inline const std::string& GetModDir() { return mModDir; }
    inline const std::string& GetUserDir() const { return mUserDir; }
    inline const std::vector<controlScheme>& GetDefControlSchemes(int player) const { return mDefControlSchemes[player]; }

    const std::string& GetLanguage() { return mLocaleLang; }
    void SetLanguage(const std::string& langCode);
};

#endif


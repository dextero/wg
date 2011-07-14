#ifndef __CBINDMANAGER_H__
#define __CBINDMANAGER_H__

#include "../Utils/CSingleton.h"
#include "../IKeyListener.h"
#include "../IFrameListener.h"
#include "../IMouseListener.h"
#include "../IJoyListener.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

#define gBindManager0 System::Input::CBindManager::GetActualBindManager(0)	/* Gracz 0 */
#define gBindManager1 System::Input::CBindManager::GetActualBindManager(1)	/* Gracz 1 */
#define gBindManagerByPlayer(x) System::Input::CBindManager::GetActualBindManager(x)

class CMouseCaster;

namespace System
{
	namespace Input
	{
		class CBindManager: public IKeyListener, public IFrameListener, public IMouseListener, public IJoyListener
		{
		public:
			enum EKEY_STATE
			{
				KEY_UNDEFINED = 0x00,
				KEY_PRESS = 0x01,
				KEY_HOLD = 0x02,
				KEY_RELEASE = 0x04,
				KEY_FREE = 0x08,
				KEY_LOCK = 0x10,
				KEY_PRESS_SYNC = 0x28,
				KEY_RELEASE_SYNC = 0x22
			};

			class CKeySyncGroup
			{
			public:
				CKeySyncGroup(): mTimeout(0.0f) {};

				std::vector<std::string> mBindings;
				std::vector<int> mKeys;
				float mTimeout;
			};

        static void SetActualBindManager( const std::wstring & name, unsigned playerNumber = 0 );
		static void SetActualBindManager( size_t nr, unsigned playerNumber = 0 );
		static void AddBindManager( const std::wstring & name = L"", unsigned playerNumber = 0 );
		static void NextBindManager( unsigned playerNumber = 0 );
		static CBindManager* GetActualBindManager( unsigned playerNumber = 0 );
		static CBindManager* GetBindManager( const std::wstring & name, unsigned playerNumber = 0 );
        static CBindManager* GetBindManagerAt( size_t index, unsigned playerNumber = 0 );
		static size_t GetBindManagersCount( unsigned playerNumber = 0 );

        static size_t GetBindId( const std::string & name, unsigned playerNumber = 0 );
        static const std::wstring GetBindName( size_t id, unsigned playerNumber = 0 );
        static void ReleaseKeys();

        // string = akcja, int = grupa
        typedef std::pair<std::string, unsigned int> ActionPair;
        enum EActionGroup {
            agNormal        = 1,
            agAbsolute      = 2,
            agAll           = 4,
            agMousecast     = 8,
            agKeyTurning    = 16,
            agNormalCast    = 32,
            agSeparateSeq   = 64,   // AbiX-* i Slot-* zamiast Abi-*
            agPointNClick   = 128,  // wskazywanie celu kliknieciem mysza
            agWtf           = 0
        };
        static const ActionPair availableActions[];
        static const unsigned int availableActionsCount;

		private:
			class CKey
			{
			friend class CBindManager;
			private:
				EKEY_STATE mKeyState;
				EKEY_STATE mNewKeyState;
				float mLockTime;
				float mLastUse;
				float mReleaseTime;
				bool mIsInGroup;
				bool mIsRepeatable;
			public:
                CKey(): mKeyState(KEY_UNDEFINED), mNewKeyState(KEY_UNDEFINED), mLockTime(0.0f), mLastUse((float)0x7fffffff), mReleaseTime(0.0f), mIsInGroup(false), mIsRepeatable(false) {};

				EKEY_STATE GetKeyState();
				void SetLockTime(float lockTime);
				void SetIsRepeatable(bool repeatable);
				void OnKeyRelease();
				void OnKeyPress();
				void OnFrame(float dt);
			};

			CKey mKeyboard[600];

			std::map<std::string, int> mKeys;
			std::vector<CKeySyncGroup> mKeySyncGroups;
			bool mUpdateKeySyncGroups;

			void UpdateKeySyncGroups();
			void SynchrnonizeGroup(CKeySyncGroup& group);

			static std::vector<CBindManager*> mBindManagers[2];
			static size_t mActual[2];

			virtual void KeyPressed(const sf::Event::KeyEvent &e);
			virtual void KeyReleased(const sf::Event::KeyEvent &e);

			virtual void MousePressed( const sf::Event::MouseButtonEvent &e );
			virtual void MouseReleased( const sf::Event::MouseButtonEvent &e );
            virtual void MouseWheelMoved( const sf::Event::MouseWheelEvent &e );
			
			virtual void JoyButtonPressed( const sf::Event::JoyButtonEvent &e );
			virtual void JoyButtonReleased( const sf::Event::JoyButtonEvent &e );
			virtual void JoyMoved( const sf::Event::JoyMoveEvent &e );

			virtual void FrameStarted(float secondsPassed);

            virtual bool FramesDuringPause(EPauseVariant pv) { return pv == pvLogicOnly; }; 

			bool mMouseLook;
            bool mIsAbsolute;
            bool mPointNClickMove;

            bool mShowOnFirstGame;

			CMouseCaster* mMouseCaster;

            std::wstring mId;
            std::wstring mName;
            std::wstring mDescr;
            std::wstring mMenuDesc;
			unsigned mPlayerNumber;

			inline unsigned int GetJoystickButtonId(int button, int joyId){
				return joyId * 100 + button;
			}

			inline int GetJoystickAxisId(int axis, int joyId, float position){
				//warning C4244: 'return' : conversion from 'float' to 'int', possible loss of data
				return joyId * (101 + axis) + (int)position;
			}
		public:

			CBindManager( const std::wstring & name, unsigned playerNumber );
			bool GetMouseLook();
			void SetMouseLook(bool arg);

            bool GetIsAbsolute();
            void SetIsAbsolute(bool arg);

            bool GetPointNClickMove();
            void SetPointNClickMove(bool arg);

            bool GetShowOnFirstGame();
            void SetShowOnFirstGame(bool arg);

			inline void SetMouseCaster(CMouseCaster* mc)	{ mMouseCaster = mc; }
			inline CMouseCaster* GetMouseCaster()			{ return mMouseCaster; }

			void Add(std::string name, int key, float lockTime = -1, bool repeatable = false);
			void AddJoystickButton(std::string name, int button, int joyid, float lockTime = -1, bool repeatable = false);
			void AddJoystickAxis(std::string name, int axis, int joyid, float lockTime = -1, float position = 0);
			void AddSyncGroup(CKeySyncGroup group);
			void Change(std::string name, int key, float lock = -1, bool repeatable = false);
			void SetKeyState(std::string name, bool pressed);
			EKEY_STATE Check(std::string name);
			void Remove(std::string name);

            const std::wstring & GetId();

            const std::wstring & GetName();
            void SetName( const std::wstring & name );

            const std::wstring & GetControlsMenuDescription();
            void SetControlsMenuDescription(const std::wstring & menuDesc);

            const std::wstring & GetControlsDescription();
            void SetControlsDescription(const std::wstring &descr);

			unsigned GetPlayerNumber();

			inline const std::map<std::string,int>& GetKeyBindings()	{ return mKeys; }
		};
	}
}

#endif//__CBINDMANAGER_H__

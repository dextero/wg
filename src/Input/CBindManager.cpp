#include "CBindManager.h"
#include "../CGame.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"
#include "CMouseCaster.h"
#include "../GUI/Localization/CLocalizator.h"
#include "../GUI/CAbilityTreeDisplayer.h"
#include "../Logic/CLogic.h"
#include <iostream>
#include <SFML/Window/Event.hpp>

using namespace System::Input;

template<> CBindManager* CSingleton<CBindManager>::msSingleton = 0;

std::vector<CBindManager*> CBindManager::mBindManagers[2];
size_t CBindManager::mActual[2] = { 0, 0 };

const CBindManager::ActionPair CBindManager::availableActions[] = {
    CBindManager::ActionPair("MoveUp",        CBindManager::agNormal),
    CBindManager::ActionPair("MoveDown",      CBindManager::agNormal),
    CBindManager::ActionPair("MoveLeft",      CBindManager::agNormal),
    CBindManager::ActionPair("MoveRight",     CBindManager::agNormal),
    CBindManager::ActionPair("TurnLeft",      CBindManager::agKeyTurning),
    CBindManager::ActionPair("TurnRight",     CBindManager::agKeyTurning),
    CBindManager::ActionPair("MoveUpAbs",     CBindManager::agAbsolute),
    CBindManager::ActionPair("MoveDownAbs",   CBindManager::agAbsolute),
    CBindManager::ActionPair("MoveLeftAbs",   CBindManager::agAbsolute),
    CBindManager::ActionPair("MoveRightAbs",  CBindManager::agAbsolute),
    CBindManager::ActionPair("Abi-0",         CBindManager::agNormalCast),
    CBindManager::ActionPair("Abi-1",         CBindManager::agNormalCast),
    CBindManager::ActionPair("Abi-2",         CBindManager::agNormalCast),
    CBindManager::ActionPair("Abi-3",         CBindManager::agNormalCast),
    CBindManager::ActionPair("SpecialAttack", CBindManager::agMousecast),
    CBindManager::ActionPair("QuickAttack",   CBindManager::agMousecast),
    CBindManager::ActionPair("AbiX-0",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("AbiX-1",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("AbiX-2",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("AbiX-3",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("Slot-0",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("Slot-1",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("Slot-2",        CBindManager::agSeparateSeq),
    CBindManager::ActionPair("Help",          CBindManager::agAll),
    CBindManager::ActionPair("Abilities",     CBindManager::agAll),
    CBindManager::ActionPair("PointNClick",   CBindManager::agPointNClick)
};

const unsigned int CBindManager::availableActionsCount = 26;


void CBindManager::SetActualBindManager(size_t nr, unsigned playerNumber)
{
    if ( nr >= mBindManagers[playerNumber].size() )
    {
        fprintf( stderr, "warning: CBindManager::SetActualBindManager: nr >= mBindManager.size(), returning\n" );
        return;
    }
	mActual[playerNumber] = nr;
}

void CBindManager::SetActualBindManager( const std::wstring & id, unsigned playerNumber )
{
   for ( size_t i = 0 ; i < mBindManagers[playerNumber].size() ; i++ )
   {
        if ( mBindManagers[playerNumber][i]->GetId() == id )
		{
			mActual[playerNumber] = i;

            // wymus odswiezenie kombinacji w tooltipach drzewek
            GUI::CGUIObject* abi = gLogic.GetGameScreens()->GetAbilities(playerNumber);
            if (abi != NULL)
            {
                std::string treesNames[] = { "abilities-fire", "abilities-electricity", "abilities-support", "abilities-special" };

                for (unsigned int t = 0; t < 4 /* ilosc drzewek */; ++t)
                {
                    GUI::CAbilityTreeDisplayer* atd = dynamic_cast<GUI::CAbilityTreeDisplayer*>(abi->FindObject(treesNames[t]));
                    if (atd)
                        atd->ForceReload();
                }
            }

			return;
		}
   }
}

CBindManager* CBindManager::GetBindManager( const std::wstring& id, unsigned playerNumber )
{
	for ( size_t i = 0 ; i < mBindManagers[playerNumber].size() ; i++ )
        if ( mBindManagers[playerNumber][i]->GetId() == id )
			return mBindManagers[playerNumber][i];
    return NULL;
}

CBindManager* CBindManager::GetBindManagerAt( size_t index, unsigned playerNumber )
{
    if ( index >= mBindManagers[ playerNumber ].size() )
        return NULL;

    return mBindManagers[ playerNumber ][ index ];
}

void CBindManager::AddBindManager( const std::wstring & id, unsigned playerNumber )
{
	CBindManager* tmp = new CBindManager( id, playerNumber );
	mActual[playerNumber] = mBindManagers[playerNumber].size();
	mBindManagers[playerNumber].push_back(tmp);

}

void CBindManager::NextBindManager( unsigned playerNumber )
{
//  warning: operation on �?System::Input::CBindManager::mActual’ may be undefined
//	mActual = ++mActual % mBindManagers.size();
    mActual[playerNumber] = ( mActual[playerNumber] + 1 ) % mBindManagers[playerNumber].size();
}

CBindManager* CBindManager::GetActualBindManager( unsigned playerNumber )
{
	return mBindManagers[ playerNumber ][ mActual[playerNumber] ];
}

size_t CBindManager::GetBindManagersCount( unsigned playerNumber )
{
    return mBindManagers[ playerNumber ].size();
}

size_t CBindManager::GetBindId( const std::string & name, unsigned playerNumber )
{
    for (size_t i = 0; i < mBindManagers[playerNumber].size(); ++i)
        if (StringUtils::ConvertToString(mBindManagers[playerNumber][i]->GetName()) == name)
            return i;

    return (size_t)-1;
}

const std::wstring CBindManager::GetBindName( size_t id, unsigned playerNumber )
{
    if (id >= mBindManagers[playerNumber].size())
        return L"incorrect bind id!";
    else
        return mBindManagers[playerNumber][id]->GetName();
}

void CBindManager::ReleaseKeys()
{
    for (unsigned i = 0; i < 2; ++i)
    {
        for (unsigned j = 0; j < 600; ++j)
        {
            gBindManagerByPlayer(i)->mKeyboard[j].OnKeyRelease();
            gBindManagerByPlayer(i)->mKeyboard[j].OnFrame(0.f);
        }
    }
}

/* ---------------------------------------------------------------------- */

void CBindManager::SetMouseLook(bool arg)
{
	mMouseLook = arg;
}
bool CBindManager::GetMouseLook()
{
	return mMouseLook;
}

void CBindManager::SetIsAbsolute(bool arg)
{
	mIsAbsolute = arg;
}
bool CBindManager::GetIsAbsolute()
{
	return mIsAbsolute;
}

void CBindManager::SetPointNClickMove(bool arg)
{
    mPointNClickMove = arg;
}
bool CBindManager::GetPointNClickMove()
{
    return mPointNClickMove;
}

void CBindManager::SetShowOnFirstGame(bool arg)
{
    mShowOnFirstGame = arg;
}

bool CBindManager::GetShowOnFirstGame()
{
    return mShowOnFirstGame;
}

CBindManager::CBindManager( const std::wstring & id, unsigned playerNumber )
:	mUpdateKeySyncGroups( false ), mMouseCaster( NULL ), mDescr(L""), mPlayerNumber( playerNumber )
{
    mId = id;
    mName = id;
    mMenuDesc = L"";

	gGame.AddFrameListener( this );
    gGame.AddKeyListener( this );
	gGame.AddMouseListener( this );
	gGame.AddJoyListener( this );

	mMouseLook = false;
    mIsAbsolute = false;
    mPointNClickMove = false;
    mShowOnFirstGame = false;
}

void CBindManager::MousePressed( const sf::Event::MouseButtonEvent &e )
{
	mKeyboard[sf::Key::Count + e.Button].OnKeyPress();
}

void CBindManager::MouseReleased( const sf::Event::MouseButtonEvent &e )
{
	mKeyboard[sf::Key::Count + e.Button].OnKeyRelease();
}
void CBindManager::ForceMouseLeftReleased( )
{
	fprintf(stderr, "ForceMouseLeftReleased()\n");
	mKeyboard[sf::Key::Count + sf::Mouse::Left].mKeyState = KEY_FREE;
	mKeyboard[sf::Key::Count + sf::Mouse::Left].mNewKeyState = KEY_FREE;
}

void CBindManager::KeyPressed( const sf::Event::KeyEvent &e )
{
	mKeyboard[e.Code].OnKeyPress();
}

void CBindManager::KeyReleased( const sf::Event::KeyEvent &e )
{
	mKeyboard[e.Code].OnKeyRelease();
}

void CBindManager::JoyButtonPressed( const sf::Event::JoyButtonEvent &e )
{
	mKeyboard[sf::Key::Count + sf::Mouse::ButtonCount + GetJoystickButtonId(e.Button,e.JoystickId)].OnKeyPress();
}

void CBindManager::JoyButtonReleased( const sf::Event::JoyButtonEvent &e )
{
	mKeyboard[sf::Key::Count + sf::Mouse::ButtonCount + GetJoystickButtonId(e.Button,e.JoystickId)].OnKeyRelease();
}

void CBindManager::JoyMoved( const sf::Event::JoyMoveEvent &e )
{
	mKeyboard[sf::Key::Count + sf::Mouse::ButtonCount + GetJoystickAxisId(e.Axis, e.JoystickId, e.Position)].OnKeyPress();
}

void CBindManager::UpdateKeySyncGroups()
{
	/* Jesli zmienily sie bindingi, trzeba uaktualnic takze sync-groupy */
	if ( mUpdateKeySyncGroups )
	{
		for (int i=0; i<512; i++)
			mKeyboard[i].mIsInGroup = false;

		for (unsigned g=0; g<mKeySyncGroups.size(); g++)
		{
			if (mKeySyncGroups[g].mBindings.size() > 0)
			{
				mKeySyncGroups[g].mKeys.clear();
				for (unsigned b=0; b<mKeySyncGroups[g].mBindings.size(); b++)
				{
					if (mKeys.count(mKeySyncGroups[g].mBindings[b]) != 1)
						continue;

					int key = mKeys[mKeySyncGroups[g].mBindings[b]];
					if ( key >= 0 )
						mKeySyncGroups[g].mKeys.push_back(key);
				}
			}

			for (unsigned k=0; k<mKeySyncGroups[g].mKeys.size(); k++)
				mKeyboard[ mKeySyncGroups[g].mKeys[k] ].mIsInGroup = true;
		}

		mUpdateKeySyncGroups = false;
	}
}

void CBindManager::SynchrnonizeGroup(CKeySyncGroup& group)
{
	bool release_timeout = false;
	bool press_timeout = false;
	std::vector<int>::iterator key = group.mKeys.begin();
	while(key != group.mKeys.end())
	{
		if((mKeyboard[*key].mKeyState == KEY_RELEASE_SYNC) && (mKeyboard[*key].mReleaseTime > group.mTimeout))
			release_timeout = true;
		else if((mKeyboard[*key].mKeyState == KEY_PRESS_SYNC) && (mKeyboard[*key].mLastUse > group.mTimeout))
			press_timeout = true;
		key++;
	}

	if(release_timeout)
	{
		key = group.mKeys.begin();
		while(key != group.mKeys.end())
		{
			if(mKeyboard[*key].mKeyState == KEY_RELEASE_SYNC)
				mKeyboard[*key].mKeyState = KEY_RELEASE;
			key++;
		}
	}

	if(press_timeout)
	{
		key = group.mKeys.begin();
		while(key != group.mKeys.end())
		{
			if(mKeyboard[*key].mKeyState == KEY_PRESS_SYNC)
				mKeyboard[*key].mKeyState = KEY_PRESS;
			key++;
		}
	}
}

void CBindManager::FrameStarted(float secondsPassed)
{
	UpdateKeySyncGroups();

	for(int i=0; i<512; i++)
		mKeyboard[i].OnFrame(secondsPassed);

	std::vector<CKeySyncGroup>::iterator group = mKeySyncGroups.begin();
	while(group != mKeySyncGroups.end())
	{
		SynchrnonizeGroup(*group);
		group++;
	}

	if (mMouseCaster != NULL)
		mMouseCaster->Update();
}

void CBindManager::Add(std::string name, int key, float lockTime, bool repeatable)
{
	mKeys[name] = key;
	mKeyboard[key].SetLockTime(lockTime);
	mKeyboard[key].SetIsRepeatable(repeatable);
	mUpdateKeySyncGroups = true;
}

void CBindManager::AddJoystickButton(std::string name, int button, int joyid, float lockTime, bool repeatable)
{
	int key = sf::Key::Count + sf::Mouse::ButtonCount + GetJoystickButtonId(button,joyid);
	mKeys[name] = key;
	mKeyboard[key].SetLockTime(lockTime);
	mKeyboard[key].SetIsRepeatable(repeatable);
	mUpdateKeySyncGroups = true;
}

void CBindManager::AddJoystickAxis(std::string name, int axis, int joyid, float lockTime, float position)
{
	int key = sf::Key::Count + sf::Mouse::ButtonCount + GetJoystickAxisId(axis, joyid, position);
	mKeys[name] = key;
	mKeyboard[key].SetIsRepeatable(false);
	mUpdateKeySyncGroups = true;
}
void CBindManager::AddSyncGroup(CKeySyncGroup group)
{
	mKeySyncGroups.push_back(group);
	mUpdateKeySyncGroups = true;
}

void CBindManager::Change(std::string name, int key, float lock, bool repeatable)
{
	mKeys[name] = key;
    if (lock != -2.f)
        mKeyboard[key].SetLockTime(lock);
    mKeyboard[key].SetIsRepeatable(repeatable);
	mUpdateKeySyncGroups = true;
}

void CBindManager::SetKeyState(std::string name, bool pressed)
{
	if (mKeys.count(name) == 1)
	{
		int k = mKeys[name];
		if (k >= 0 )
		{
			if (pressed)	mKeyboard[k].OnKeyPress();
			else			mKeyboard[k].OnKeyRelease();
		}
	}
}

CBindManager::EKEY_STATE CBindManager::Check(std::string name)
{
	if(mKeys.count(name) != 1)
		return KEY_UNDEFINED;

	int k = mKeys[name];
	if (k < 0 )
		return KEY_UNDEFINED;

	return mKeyboard[k].GetKeyState();
}

void CBindManager::Remove(std::string name)
{
	if(mKeys.count(name))
		mKeys.erase(name);
}

void CBindManager::CKey::SetLockTime(float lockTime)
{
	mLockTime = lockTime;
}

void CBindManager::CKey::SetIsRepeatable(bool repeatable)
{
	mIsRepeatable = repeatable;
}

CBindManager::EKEY_STATE CBindManager::CKey::GetKeyState()
{
	return mKeyState;
}

void CBindManager::CKey::OnKeyRelease()
{
	mNewKeyState = CBindManager::KEY_RELEASE;
}

void CBindManager::CKey::OnKeyPress()
{
	mNewKeyState = CBindManager::KEY_PRESS;
}

void CBindManager::CKey::OnFrame(float dt)
{
	mLastUse += dt;
	mReleaseTime += dt;

	if(mKeyState == CBindManager::KEY_PRESS)
		mKeyState = CBindManager::KEY_HOLD;
	if(mKeyState == CBindManager::KEY_RELEASE)
		mKeyState = CBindManager::KEY_FREE;

	if(!mIsInGroup)
	{
		if(mKeyState == CBindManager::KEY_PRESS_SYNC)
			mKeyState = CBindManager::KEY_HOLD;
		if(mKeyState == CBindManager::KEY_RELEASE_SYNC)
			mKeyState = CBindManager::KEY_FREE;
	}

	if(mNewKeyState == CBindManager::KEY_RELEASE)
	{
		if(mIsInGroup)
			mKeyState = CBindManager::KEY_RELEASE_SYNC;
		else
			mKeyState = CBindManager::KEY_RELEASE;
		mReleaseTime = 0;
	}

	if(mIsRepeatable && mKeyState == CBindManager::KEY_HOLD &&  mLastUse>=mLockTime)
	{
		mKeyState = CBindManager::KEY_PRESS;
		mLastUse = 0;
		mNewKeyState = CBindManager::KEY_UNDEFINED;
		return;
	}

	if(mNewKeyState == CBindManager::KEY_PRESS)
	{
		if(mLastUse>=mLockTime)
		{
			if(mKeyState != CBindManager::KEY_HOLD)
			{
				if(mIsInGroup)
					mKeyState = CBindManager::KEY_PRESS_SYNC;
				else
					mKeyState = CBindManager::KEY_PRESS;
			}

			mLastUse = 0;
		}
	}

	mNewKeyState = CBindManager::KEY_UNDEFINED;
}

const std::wstring & CBindManager::GetId()
{
    return mId;
}

const std::wstring & CBindManager::GetName()
{
    return mName;
}

void CBindManager::SetName( const std::wstring & name )
{
    mName = name;
}

void CBindManager::SetControlsMenuDescription(const std::wstring &menuDesc)
{
    mMenuDesc = menuDesc;
}

const std::wstring & CBindManager::GetControlsMenuDescription()
{
    return mMenuDesc;
}

const std::wstring & CBindManager::GetControlsDescription(){
    std::wstringstream s;
    s << gLocalizator.GetText("CTRL_STEERING") << L":\n";
    for (std::map<std::string, int>::iterator i = mKeys.begin(); i != mKeys.end(); ++i)
        s << KeyStrings::GetBindDesc(i->first.c_str()) << L" - " << KeyStrings::KeyToString(i->second).c_str() << L'\n';
    mDescr = s.str();
    return mDescr;
}

void CBindManager::SetControlsDescription(const std::wstring &descr){
    mDescr = descr;
}

unsigned CBindManager::GetPlayerNumber() {
	return mPlayerNumber;
}


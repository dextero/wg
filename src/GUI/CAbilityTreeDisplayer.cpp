#include "CAbilityTreeDisplayer.h"
#include "CWindow.h"
#include "CRoot.h"
#include "CImageBox.h"
#include "CTextArea.h"
#include "CButton.h"
#include "CInventoryDisplayer.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/CHudSprite.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "../Utils/StringUtils.h"
#include "../Utils/KeyStrings.h"
#include "../Utils/Maths.h"
#include "../Logic/Abilities/CAbilityTree.h"
#include "../Logic/Abilities/CAbility.h"
#include "../Logic/CPlayer.h"
#include "../Logic/CPlayerManager.h"
#include "../Logic/Effects/CEffectManager.h"
#include "../Logic/Effects/CEffectHandle.h"
#include "../Logic/CLogic.h"
#include "Localization/GeneralKeys.h"
#include "Localization/CLocalizator.h"
#include "../Input/CBindManager.h"

#undef CreateWindow

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <cassert>
#include <cstdio>

using namespace GUI;

const float CAbilityTreeDisplayer::HOLDER_SIZE = 15.f;//7.5f
const float CAbilityTreeDisplayer::DEPLINE_WIDTH = 5.0f;

CAbilityTreeDisplayer::CAbilityTreeDisplayer( const std::string& name, CGUIObject* parent, 
                                             unsigned int zindex, int player, const std::string &tree )
    : CGUIObject(name, GUI_ABILITYTREEDISPLAYER, parent, zindex),
    mForceReload(false),
    mTreeName(tree),
    mTree(NULL),
    mPlayer(player),
    mDepLineImage(NULL),
    mHolderImage(""),
	mHolderPotentialImage(""),
    mLvlTextFontSize(10.0f)
{
    mRows.resize(ROW_COUNT);

    mHoveredAbiBorder = CreateImageBox("hovered-ability-border");
    mHoveredAbiBorder->AddImageToSequence("data/GUI/abilities-hover-border.png");
    mHoveredAbiBorder->SetSequenceState(0);
    mHoveredAbiBorder->SetVisible(false);
}

CAbilityTreeDisplayer::~CAbilityTreeDisplayer(){

}

void CAbilityTreeDisplayer::UpdateSprites( float secondsPassed ){
    if (mIsVisible){
        CPlayer *player = gPlayerManager.GetPlayerByNumber(mPlayer);
        mTree = NULL;
        if (player)
        {
            size_t at = (size_t)player->FindAbilityTree(mTreeName);
            // dex: bez tego ifa crashowalo przy zmianie drzewka (tylko przy 2 graczach)
            if (at < player->GetBoundAbilityTrees()->size())
                mTree = player->GetBoundAbilityTrees()->at(at);
        }
        if (((mNodes.size()==0)||mForceReload)&&(mTree)){ // musimy przeladowac?
            if (mForceReload){
				mRows.resize(mTree->GetRowCount());
                for (unsigned int i = 0; i < mNodes.size(); i++)
                    mNodes[i].Release();
                mNodes.clear();
                mForceReload=false;
            }
            // zmiana tla
            dynamic_cast<CWindow*>(mParent->GetParent())->SetBackgroundImage(StringUtils::ConvertToString(mTree->GetBackground()));

            const std::vector<SAbilityNode> &aNodes = mTree->GetAbilityNodes();
            std::vector<SAbilityInstance> *playerAbis = player->GetAbilityPerformer().GetAbilities();
            // tworzymy podstawowe obiekty GUI
            mNodes.reserve(aNodes.size()); // konieczne
            for (unsigned int i = 0; i < aNodes.size(); ++i){
                LoadAbilityNode(aNodes[i]);
            }
            // czyscimy informacje o rzedach - na wszelki wypadek
            for (unsigned int i = 0; i < mRows.size(); ++i)
                mRows[i].clear();
            // uaktualniamy, ktora umiejetnosc mieszka w ktorym rzedzie
            for (unsigned int i = 0; i < mNodes.size(); ++i){
                int r = mNodes[i].abiNode->row;
                if ( r < 0 ) r = 0;
                if ( r >= ROW_COUNT ) r = ROW_COUNT -1;
                mRows[r].push_back(i);
            }
            // rozlokowujemy obiekty GUI w rzedach
            // jak rozmiescic srodki ikonek w pionie?
            float rowYSpacing = 100.0f / (mRows.size() + 1);
            for (unsigned int i = 0; i < mRows.size(); i++){
                float y = (i + 1) * rowYSpacing;
                // jak rozmiescic srodki ikonek w poziomie?
                float rowXSpacing = 100.0f / (mRows[i].size() + 1);
                for (unsigned int j = 0; j < mRows[i].size(); j++){
                    float x = (j + 1) * rowXSpacing;
                    CAbilityNodeMirror *anm = &(mNodes[mRows[i][j]]);
                    float h = HOLDER_SIZE * mGlobalSize.x / mGlobalSize.y;
                    anm->holder->SetPosition(x - HOLDER_SIZE / 2.0f, y - h / 2.0f, HOLDER_SIZE, h);
                    int treeIdx = -1,abiIdx = -1;
                    anm->GetAbilityAddress(treeIdx,abiIdx);
                    if ((treeIdx >= 0) && (abiIdx >= 0)){
                        EAbilityBuyingResult abr = player->CanBuyAbilityLevel(treeIdx,abiIdx);
						anm->canBuy = (abr == abrOK);
                        // zmieniamy ikone w zaleznosci od tego, czy mozna kupic umiejetnosc
                        // (albo czy juz jest kupiona)
                        bool found = false;
                        for (unsigned int k = 0; k < playerAbis->size(); k++){
                            if (playerAbis->at(k).abiNode == anm->abiNode){
                                found = true;
                                break;
                            }
                        }
                        if ((abr == abrOK)||found){
                            anm->button->SetImage(anm->abiNode->ability->icon);

                            // drag&drop - czy kupiona?
                            bool isBought = false;
					        std::vector<SAbilityInstance> *abis = player->GetAbilityPerformer().GetAbilities();
                            for (unsigned k = 0; k < abis->size(); k++)
						        if ((abis->at(k).abiNode == anm->abiNode) && (abis->at(k).level>0)){
                                    isBought = true;
                                    break;
                                }
                            // sprawdzanie typu
                            EAbilityType abiType = anm->abiNode->ability->abilityType;
                            if (abiType != atNone && abiType != atPassive && abiType != atExport && isBought)
                                anm->button->SetDraggable(true);
                            else
                                anm->button->SetDraggable(false);
                        } else {
                            anm->button->SetImage(anm->abiNode->ability->unavailableIcon);
                            anm->button->SetDraggable(false);
                        }
                    }
                }
            }
            // linie zaleznosci
            for (unsigned int i = 0; i < mNodes.size(); i++){
                CAbilityNodeMirror &anm = mNodes[i];
                sf::Vector2f anmPos = ConvertToGlobalPosition(anm.holder->GetPosition() + anm.holder->GetSize() / 2.0f);
                for (unsigned int j = 0; j < anm.depLines.size(); j++){
                    CAbilityNodeMirror &preq = mNodes[anm.abiNode->preqs[j].first];
                    sf::Vector2f preqPos = ConvertToGlobalPosition(preq.holder->GetPosition() + preq.holder->GetSize() / 2.0f);
                    CHudSprite *hs = anm.depLines[j];
                    hs->GetSFSprite()->Resize(Maths::Length(anmPos-preqPos),DEPLINE_WIDTH);
                    float rot = - Maths::AngleBetween(Maths::VectorRight(),anmPos-preqPos);
                    hs->GetSFSprite()->SetRotation(rot);
                    hs->GetSFSprite()->SetPosition(preqPos);
                }
            }
            // txtboxy o lvlach umiejetnosci
            std::vector<SAbilityInstance> *abiIntances = player->GetAbilityPerformer().GetAbilities();
            for (unsigned int i = 0; i < mNodes.size(); i++){
                CAbilityNodeMirror &anm = mNodes[i];
                anm.lvlText->SetPosition(
                    anm.holder->GetPosition().x*100.0f-HOLDER_SIZE*0.6f,
                    anm.holder->GetPosition().y*100.0f+HOLDER_SIZE*0.25f,
                    HOLDER_SIZE*0.6f,
                    HOLDER_SIZE*0.5f);
                int level = 0;
                for (unsigned int j = 0; j < abiIntances->size(); j++){
                    if (abiIntances->at(j).abiNode == anm.abiNode){
                        level = abiIntances->at(j).level;
                        break;
                    }
                }
				anm.leveled = level;
                if (level > 0){
                    anm.lvlText->SetText(L"[" + 
                        StringUtils::ToWString(level) + L"]");
                    anm.lvlText->SetVisible(true);
				} else {
                    anm.lvlText->SetVisible(false);
				}
            }
        }
        // nawet, gdy nie trzeba wczytywac info o umiejetnosciach, trzeba pare rzeczy "naprawic"
        // linie zaleznosci
        for (unsigned int i = 0; i < mNodes.size(); i++){
            CAbilityNodeMirror &anm = mNodes[i];
            for (unsigned int j = 0; j < anm.depLines.size(); j++){
                anm.depLines[j]->SetVisible(true);
            }
        }
        // nazwa umiejki, ramka
        mHoveredAbiBorder->SetVisible(false);
        for (unsigned int i = 0; i < mNodes.size(); i++){
            CAbilityNodeMirror &anm = mNodes[i];
                
            if (anm.button->IsMouseOver())
            {
                // ramka
                sf::Vector2f pos = anm.holder->GetPosition() * 100.f;
                float y = HOLDER_SIZE * mGlobalSize.x / mGlobalSize.y;
                mHoveredAbiBorder->SetSequenceState(0);
                mHoveredAbiBorder->SetPosition(pos.x - HOLDER_SIZE / 2.f, pos.y - y / 2.f, HOLDER_SIZE * 2.f, y * 2.f);
                mHoveredAbiBorder->SetVisible(true);

                // tekst informacyjny
                CTextArea* info = dynamic_cast<CTextArea*>(anm.button->GetTooltip()->FindObject("info"));
                int treeIdx = -1, abiIdx = -1;
                anm.GetAbilityAddress(treeIdx,abiIdx);
                if ((treeIdx >= 0) && (abiIdx >= 0))
                {
                    EAbilityBuyingResult canBuy = player->CanBuyAbilityLevel(treeIdx,abiIdx);

                    info->SetColor(sf::Color::Red);
                    switch(canBuy)
                    {
                        case abrLevelReqNotMet:
                            info->SetText(gLocalizator.GetText("REQ_LEVEL") + StringUtils::ToWString(anm.abiNode->minPlayerLevel));
                            break;
                        case abrNoPoints:
                            info->SetText(gLocalizator.GetText("NOT_ENOUGH_SP"));
                            break;
                        case abrTooLowPreqLevel:
                            info->SetText(gLocalizator.GetText("TOO_LOW_PREQ_LEVEL") + player->GetUnboughtPrequisitivesString(treeIdx,abiIdx));
                            break;
                        case abrMaxLevelReached:
                            info->SetText(gLocalizator.GetText("MAX_LEVEL_REACHED"));
                            break;
                        case abrNoBuy:
                            info->SetText(gLocalizator.GetText("ABILITY_NOT_FOR_SALE"));
                            break;
                        default:
                            info->SetText(gLocalizator.GetText("CLICK_TO_BUY"));
                            info->SetColor(sf::Color::Green);
                            break;
                    }
                }
            }
        }
		// ikonki tla pod umiejetnosciami
		for (unsigned int i = 0; i < mNodes.size(); i++){
            CAbilityNodeMirror &anm = mNodes[i];
			anm.holder->ReleaseImages();
			if (anm.leveled > 0)
				anm.holder->AddImageToSequence(mHolderActiveImage);
			else {
				if (anm.canBuy)
					anm.holder->AddImageToSequence(mHolderPotentialImage);
				else
					anm.holder->AddImageToSequence(mHolderImage);
			}
		}
    } else { // mIsVisible = false
        // depLine'y nie sa czescia GUI - ustawiamy je recznie
        for (unsigned int i = 0; i < mNodes.size(); i++)
            for (unsigned int j = 0; j < mNodes[i].depLines.size(); j++)
                mNodes[i].depLines[j]->SetVisible(false);
    }
}

void CAbilityTreeDisplayer::SetDependencyLineImage(const std::string &filename){
    mDepLineImage = gResourceManager.GetImage(filename);
}

void CAbilityTreeDisplayer::SetHolderImage(const std::string &filename,float padding){
    mHolderImage = filename;
    mHolderPadding = padding;
}

void CAbilityTreeDisplayer::LoadAbilityNode(const SAbilityNode &an){
    mNodes.push_back(*(new CAbilityNodeMirror(&an)));
    CAbilityNodeMirror *m = &(mNodes[mNodes.size()-1]);
    m->lvlText = CreateTextArea(mName+"_"+StringUtils::ToString(mChilds.size()));
    m->lvlText->SetColor(sf::Color::White);
    m->lvlText->SetFont(mLvlTextFont,mLvlTextFontSize);
    m->holder = CreateImageBox(mName+"_"+StringUtils::ToString(mChilds.size()));
    m->holder->SetPadding(mHolderPadding,mHolderPadding,mHolderPadding,mHolderPadding);
    m->holder->AddImageToSequence(mHolderImage);
    m->button = m->holder->CreateButton(mName+"_"+StringUtils::ToString(mChilds.size()));
    m->button->SetPosition(0,0,100.0f,100.0f);
    m->button->GetClickCallback()->bind(m,&CAbilityNodeMirror::AddAbilityCallback);
    m->button->SetImage(m->abiNode->ability->icon);

    CWindow* tooltip = gGUI.CreateWindow(mName+"_tooltip"+StringUtils::ToString(mChilds.size()), true, Z_GUI4);
    tooltip->SetBackgroundImage("data/GUI/transparent-black.png");
    tooltip->SetVisible(false);
    tooltip->SetOffset(ConvertToGlobalPosition(sf::Vector2f(HOLDER_SIZE / 200.f, 0.f)));

    // opis
    CTextArea* description = tooltip->CreateTextArea("desc");
    description->SetVisible(false);
    //description->SetCenter(true); // ³eeee³oooo³eeee³oooo bug alert, nie centrowac
    description->SetAutoHeight(true);
    description->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);
    
	EAbilityType type = an.ability->abilityType;
    // kombinacja
    std::wstring seq = L"";
    if (type != atNone && type != atPassive && type != atExport && an.ability->trigger.size() > 0){
        
        // copypasta z Xirdusowego kodu (CGameScreens::InitControlsListing)
        std::string t = an.ability->trigger;
        std::string t_ = " (", aReplace = "a", bReplace = "b", cReplace = "c", dReplace = "d";
        
        if ( System::Input::CBindManager::GetActualBindManager(mPlayer) != NULL )
		{
            const std::map<std::string,int>& keys = System::Input::CBindManager::GetActualBindManager(mPlayer)->GetKeyBindings();

			if ( keys.count("Abi-0") > 0 )	aReplace = KeyStrings::KeyToString( keys.find("Abi-0")->second );
            else if ( keys.count("AbiX-0") > 0 ) aReplace = KeyStrings::KeyToString( keys.find("AbiX-0")->second );
			if ( keys.count("Abi-1") > 0 )	bReplace = KeyStrings::KeyToString( keys.find("Abi-1")->second );
            else if ( keys.count("AbiX-1") > 0 ) bReplace = KeyStrings::KeyToString( keys.find("AbiX-1")->second );
			if ( keys.count("Abi-2") > 0 )	cReplace = KeyStrings::KeyToString( keys.find("Abi-2")->second );
            else if ( keys.count("AbiX-2") > 0 ) cReplace = KeyStrings::KeyToString( keys.find("AbiX-2")->second );
			if ( keys.count("Abi-3") > 0 )	dReplace = KeyStrings::KeyToString( keys.find("Abi-3")->second );
            else if ( keys.count("AbiX-3") > 0 ) dReplace = KeyStrings::KeyToString( keys.find("AbiX-3")->second );
		}

		for ( unsigned j = 0; j < t.size(); j++ )
		{
			switch ( t[j] )
			{
			case 'a':	t_ += aReplace; break;
			case 'b':	t_ += bReplace; break;
			case 'c':	t_ += cReplace; break;
			case 'd':	t_ += dReplace; break;
            default:    t_ += t[j];
			}

			if ( j < t.size() - 1 )
				t_ += ",";
		}

        seq = StringUtils::ConvertToWString(t_ + ")");
    }

	std::wstring text = an.ability->name + L"\n\n";
	if (type != atNone && type != atPassive && type != atExport)
    {
        if (seq.size() > 0)
		    text += gLocalizator.GetText("KEY_SEQUENCE") + seq + L"\n\n";
        
        text += gLocalizator.GetText("DRAG_TO_USE");
    }
	else
		text += gLocalizator.GetText("ABI_PASSIVE");

	text += L"\n\n" + an.ability->description;
    description->SetText(text);
    description->SetPosition(20.f, 20.f, 360.f, 0.f, UNIT_PIXEL);

    // info (wymagania itd)
    CTextArea* info = tooltip->CreateTextArea("info");
    info->SetVisible(false);
    info->SetAutoHeight(true);
    info->SetFont(gLocalizator.GetFont(FONT_DIALOG), 14.f);

    tooltip->UpdateChilds(0.f);
    description->UpdateText();
    info->SetPosition(20.f, description->GetSize(UNIT_PIXEL).y + 30.f, 360.f, 0.f, UNIT_PIXEL);
    info->UpdateText();
    tooltip->SetPosition(0.f, 0.f, 400.f, info->GetPosition(UNIT_PIXEL).y + info->GetSize(UNIT_PIXEL).y + 30.f, UNIT_PIXEL);

    // nie chcemy, zeby tooltip odbieral komunikaty myszy
    gGUI.UnregisterObject(tooltip);
    gGUI.UnregisterObject(description);
    gGUI.UnregisterObject(info);
    
    m->button->SetTooltip(tooltip);

    for (unsigned int i = 0; i < an.preqs.size(); i++){
        m->depLines.push_back(gDrawableManager.CreateHudSprite(mZIndex+1));
        m->depLines[m->depLines.size()-1]->GetSFSprite()->SetImage(*mDepLineImage);
    }
	m->leveled = false;
}

void CAbilityTreeDisplayer::CAbilityNodeMirror::Release(){
    if (holder) { holder->Remove(); holder = NULL; }
    if (lvlText) { lvlText->Remove(); lvlText = NULL; }
    for (unsigned int i = 0; i < depLines.size(); ++i){
        gDrawableManager.DestroyDrawable(depLines[i]);
    }
    depLines.clear();
}

void CAbilityTreeDisplayer::CAbilityNodeMirror::AddAbilityCallback(){
    CAbilityTreeDisplayer *atd = (CAbilityTreeDisplayer*)(holder->GetParent());
    CPlayer *pl = gPlayerManager.GetPlayerByNumber(atd->mPlayer);
    int treeIdx = -1, abiIdx = -1;
    GetAbilityAddress(treeIdx,abiIdx);
    if ((treeIdx >= 0) && (abiIdx >= 0)){
		if (pl->TryBuyAbilityLevel(treeIdx,abiIdx) == abrOK){
			atd->mForceReload=true;
            gLogic.GetGameScreens()->GetInventoryDisplayer(atd->mPlayer)->ForceReload();
		}
    }
}

void CAbilityTreeDisplayer::CAbilityNodeMirror::GetAbilityAddress(int &outTreeIdx, int &outAbiIdx){
    outTreeIdx = -1;
    outAbiIdx = -1;
    CAbilityTreeDisplayer *atd = (CAbilityTreeDisplayer*)(holder->GetParent());
    CPlayer *pl = gPlayerManager.GetPlayerByNumber(atd->mPlayer);
    std::vector<CAbilityTree*> *ats = pl->GetBoundAbilityTrees();
    for (unsigned int i = 0; i < ats->size(); ++i){
        if (ats->at(i) == atd->mTree){
            outTreeIdx = i;
            break;
        }
    }
    const std::vector<SAbilityNode> &ans = atd->mTree->GetAbilityNodes();
    for (unsigned int i = 0; i < ans.size(); ++i){
        if (&(ans[i]) == abiNode){
            outAbiIdx = i;
            break;
        }
    }
}

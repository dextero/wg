#include "CPlayerTemplate.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/CRand.h"
#include "../../Utils/MathsFunc.h"
#include "../CPhysicalManager.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../Abilities/SAbilityInstance.h"
#include "../../Input/CPlayerController.h"
#include "../Abilities/CTriggerEffects.h"
#include "../Abilities/CAbilityTree.h"
#include "../../Utils/ToxicUtils.h"
#include "../Abilities/CAbility.h"

using namespace rapidxml;
using namespace StringUtils;
using namespace Maths;

std::string CPlayerTemplate::GetType()
{
	return "CPlayerTemplate";
}

bool CPlayerTemplate::Load(CXml &xml)
{
	
    if (!CActorTemplate::Load(xml)) return false;

    std::string triggerFile = xml.GetString(xml.GetChild(xml.GetRootNode(),"trigger-effects"));
    if (triggerFile != "")
        mTriggerEffects = new CTriggerEffects(triggerFile,mAnimations);
    else
        mTriggerEffects = NULL;

	if ( xml.GetString(0,"type") != "player" )
		return false;

    for (xml_node<> *node = xml.GetChild(xml.GetRootNode(),"ability-tree"); node; node = xml.GetSibl(node,"ability-tree")){
        CAbilityTree *at = new CAbilityTree();
        at->Load(xml.GetString(node));
        mAbilityTrees.push_back(at);
        mAbilityTreeCodes.push_back(xml.GetString(node,"code"));
    }

    xml_node<>* hud = xml.GetChild(xml.GetRootNode(), "hud");

    if (!hud)
    {
        fprintf(stderr, "ERROR: didn't find <hud> in player xml");
        return false;
    }

	mHudDesc.hudType = xml.GetString(hud, "type");
	
	for (xml_node<>* node = xml.GetChild(hud,"position"); node; node=xml.GetSibl(node,"position"))
	{
		GUI::guiUnit u = (xml.GetString(node,"unit") == "pixel" ? GUI::UNIT_PIXEL : GUI::UNIT_PERCENT);
		mHudDesc.position[u].x = xml.GetFloat(node,"x");
		mHudDesc.position[u].y = xml.GetFloat(node,"y");
		mHudDesc.size[u].x = xml.GetFloat(node,"width");
		mHudDesc.size[u].y = xml.GetFloat(node,"height");
	}

    for (xml_node<>* node = hud->first_node(); node; node = node->next_sibling())
    {
        if (!node->first_attribute())
            continue;

        if (std::string(node->name()) == "circle1")
            mHudDesc.path[HUDDESC_CIRCLE1] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "bg")
            mHudDesc.path[HUDDESC_BG] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "avatar0")
            mHudDesc.path[HUDDESC_AVATAR0] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "avatar1")
            mHudDesc.path[HUDDESC_AVATAR1] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "avatar2")
            mHudDesc.path[HUDDESC_AVATAR2] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "avatar3")
            mHudDesc.path[HUDDESC_AVATAR3] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "avatar4")
            mHudDesc.path[HUDDESC_AVATAR4] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "circle2")
            mHudDesc.path[HUDDESC_CIRCLE2] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "circle3")
            mHudDesc.path[HUDDESC_CIRCLE3] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "hp")
            mHudDesc.path[HUDDESC_HP] = node->first_attribute("path")->value();
		else if (std::string(node->name()) == "hp-bg")
            mHudDesc.path[HUDDESC_HP_BG] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "mana")
            mHudDesc.path[HUDDESC_MANA] = node->first_attribute("path")->value();
		else if (std::string(node->name()) == "mana-bg")
            mHudDesc.path[HUDDESC_MANA_BG] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "mana-flared")
            mHudDesc.path[HUDDESC_MANA_FLARED] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "xp")
            mHudDesc.path[HUDDESC_XP] = node->first_attribute("path")->value();
		else if (std::string(node->name()) == "xp-bg")
            mHudDesc.path[HUDDESC_XP_BG] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "fg")
            mHudDesc.path[HUDDESC_FG] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "fg-flared")
            mHudDesc.path[HUDDESC_FG_FLARED] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "seq-q")
            mHudDesc.path[HUDDESC_SEQ_Q] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "seq-w")
            mHudDesc.path[HUDDESC_SEQ_W] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "seq-e")
            mHudDesc.path[HUDDESC_SEQ_E] = node->first_attribute("path")->value();
        else if (std::string(node->name()) == "seq-r")
            mHudDesc.path[HUDDESC_SEQ_R] = node->first_attribute("path")->value();
    }

    return true;
}

void CPlayerTemplate::Drop()
{
	delete this;
}

CPlayer* CPlayerTemplate::Create(std::wstring id)
{
    CPlayer* player = gPhysicalManager.CreatePlayer(id);

	float random =	gRand.Rndf( 0.0f, 1.0f );

	player->SetCategory(PHYSICAL_PLAYER);
    Fill(player,random);
    player->GetAbilityPerformer().Bind(new std::vector<SAbilityInstance>(), new std::vector<int>());

    if (ToxicUtils::isGameInCrimsonMode) {
        CAbility * ability = NULL;
        ability = gResourceManager.GetAbility("data/player/firearrow.xml");
        ability->trigger = "a";
        SAbilityInstance ai(ability,1);
        player->GetAbilityPerformer().AddAbility(ai,/*an->animCode o cholera, skad to wziac? dam jeden moze bedzie pasowac:*/ 1 );
//        ability = gResourceManager.GetAbility("data/player/freezebomb.xml");
//        ability->trigger = "b";
//        ai = SAbilityInstance(ability,1);
//        player->GetAbilityPerformer().AddAbility(ai,/*an->animCode o cholera, skad to wziac? dam jeden moze bedzie pasowac:*/ 2 );
//        ability = gResourceManager.GetAbility("data/player/entangle.xml");
//        ability->trigger = "c";
//        ai = SAbilityInstance(ability,8);
//        player->GetAbilityPerformer().AddAbility(ai,/*an->animCode o cholera, skad to wziac? dam jeden moze bedzie pasowac:*/ 3 );
//        ability = gResourceManager.GetAbility("data/player/meditation.xml");
//        ability->trigger = "d";
//        ai = SAbilityInstance(ability,4);
//        player->GetAbilityPerformer().AddAbility(ai,/*an->animCode o cholera, skad to wziac? dam jeden moze bedzie pasowac:*/ 4 );
    } 

    for (unsigned int i = 0; i < mAbilities.size(); i++)
        player->GetAbilityPerformer().AddAbility(mAbilities[i],mAbilityAnims[i]);

    player->BindAbilityTrees(&mAbilityTrees,&mAbilityTreeCodes);
    player->GetController()->BindTriggerEffects(mTriggerEffects);
    player->GetStats()->SetBaseAspect(aLevel,1.0f);
    player->SetHudDesc(mHudDesc);

	return player;
}


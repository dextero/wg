#include "CAbilityTree.h"
#include "../../Utils/StringUtils.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../../Rendering/Animations/CAnimSet.h"
#include "../../GUI/Localization/CLocalizator.h"
#include <cstdlib>
#include <cstring>

CAbilityTree::CAbilityTree():mRowCount(0){
    
}

CAbilityTree::~CAbilityTree(){

}

int CAbilityTree::FindByCode(const std::string &code){
    for (unsigned int i = 0; i < mAbilityNodes.size(); i++)
        if (mAbilityNodes[i].code == code)
            return i;
    return -1;
}

void CAbilityTree::Load(const std::string &filename){
    CXml xml(filename,"root");
    Load(xml,xml.GetRootNode());
}

void CAbilityTree::Load(CXml &xml, rapidxml::xml_node<> *node){
    mName = gLocalizator.Localize(xml.GetString(xml.GetChild(node, "name")));
    mBackground = (xml.GetChild(node, "background") ? StringUtils::ConvertToWString(xml.GetString(xml.GetChild(node, "background"))) : L"data/GUI/skills-bkg.jpg");

    rapidxml::xml_node<> *tmpNode;
    for(rapidxml::xml_node<> *n = xml.GetChild(node,"ability"); n; n = xml.GetSibl(n,"ability")){
        SAbilityNode an;
        an.abilityFilename = xml.GetString(xml.GetChild(n,"name"));
        an.code = xml.GetString(n,"code");
        an.minPlayerLevel = xml.GetInt(xml.GetChild(n,"req-level"));
        for (rapidxml::xml_node<>* req = xml.GetChild(n, "req"); req; req = req->next_sibling("req"))
        {
            std::string code = xml.GetString(req, "ability");
            int at = FindAbility(code);
            if (at != -1)
                an.preqs.push_back(std::pair<int, int>(at, xml.GetInt(req, "level", 1)));
            else
                fprintf(stderr, "WARNING: invalid req: \"%s\" at %s, abi %s, omitting..\n", code.c_str(), xml.GetFilename().c_str(), an.abilityFilename.c_str());
        }
		an.sharedTrigger = (xml.GetChild(n,"shared-trigger") != NULL);

        tmpNode = xml.GetChild(n,"anim");
        if (tmpNode){
            std::string tmp = xml.GetString(tmpNode);
            an.animCode = CAnimSet::ParseAnimClass(tmp);
        }
        tmpNode = xml.GetChild(n,"export");
        if (tmpNode){
			an.doExport = true;
			tmpNode = xml.GetChild(tmpNode,"name");
			if (tmpNode){
				an.exportName = xml.GetString(tmpNode);
			}
		}

        an.startLevel = xml.GetInt(xml.GetChild(n,"start-level"));
        an.row = xml.GetInt(xml.GetChild(n,"row"),"",-1);
        mAbilityNodes.push_back(an);
    }
	for (unsigned int i = 0; i < mAbilityNodes.size(); i++){
		SAbilityNode &an = mAbilityNodes[i];
		if (mRowCount < (an.row+1))
			mRowCount = an.row + 1;
		an.ability = gResourceManager.GetAbility (an.abilityFilename);	
	}
}

int CAbilityTree::FindAbility(const std::string &code){
    for (unsigned int i = 0; i < mAbilityNodes.size(); i++)
        if (mAbilityNodes[i].code == code)
            return i;
    return -1;
}


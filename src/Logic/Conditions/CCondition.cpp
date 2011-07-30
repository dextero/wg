#include "CCondition.h"
#include "../CPhysicalManager.h"
#include "../../Map/CMapManager.h"
#include "../PhysicalCategories.h"
#include "CConditionManager.h"
#include "../../Utils/StringUtils.h"
#include "../MapObjects/CRegion.h"

std::string CCondition::ConditionTypeNames[] = {"none", "clear", "time-elapsed", "region-triggered", "killed" };
const int CCondition::ConditionTypeCount = sizeof(CCondition::ConditionTypeNames)/sizeof(std::string);

std::string CCondition::SpecificCheckTypeNames[] = {"is", "isnt", "once", "never"};
const int CCondition::SpecificCheckTypeCount = sizeof(CCondition::SpecificCheckTypeNames)/sizeof(std::string);

CCondition::CCondition():
mType(ctNone),
mParam(0),
mfParam(0.0f),
msParam(""),
mRegionParam(Memory::CSafePtr<CDynamicRegion>::NullPtr()),
mSct(sctIs),
mWasTriggered(false),
mCurrentState(false),
mIndex(-1)
{
    // opcjonalne: wymusza sprawdzanie Conditiona co klatke
    // mozna odkomentowac (lub wywolac z zewnatrz) gdy komus bedzie to potrzebne
    // Register();
}

CCondition::~CCondition(){
    fprintf(stderr,"~CCondition() at %p\n",this);
    Unregister();
}

void CCondition::SetType(ConditionType type){
    mType = type;
    switch(mType){
        case ctClear:
            mParam = PHYSICAL_HOSTILES;
            break;
        default:
            mParam = 0;
            break;
    }
}

void CCondition::SetSpecificCheckType(int sct){
    mSct = (SpecificCheckType)sct;
}

void CCondition::LoadParam(const std::string &str){
    switch(mType){
        case ctClear:
            if (str != "")
                mParam = ParsePhysicalFilter(str);
            break;
        case ctTimeElapsed:
            if (str != "")
                StringUtils::FromString(str,mfParam);
            break;
        case ctRegionTriggered:
            if (str != "")
                msParam = str;
            break;
        case ctEnemyKilled:
            if (str != "")
                msParam = str;
            break;
        default:
            break;
    }
}
void CCondition::Load(CXml &xml, rapidxml::xml_node<> *node){
    SetType(ParseConditionType(xml.GetString(xml.GetChild(node,"type"))));
    rapidxml::xml_node<> *param = xml.GetChild(node,"param");
    LoadParam(xml.GetString(param));
    std::string tmp = xml.GetString(node,"check");
    SetSpecificCheckType(ParseSpecificCheckType(tmp));
    if ((mSct == sctOnce) || (mSct == sctNever))
        Register();
}

void CCondition::SerializeParam(std::ostream &out, int indent){
    switch(mType){
        case ctClear:
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "<param>" << SerializePhysicalFilter(mParam) << "</param>\n";
            break;
        case ctTimeElapsed:
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "<param>" << mfParam << "</param>\n";
            break;
        case ctRegionTriggered:
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "<param>" << msParam << "</param>\n";
            break;
        case ctEnemyKilled:
            for (int i = 0; i < indent; ++i, out << "\t");
            out << "<param>" << msParam << "</param>\n";
            break;
        default:
            break;
    }
}

/*
    <cond check="is">
      <type>clear</type>
      <param>hostiles</param>
    </cond>*/
void CCondition::Serialize(std::ostream &out, int indent){
    for (int i = 0; i < indent; ++i, out << "\t");
    out << "<cond check=\"" << SpecificCheckTypeNames[(int)mSct] << "\">\n";
    for (int i = 0; i < indent + 1; ++i, out << "\t");
    out << "<type>" << ConditionTypeNames[(int)mType] << "</type>\n";
    SerializeParam(out, indent + 1);
    for (int i = 0; i < indent; ++i, out << "\t");
    out << "</cond>\n";
}

bool CCondition::LoadConsoleFriendly(const std::string& data)
{
	std::vector<std::string> exploded = StringUtils::Explode(data, ";");
	if (exploded.size() < 2)
		return false;

	SetSpecificCheckType(ParseSpecificCheckType(exploded[0]));
	SetType(ParseConditionType(exploded[1]));

	if (exploded.size() > 2)
		LoadParam(exploded[2]);

    if ((mSct == sctOnce) || (mSct == sctNever))
        Register();

	return true;
}

const std::string CCondition::SerializeConsoleFriendly()
{
	std::stringstream ss;
	ss << SpecificCheckTypeNames[(int)mSct] << ";" << ConditionTypeNames[(int)mType];
	switch (mType)
	{
	case ctClear: ss << ";" << SerializePhysicalFilter(mParam); break;
	case ctTimeElapsed: ss << ";" << mfParam; break;
	case ctRegionTriggered:
	case ctEnemyKilled: ss << ";" << msParam; break;
	default: break;
	}

	return ss.str();
}

bool CCondition::IsTriggered(){
    if (mIndex < 0)
        Check();
    return mCurrentState;
}

bool CCondition::SpecificCheck(int sct){
    switch(sct){
        case sctIsnt: 
            return !IsTriggered();
        case sctOnce: 
            return WasEverTriggered();
        case sctNever: 
            return !WasEverTriggered();
        default:
            return IsTriggered();
    }
}

bool CCondition::SpecificCheck(){
    return SpecificCheck(mSct);
}

CCondition::ConditionType CCondition::ParseConditionType(const std::string &str){
    for (int i = 1; i < ConditionTypeCount; i++) // ignorujemy ctNone
        if (ConditionTypeNames[i]==str)
            return ConditionType(i);
    if (str != ConditionTypeNames[0])
        fprintf(stderr,"WARNING: unable to parse condition type: %s\n",str.c_str());
    return ctNone;
}

int CCondition::ParseSpecificCheckType(std::string &str){
    for (int i = 1; i < SpecificCheckTypeCount; i++) // ignorujemy sctIs
        if (SpecificCheckTypeNames[i]==str)
            return SpecificCheckType(i);
    if (str != SpecificCheckTypeNames[0])
        fprintf(stderr,"WARNING: unable to check type: %s\n",str.c_str());
    return sctIs;
}

void CCondition::Register(){
    if (mIndex < 0){
        mIndex = gConditionManager.RegisterCondition(this);
    }
}

void CCondition::Unregister(){
    gConditionManager.UnregisterCondition(mIndex);
}

void CCondition::Reset(){
    mWasTriggered = false;
    mCurrentState = false;
    mRegionParam = Memory::CSafePtr<CDynamicRegion>::NullPtr();
}

void CCondition::ClearTriggered(){
    mWasTriggered = false;
    mCurrentState = false;
}

void CCondition::TryCheck(float dt){
    Check(); // na razie tylko tyle ;)
    // mozna to kiedys usprawnic - sprawdzanie w pewnych odstepach czasu
}

void CCondition::SwitchIndex(int i){
    mIndex = i;
}

void CCondition::Check(){
    bool result = false;
    switch(mType){
        case ctClear:
            result = !gScene.ContainsPhysicals(  mParam  );
            break;
        case ctTimeElapsed:
            result = gMapManager.GetCurrentMapTimeElapsed() > mfParam;
            break;
        case ctRegionTriggered:
            if (mRegionParam == NULL)
            {
                CDynamicRegion* tmp = dynamic_cast<CDynamicRegion*>(gPhysicalManager.GetPhysicalById(StringUtils::ConvertToWString(msParam)));
                if (tmp)
                    mRegionParam = tmp->GetSafePointer();
            }
            if (mRegionParam != NULL)
                result = mRegionParam->IsTriggered();
            break;
        case ctEnemyKilled:
            {
                CPhysical* physical = gPhysicalManager.GetPhysicalById(StringUtils::ConvertToWString(msParam));
                if (physical)
                    if (physical->IsDead())
                        result = true;
            }
            break;
        default:
            break;
    }
    if (!mWasTriggered)
        mWasTriggered = result;
    mCurrentState = result;
}


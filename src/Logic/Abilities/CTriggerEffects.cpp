#include "CTriggerEffects.h"
#include <vector>
#include "../../Utils/StringUtils.h"
#include "../../Utils/CXml.h"
#include "../../ResourceManager/CResourceManager.h"
#include "../Effects/CEffectManager.h"
#include "../../Rendering/Animations/CAnimSet.h"

const int CTriggerEffects::AbilityKeyCount = 4;

class CTriggerEffects::CTriggerKeyMap{
private:
    struct Node{
        Node *children[CTriggerEffects::AbilityKeyCount];
        STriggerEffect mEffect;

        Node(){
            for (int i = 0; i < CTriggerEffects::AbilityKeyCount; i++)
                children[i]=NULL;
        }

        ~Node(){
            Clear();
        }

        void Clear(){
            for (int i = 0; i < CTriggerEffects::AbilityKeyCount; i++)
                if (children[i]!=NULL){
                    delete children[i];
                    children[i]=NULL;
                }
        }

        void Add(std::string &sequence,int seqIndex, STriggerEffect *te){
            if (seqIndex >= (int)sequence.size()){
                this->mEffect = *te;
            } else {
                int c = StringUtils::CharToCode(sequence[seqIndex]);
                if (children[c] == NULL)
                    children[c] = new Node();
                children[c]->Add(sequence,seqIndex+1,te);
            }
        }

        STriggerEffect *Find(const std::vector<int> &sequence, int seqIndex){
            if (seqIndex >= (int)sequence.size())
                return &mEffect;
            int c = sequence[seqIndex];
            if (children[c] == NULL)
                return NULL;
            return children[c]->Find(sequence,seqIndex+1);
        }
    } mRoot;

public:
    CTriggerKeyMap(){}
    ~CTriggerKeyMap(){}

    STriggerEffect *FindEffect(const std::vector<int> &sequence){
        return mRoot.Find(sequence,0);
    }

    void Add(std::string &sequence, STriggerEffect *te){
        mRoot.Add(sequence,0,te);
    }
};

CTriggerEffects::CTriggerEffects(const std::string &filename, CAnimSet *anims){
    mMap = new CTriggerKeyMap();
    CXml xml(filename,"root");

    if (xml.GetString(xml.GetRootNode(),"type") != "trigger-effects"){
        fprintf(stderr,"WARNING: trying to load file %s as \"trigger-effects\", but its \"%s\". ABORTING\n",filename.c_str(),xml.GetString(xml.GetRootNode(),"type").c_str());
    } else {
        std::string tmp;
        for (xml_node<> *node = xml.GetChild(NULL,"sequence");node;node = xml.GetSibl(node,"sequence")){
            STriggerEffect te;
            tmp = xml.GetString(xml.GetChild(node,"sound"));
            if (tmp != "")
                te.mSound = gResourceManager.GetSound(tmp);
            if (xml.GetChild(node,"effect")!=NULL){
                te.mEffect = gEffectManager.LoadEffect(xml,xml.GetChild(node,"effect"));
            }
            tmp = xml.GetString(xml.GetChild(node,"anim"));
            if (tmp != "")
                te.mAnim = anims->GetAnim(anims->ParseAnimClass(tmp));
            tmp = xml.GetString(node,"trigger");
            if (tmp != "")
                mMap->Add(tmp,&te);
        }
    }
}

CTriggerEffects::STriggerEffect *CTriggerEffects::Find(const std::vector<int> &sequence){
    return mMap->FindEffect(sequence);
}


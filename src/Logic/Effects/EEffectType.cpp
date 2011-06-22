#include "EEffectType.h"
#include "../../Utils/StringUtils.h"

#include <cstdio>

namespace EffectTypes{

    std::wstring effectNames[] = { L"noEffect", L"timed", L"recastable", L"area",
        L"chance", L"graphical-effect", L"list", L"delayed", L"retarget",
        L"non-cumulative", L"check-level", L"ray",
        L"damage", L"heal", L"modify", L"AIState",
        L"restore-mana", L"drain-mana", L"console", L"bullet", L"stun", L"knockback",
        L"linked-graphical-effect", L"jump", L"summon", L"inject", L"sound",
        L"transfer-life", L"transfer-mana", L"annihilate", L"healthcast",
		L"increase-next-ability-level", L"heal-target" };

    const int effectCount = sizeof(effectNames)/sizeof(std::wstring);

    std::wstring &effectName(EffectType e) { return effectNames[(int)e]; }

    EffectType parseEffect(std::wstring &name,const std::string &filename){
        for (int i = 0; i < effectCount; i++)
            if (effectNames[i]==name)
                return (EffectType)(i);
        if (name != L""){
            if (filename != "")
                fprintf(stderr,"WARNING: unknown effect type: %s, in file: %s\n",StringUtils::ConvertToString(name).c_str(),filename.c_str());
            else
                fprintf(stderr,"WARNING: unknown effect type: %s\n",StringUtils::ConvertToString(name).c_str());
        }
        return etNoEffect;
    }

    std::string paramNames[] = { "badparam", "effect", "time",
        "recast", "area", "filter", "chance", "name",
        "amount", "aspect", "text", "emitter", "target-chooser",
		"offset-x", "offset-y", "damage-type", "summoner",
		"tree", "code", "show-amount" };

    const int paramCount = sizeof(paramNames)/sizeof(std::string);

    std::string &paramName(ParamType pt){
        return paramNames[-(int)pt-1];
    }

    /**
     * Lista parametrow dla poszczegolnych efektow;
     * na tej podstawie (run-time) jest budowana tablica paramIndexes
     * kolejne wiersze to rodzaj efektu, parametry efektu.
     * efekty sa rozrozniane od parametrow, bo jedne sa >=0 a drugie <0 ;)
     */
    const int paramList[] = { // subEffect powinien byc zawsze ostatni
        etNoEffect,
        etTimedEffect, ptTime, ptSubEffect,
        etRecastableEffect, ptRecast, ptTime, ptSubEffect,
        etAreaEffect, ptArea, ptFilter, ptSubEffect,
        etChanceEffect, ptChance, ptSubEffect,
        etGraphicalEffect, ptTime, ptName,
        etEffectList, ptSubEffect,
        etDelayedEffect, ptTime, ptSubEffect,
        etRetarget, ptTargetChooser, ptSubEffect,
        etNonCumulative, ptSubEffect,
        etCheckLevel, ptAmount, ptSubEffect,
		etRay, ptArea, ptFilter, ptOffsetX, ptOffsetY, ptSubEffect,
        betDamage, ptAmount, ptDamageType,
        betHeal, ptAmount, ptShowAmount,
        betRestoreMana, ptAmount,
        betDrainMana, ptAmount,
        betModify, ptAspect, ptAmount,
        betConsole, ptText,
        betBullet, ptBulletEmitter,
        betStun,
        betKnockback, ptAmount,
        betLinkedGraphicalEffect, ptOffsetX, ptOffsetY, ptTime, ptName,
		betJump, ptName,
        betSummon, ptSummoner,
		betInject, ptTree, ptCode, ptName,
		betSound, ptName,
        betTransferLife, ptAmount,
        betTransferMana, ptAmount,
		betAnnihilate,
		betIncreaseNextAbilityLevel, ptAmount,
        betHealTarget, ptAmount
    };
    const int paramListSize = sizeof(paramList)/sizeof(int);

    int paramIndexes[effectCount * paramCount];
    inline int &paramIdx(EffectType et, ParamType pt){
        return paramIndexes[et * paramCount - pt - 1];
    }
    // tablica pomocnicza, do informowania uzytkownika o dostepnych parametrach
    std::vector<ParamType> effectParamVectors[effectCount];

    // tablica pomocnicza, do kompilacji efektow - offset statyczny
    int effectTotalOffsets[effectCount];

    int paramOffset(EffectType et, ParamType pt){
        //fprintf(stderr,"paramOffset for %ws,%s is %d\n",effectName(et).c_str(),paramName(pt).c_str(),paramIdx(et,pt));
        return paramIdx(et,pt);
    }

    bool hasParam(EffectType et,ParamType pt){
        return paramIdx(et,pt) >= 0;
    }

    /* budujemy tablice pomocnicze effectParamVectors
     */
    void BuildData(){
        for (int i = 0; i < effectCount * paramCount; i++)
            paramIndexes[i]=-1;
        EffectType et = (EffectType)0;
        ParamType pt = (ParamType)0;
        int offset = 0;
        for (int i = 0; i < paramListSize; i++){
            if (paramList[i] >= 0){
                et = (EffectType)paramList[i];
                offset = 1;
            } else {
                pt = (ParamType)paramList[i];
                paramIdx(et,pt)=offset;
                offset++;
                effectParamVectors[et].push_back(pt);
            }
        }
        for (int i = 0; i < effectCount; i++)
            effectTotalOffsets[i]=0;
        for (int i = 0; i < effectCount; i++)
            for (int j = 0; j < paramCount; j++)
                if (hasParam((EffectType)i,(ParamType)(-j-1)))
                    effectTotalOffsets[i]++;
    }

    std::vector<ParamType> &availableParams(EffectType et){
        return effectParamVectors[et];
    }

    int totalStaticOffset(EffectType et){
        //fprintf(stderr,"totalStaticOffset for %ws is %d\n",effectName(et).c_str(),effectTotalOffsets[et]);
        return effectTotalOffsets[et];
    }

	void PrintOutSyntax(){
		fprintf(stderr,"<effect type=\"list\">\n");
		for (int i = 0; i < paramListSize; i++){
			int v = paramList[i];
			if (v >= 0){ // efekty
				if (v != etEffectList){
					if (i != 0){ // trzeba zamknac poprzedni blok <effect>
						fprintf(stderr,"\t</effect>\n");
					}
					fprintf(stderr,"\t<effect type=\"%ls\">\n",effectName((EffectType)v).c_str());
				}
			} else { // parametry
				if ((v == ptBadParam)
					|| (v == ptBulletEmitter)
					|| (v == ptTargetChooser)
					|| (v == ptSubEffect)
					|| (v == ptSummoner))
					fprintf(stderr,"\t\t<%s ...>\n",paramName((ParamType)v).c_str());
				else
					fprintf(stderr,"\t\t<%s value=\"...\"/>\n",paramName((ParamType)v).c_str());
			}
		}
		fprintf(stderr,"\t</effect>\n");
		fprintf(stderr,"</effect>\n");
	}
}


#include "CExprParser.h"

#include "../../Utils/muParser/muParser.h"
#include "../Abilities/CPinnedAbilityBatch.h"
#include <cassert>
#include <cstdio>
#include <set>
#include <vector>
#include <algorithm>

using namespace mu;

const std::string lvlStr = "lvl";
const std::string intStr = "int";
const std::string strStr = "str";
const std::string wpStr = "wp";

double sNull = 0.0;
double sLevel = 0.0;
double sIntelligence = 0.0;
double sWillpower = 0.0;
double sStrength = 0.0;

double* AddVariable(const char *a_szName, void *pUserData)
{
  return &sNull;
}


CExprParser::CExprParser(const std::string& expr){
	mPAB = NULL;
	mParser = new Parser();
	try{
		mParser->SetVarFactory(AddVariable, NULL);
		mParser->DefineVar(lvlStr,&sLevel);
		mParser->DefineVar(intStr,&sIntelligence);
		mParser->DefineVar(strStr,&sStrength);
		mParser->DefineVar(wpStr,&sWillpower);
		mParser->SetExpr(expr);
		// testujemy...
		mParser->Eval();
	} catch (Parser::exception_type &e){
		fprintf(stderr,"error - parsing of expresion %s failed: %s\n",expr.c_str(),e.GetMsg().c_str());
		mParser = NULL;
	}
}

CExprParser::~CExprParser(){
	if(mParser)
		delete mParser;
}

float CExprParser::Evaluate(ExecutionContextPtr context){
	if (mParser){
		if (context != NULL){
			sLevel = context->values[aLevel];
			sWillpower = context->values[aWillpower];
			sIntelligence = context->values[aIntelligence];
			sStrength = context->values[aStrength];
			CPinnedAbilityBatch *pab = context->GetPinnedAbilityBatch();
			if (pab != mPAB){
				mPAB = pab;
			}
			if (mPAB != NULL) {
				Rebind();
			}
		} else {
			sLevel = 0.0;
			sWillpower = 0.0;
			sIntelligence = 0.0;
			sStrength = 0.0;
		}
		
		return (float)(mParser->Eval());
	} else
		return 0.0f;
}

void CExprParser::Rebind(){
	static int counter = 0;
	counter++;
	if (counter % 10000 == 0){
		fprintf(stderr,"CExprParser::Rebind() performed total %d times\n",counter);
	}
	const std::vector<CPinnedAbilityBatch::SExportedAbility> &exported = mPAB->GetPinnedAbilities();
	for (unsigned int i = 0; i < exported.size(); i++){
		if (exported[i].exportName != ""){
			mParser->DefineVar(exported[i].exportName, (double*)&(exported[i].level));
		}
	}
}


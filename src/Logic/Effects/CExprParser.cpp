#include "CExprParser.h"

#include "../../Utils/muParser/muParser.h"
#include "../Abilities/CPinnedAbilityBatch.h"
#include <cassert>
#include <cstdio>
#include <set>
#include <vector>
#include <algorithm>

using namespace mu;

double sNull = 0.0;
double sLevel = 0.0;
double sIntelligence = 0.0;
double sPoisonous = 0.0;
double sStrength = 0.0;
double sPower = 0.0;

double* AddVariable(const char *a_szName, void *pUserData)
{
  return &sNull;
}

// Function callback
value_type dot(value_type ax, value_type bx, value_type ay, value_type by, value_type az, value_type bz, value_type av, value_type bv) 
{ 
//  <!-- power = 5x cho, 1x mel, 2x san, 0x phl -->
//  <power calc="0.2*((5*choleric+1*melancholic+2*sanguine+0*phlegmatic)/sqrt(5^2 + 1^2 + 2^2 + 0^2))*(((5*choleric+1*melancholic+2*sanguine+0*phlegmatic)/(sqrt(5^2+1^2+2^2+0^2)*sqrt(max(1,choleric^2+melancholic^2+sanguine^2+phlegmatic^2))))^8)"/>
    double a_dot_b = (ax * bx) + (ay * by) + (az * bz) + (av * bv);
    double a_sq_length = (ax * ax) + (ay * ay) + (az * az) + (av * av);
    double a_length = a_sq_length <= 0.0 ? 1 : sqrt(a_sq_length);
    double b_sq_length = (bx * bx) + (by * by) + (bz * bz) + (bv * bv);
    double b_length = b_sq_length <= 0.0 ? 1 : sqrt(b_sq_length);
    double cos_ab = a_dot_b / (a_length * b_length);
    double b_projected_on_a = a_dot_b / a_length;
    double ret = b_projected_on_a * pow(cos_ab, 8);

    return ret; 
}

CExprParser::CExprParser(const std::string& expr){
	mPAB = NULL;
	mParser = new Parser();
	try{
		mParser->SetVarFactory(AddVariable, NULL);
		mParser->DefineVar("level",&sLevel);
		mParser->DefineVar("intelligence",&sIntelligence);
		mParser->DefineVar("strength",&sStrength);
		mParser->DefineVar("poisonous",&sPoisonous);
		mParser->DefineVar("power",&sPower);
		mParser->DefineFun("dot", dot, false);
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
			sPoisonous = context->values[aPoisonous];
			sIntelligence = context->values[aIntelligence];
			sStrength = context->values[aStrength];
			sPower = context->abilityPower;
			CPinnedAbilityBatch *pab = context->GetPinnedAbilityBatch();
			if (pab != mPAB){
				mPAB = pab;
			}
			if (mPAB != NULL) {
				Rebind();
			}
		} else {
			sLevel = 0.0;
			sPoisonous = 0.0;
			sIntelligence = 0.0;
			sStrength = 0.0;
			sPower = 0.0;
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


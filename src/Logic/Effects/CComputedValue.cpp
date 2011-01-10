#include "CComputedValue.h"
#include "CExprParser.h"
#include "../Stats/CRPGCalc.h"
#include "../Stats/CAspectBatch.h"

CComputedValue::CComputedValue(): 
mValue(0.0f), 
mModifiers(0), 
mLinearGain(0.0f), 
mSquareGain(0.0f), 
mParser(NULL){}

CComputedValue::CComputedValue(const std::string& expr):
mValue(0.0f), mModifiers(0),mLinearGain(0.0f),mSquareGain(0.0f){
	mParser = new CExprParser(expr);
}

CComputedValue::CComputedValue(float value, int modifiers, float linearGain, float squareGain):
mValue(value), mModifiers(modifiers), mLinearGain(linearGain), mSquareGain(squareGain), mParser(NULL){}

CComputedValue::~CComputedValue(){
	// nie moge zniszczyc tego parsera, bo on moze byc wspoldzielony na kilka obiektow
	// wszystko przez to, ze zachcialo mi sie przekazywac obiekt by-value
	// TODO: zliczanie referencji?
	//if (mParser) delete mParser;
}

float CComputedValue::Evaluate(ExecutionContextPtr context) const{
	if (mParser)
		return mParser->Evaluate(context);
	else {
		if (context == NULL){
			CAspectBatch tmpValues;
			return gRPGCalc.CalculateModifiedValue(mValue,tmpValues,0);
		}
		float v = mValue;
		float lvl = context->values[aLevel];
		v += lvl * mLinearGain + lvl * lvl * mSquareGain;
        return gRPGCalc.CalculateModifiedValue(v, context->values,mModifiers);
	}
}


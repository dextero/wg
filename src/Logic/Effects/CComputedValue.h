#ifndef __CCOMPUTED_VALUE_H__
#define __CCOMPUTED_VALUE_H__

// floatowy parametr, ktory moze byc zmieniany modyfikatorami lub obliczany ze sparsowanego wyrazenia

#include "CExecutionContext.h"

class CExprParser;

class CComputedValue{
private:
	float mValue;

    int mModifiers;
	float mLinearGain;
	float mSquareGain;

	CExprParser *mParser;
public:
	CComputedValue();
	CComputedValue(const std::string& expr);
	CComputedValue(float value, int modifiers = 0, float linearGain = 0.0f, float squareGain = 0.0f);
	~CComputedValue();

	float Evaluate(ExecutionContextPtr context) const;
};

#endif /*__CCOMPUTED_VALUE_H__*/

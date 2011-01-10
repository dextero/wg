#ifndef __CCOMPUTEDVALUEFACTORY_H__
#define __CCOMPUTEDVALUEFACTORY_H__

// wczytywacz obiektow computedvalue

#include "CComputedValue.h"
#include "../../Utils/CXml.h"
#include "../../Utils/StringUtils.h"

class CComputedValueFactory{
public:
	inline static CComputedValue Parse(CXml &xml, rapidxml::xml_node<> *n, float def = 0.0f){
		CComputedValue result(def);
		if (n) {
			if (n->first_attribute("value")!=NULL){
				float v = xml.GetFloat(n,"value");
				int modifiers = parseModifiers(StringUtils::ConvertToWString(xml.GetString(n,"modifiers")));
				float linearGain = xml.GetFloat(n,"linear-gain",0.0f);
				float squareGain = xml.GetFloat(n,"square-gain",0.0f);
				result = CComputedValue(v,modifiers,linearGain,squareGain);
			}else if (n->first_attribute("calc")!=NULL){
				result = CComputedValue(xml.GetString(n,"calc"));
			} else 
				result = CComputedValue(def);
		}
		return result;
	}
};

#endif /*__CCOMPUTEDVALUEFACTORY_H__*/

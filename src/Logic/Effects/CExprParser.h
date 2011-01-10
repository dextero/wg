#ifndef __CEXPRPARSER_H__
#define __CEXPRPARSER_H__

// wrapper na parser matematyczny dedykowany do efektow logicznych

#include "CExecutionContext.h"

class CPinnedAbilityBatch;

namespace mu {
	class Parser;
}

class CExprParser{
private:
	mu::Parser *mParser;

	CPinnedAbilityBatch *mPAB;

	void Rebind();
public:
	CExprParser(const std::string& expr);
	~CExprParser();

	float Evaluate(ExecutionContextPtr context);
};

#endif /*__CEXPRPARSER_H__*/

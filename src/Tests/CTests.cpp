#include "CTests.h"
#include <cstdio>

#include "../Utils/Memory/MemoryTests.h"
#include "../Utils/CUtilsTests.h"

void CTests::RunAll(){
	fprintf(stderr,"Running all tests\n");
	Memory::Tests::RunAll();
	CUtilsTests::RunAll();
}

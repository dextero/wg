#include "RefCntPtr.h"
#include "CPoolable.h"
#include "CRefPoolable.h"
#include "CPool.h"
#include "CSafePtrable.h"
#include <cstdio>
#include <cassert>
#include <vector>

using namespace Memory;

class Test{
private:
    int cnt;
public:
    Test(): cnt(0){
        fprintf(stderr,"Test()\n");
    }
    ~Test(){
        fprintf(stderr,"~Test()\n");
    }
    inline int &GetRefCount(){
        return cnt;
    }
    inline void Release(){
        delete this;
    }
};

class PoolTest: public CRefPoolable{
public:
    static int cAllocs;
    int a;
    int b;
    int *c;
    PoolTest(){
        //fprintf(stderr,"CPoolTest() at %p\n",this);
        a = 13;
        b = 15;
        cAllocs++;
        c = new int[15];
        for (int i = 0; i < 15; i++)
            c[i] = i;
    }
    PoolTest(const PoolTest& other): a(other.a), b(other.b){
        cAllocs++;
        c = new int[15];
        for (int i = 0; i < 15; i++)
            c[i] = other.c[i];
    }
    virtual ~PoolTest(){
        //fprintf(stderr,"~CPoolTest() at %p\n",this);
        cAllocs--;
        delete[] c;
    }

    void CheckOk(){
        assert(a = 13);
        assert(b = 15);
        assert(c!=NULL);
        for (int i = 0; i < 15; i++)
            assert(c[i] == i);
    }
};

int PoolTest::cAllocs = 0;

typedef CPool<PoolTest> CTestPool;

void PoolTestInterleaved(CTestPool &pool, int cnt){
    std::vector<PoolTest *> tests;
    for (int i = 0; i < cnt; i++)
        tests.push_back(pool.Alloc());
    for (int i = 0; i < cnt; i++)
        tests[i]->CheckOk();
    for (int i = 0; i < cnt; i+=2)
        pool.Dealloc(tests[i]);
    for (int i = 1; i < cnt; i+=2)
        tests[i]->CheckOk();
    for (int i = 1; i < cnt; i+=2)
        pool.Dealloc(tests[i]);
}

void PoolTest1(){
    fprintf(stderr,"Memory::Tests: running CPool test 1 (single allocation) ...");
    CTestPool pool;
    PoolTest *x = pool.Alloc();
    x->CheckOk();
    pool.Dealloc(x);
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void PoolTest2(){
    fprintf(stderr,"Memory::Tests: running CPool test 2 (interleaved usage, 50 objects) ...");
    CTestPool pool;
    PoolTestInterleaved(pool,50);
    PoolTestInterleaved(pool,50);
    PoolTestInterleaved(pool,50);
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void PoolTest3(){
    fprintf(stderr,"Memory::Tests: running CPool test 3 (10 objects allocation, size-4 blocks) ...");
    CPool<PoolTest,4> pool;
    std::vector<PoolTest *> tests;
    for (int i = 0; i < 10; i++)
        tests.push_back(pool.Alloc());
    for (int i = 0; i < 10; i++)
        tests[i]->CheckOk();
    for (int i = 0; i < 10; i++)
        pool.Dealloc(tests[i]);
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void PoolTest4(){
    fprintf(stderr,"Memory::Tests: running CPool test 4 (escalating interleaved usage, up to 5k objects)...");
    CTestPool pool;
    PoolTestInterleaved(pool,1000);
    PoolTestInterleaved(pool,2000);
    PoolTestInterleaved(pool,3000);
    PoolTestInterleaved(pool,4000);
    PoolTestInterleaved(pool,5000);
    assert(PoolTest::cAllocs == 0);
    assert((int)(pool.CurrentCapacity()) == pool.BlockSize()*5);
    fprintf(stderr,"passed!\n");
}

void PoolTest5(){
    fprintf(stderr,"Memory::Tests: running CPool test 5 (massive usage - 100k objects)...");
    CTestPool pool;
    PoolTestInterleaved(pool,100000);
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

typedef RefCntPtr<PoolTest> testptr_t;

void RefTest1(){
    fprintf(stderr,"Memory::Tests: running RefCntPtr test 1 (single pointer, 10x)...");
    for (int i = 0; i < 10; i++){
        testptr_t t(new PoolTest());
        t->CheckOk();
    }
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void RefTest2(){
    fprintf(stderr,"Memory::Tests: running RefCntPtr test 2 (multiple pointers, 10x)...");
    for (int i = 0; i < 10; i++){
        std::vector<testptr_t> ptrs;
        testptr_t *t = new testptr_t(new PoolTest());
        (*t)->CheckOk();
        for (int j = 0; j < 20; j++){
            testptr_t tmp = new PoolTest();
            ptrs.push_back(tmp);
        }
        delete t;
    }
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void RefTest3(){
    fprintf(stderr,"Memory::Tests: running RefCntPtr test 3 (pointers to NULL, 10x)...");
    for (int i = 0; i < 10; i++){
        testptr_t t(NULL);
    }
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void RefTest4(){
    fprintf(stderr,"Memory::Tests: running RefCntPtr test 4 (resource copying, 10x)...");
    for (int i = 0; i < 10; i++){
        testptr_t t(new PoolTest());
        testptr_t t2(new PoolTest(*t));
        testptr_t t3(new PoolTest(*t2));
        testptr_t t4(t3);
        testptr_t t5(new PoolTest(*t2));
        testptr_t t6(t5);
    }
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

void RefPoolTest1(){
    fprintf(stderr,"Memory::Tests: running RefCntPtr+CPool test 1 (basic usage, 10x)...");
    CTestPool pool;
    for (int i = 0; i < 10; i++){
        testptr_t t(pool.Alloc());
        testptr_t t2(t);
        testptr_t t3(t);
        testptr_t t4(pool.Alloc());
    }
    assert(PoolTest::cAllocs == 0);
    fprintf(stderr,"passed!\n");
}

class SPFoo : public CSafePtrable<SPFoo>{
public:
	int b;
	SPFoo():b(17){}
};

void SafePtrTest1(){
    fprintf(stderr,"Memory::Tests: running SafePtr test 1 (basic usage)...");
	SPFoo *foo = new SPFoo();
	CSafePtr<SPFoo> test = foo->GetSafePointer();
	assert(test == foo);

	foo = new SPFoo();
	test = foo->GetSafePointer();
	assert(test == foo);
	test.DeleteObject();
	assert(test == NULL);
	
	foo = new SPFoo();
	test = foo->GetSafePointer();
	delete foo;
	assert(test == NULL);
    fprintf(stderr,"passed!\n");
}

void SafePtrTest2(){
    fprintf(stderr,"Memory::Tests: running SafePtr test 2 (basic usage - 10 pointers in vector)...");
	SPFoo *foo = new SPFoo();
	std::vector<CSafePtr<SPFoo> > tests;
	tests.push_back(foo->GetSafePointer());
	delete foo;
	for (unsigned int i = 0; i < tests.size(); i++)
		assert(tests[i] == NULL);
	
	fprintf(stderr,"passed!\n");
}

void SafePtrTestFunc(CSafePtr<SPFoo> foo){
	assert(foo != NULL);
	foo.DeleteObject();
	assert(foo == NULL);
}

void SafePtrTest3(){
    fprintf(stderr,"Memory::Tests: running SafePtr test 3 (passing to function)...");
	CSafePtr<SPFoo> foo = (new SPFoo())->GetSafePointer();
	assert(foo != NULL);
	SafePtrTestFunc(foo);
	assert(foo == NULL);
	fprintf(stderr,"passed!\n");
}

namespace Memory{
    namespace Tests{
        void RunAll(){
            PoolTest1();
            PoolTest2();
            PoolTest3();
            PoolTest4();
            //PoolTest5();
            RefTest1();
            RefTest2();
            RefTest3();
            RefTest4();
            RefPoolTest1();
			SafePtrTest1();
			SafePtrTest2();
			SafePtrTest3();
        }
    }
}

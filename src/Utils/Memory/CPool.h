#ifndef __CPOOL_H__
#define __CPOOL_H__

/**
 * Liosan, 23.06
 * Klasa implementujaca pule obiektow.
 * Wymagania co T: dziedziczy po CPoolable (lub udostepnia jego interfejs), czyli:
 * - konstruktor bezparametrowy, bedzie wykorzystany przez placement new
 * - destruktor - bedzie wykorzystany jawnie
 * - metoda Bind(CPool<T> *) - potrzebna, jesli obiekt ma sam sie niszczyc
 * Wszelka dalsza inicjalizacja musi byc wykonana po stronie uzytkownika.
 *
 * Mozna tez dziedzic po CRefPoolable, jesli porzadany jest mechanizm
 * zliczania referencji.
 *
 * Sugeruje, by blockSize byl potega dwojki, to kompilator zoptymalizuje
 * operacje modulo - a sa one powszechne. Proponuje go rowniez dobrac rozsadnie
 * - czas dzialania rosnie liniowo z liczba blokow, zuzycie pamieci rosnie liniowo
 * z wielkoscia bloku. Domyslna wartosc to 1024.
 */

#include <vector>
#include <cstdio>
#include <cassert>
namespace Memory{

class CPoolBase{ // bo nie wszystko wejdzie w template...
public:
    CPoolBase(){}
    virtual ~CPoolBase(){}

    virtual void DeallocMe(void *obj) = 0;
};

template<class T,int blockSize = 1024>
class CPool: public CPoolBase{
private:
    /* dwa rodzaje indeksowania:
     * list (ktory blok pamieci w mVector) i index (indeks w tym bloku)
     * node/link (blockSize*numer_bloku + indeks w bloku)
     */
    struct obj_t{
    public:
        char data[sizeof(T)/sizeof(char)];
    };
    // wektor blokow pamieci
    std::vector<obj_t*> mVector;
    // pierwszy i ostatni element
    unsigned int mFirst,mLast;
    
    inline bool GoodNode(unsigned int list, unsigned int index){
        return GoodNode(list*blockSize+index);
    }
    inline bool GoodNode(unsigned int node){
        return (node < CurrentCapacity()) && (node >= 0);
    }

    inline void Advance(){
        if (mFirst >= CurrentCapacity()){
            AllocBlock();
            //PrintWholeLinkage();
        }
        assert(GoodNode(mFirst));
        mFirst = GetLink(mFirst);
    }
    inline int GetLink(unsigned int list, unsigned int index){
        assert(GoodNode(list,index));
        int result = *((int*)(mVector[list]+index));
        return result;
    }
    inline int GetLink(unsigned int node){
        return GetLink(node/blockSize,node%blockSize);
    }
    inline void SetLink(unsigned int list, unsigned int index, unsigned int link){
        assert(GoodNode(list,index));
        *((int*)(mVector[list]+index))=link;
    }
    inline void SetLink(unsigned int node, unsigned int link){
        SetLink(node/blockSize,node%blockSize,link);
    }
    inline void AllocBlock(){
        mVector.push_back(new obj_t[blockSize]());
        unsigned int list = (int)mVector.size()-1;
        for (unsigned int i = 0; i < blockSize; i++)
            SetLink(list,i,list*blockSize+i+1);
        assert(GoodNode(mLast));
        if (mLast!=0)
            SetLink(mLast,((int)mVector.size()-1)*blockSize);
        mLast = CurrentCapacity()-1;
    }
    inline void Reuse(unsigned int list, unsigned int index){
        assert(GoodNode(list,index));
        assert(GoodNode(mLast));
        SetLink(list,index,GetLink(mLast));
        SetLink(mLast,list*blockSize+index);
        mLast = list*blockSize+index;
    }
    inline void PrintWholeLinkage(){
        fprintf(stderr,"linkage: ");
        unsigned int node = mFirst;
        while(node != mLast){
            fprintf(stderr,"%u, ",node);
            node = GetLink(node);
        }
        fprintf(stderr,"%u.\n",mLast);
    }
public:
    CPool(): mFirst(0), mLast(0){
        assert(sizeof(obj_t) == sizeof(T));
        AllocBlock();
    }
    virtual ~CPool(){
        for (unsigned int i = 0; i < mVector.size(); i++)
            delete mVector[i];
    }
    inline T* Alloc(){
        // kluczowa jest kolejnosc: 
        // najpierw zapamietujemy wartosci z mFirst
        unsigned int list = mFirst/blockSize, index = mFirst%blockSize;
        // jesli zuzylismy ostatni kawalek pamieci, trzeba go "odlaczyc"
        if (mFirst == mLast)
            mLast = 0;
        // potem przesuwamy mFirst
        Advance();
        // potem nadpisujemy tam pamiec
        T *obj = new((T*)(mVector[list]+index)) T(); // ... placement new :)
        obj->Bind(this);
        // i wreszcie zwracamy to, co zainicjowalismy
        //fprintf(stderr,"alocated %p\n",obj);
        return obj;
    }
    inline void Dealloc(T* obj){
        assert(obj!=NULL);
        for (unsigned int i = 0; i < mVector.size(); i++){
            // ack! arytmetyka wskaznikow ;) ale coz poradze...
            // note to self: nie dzielic przez sizeof
            int diff = (obj_t*)obj - mVector[i]; 
            if ((diff >= 0) && (diff < blockSize)){
                //fprintf(stderr,"deallocating %p at mVector[%u][%d]==%p\n",obj,i,diff,mVector[i]+diff);
                obj->~T(); // ... jawne wywolanie destruktora :)
                Reuse(i,diff);
                //PrintWholeLinkage();
                return;
            }
        }
    }
    virtual void DeallocMe(void *obj){
        Dealloc((T*)obj);
    }
    inline unsigned int CurrentCapacity(){
        return (int)mVector.size() * blockSize;
    }
    inline int BlockSize(){
        return blockSize;
    }
};

}

#endif

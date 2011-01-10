#ifndef __CCell_h__
#define __CCell_h__

struct CCell
{
public:
	CCell();

    bool collision;
    int density;    
    int regionId;
};

#endif // __CField_h__

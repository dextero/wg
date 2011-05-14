#ifndef _CITEM_H_
#define _CITEM_H_

// klasa do przechowywania obiektow w plecaku bohatera

#include <string>

class CItem
{
    public:
        CItem();
        ~CItem();
        const std::string & GetAbility();
        void SetAbility(const std::string & ability);

        size_t mInvPos;
    private:
        std::string mAbility;
};

#endif // _CITEM_H_
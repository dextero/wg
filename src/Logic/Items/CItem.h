// klasa do przechowywania obiektow w plecaku bohatera

#include <string>

class CItem
{
    public:
        CItem();
        ~CItem();
        const std::string & GetAbility();
        void SetAbility(const std::string & ability);
    private:
        std::string mAbility;
};

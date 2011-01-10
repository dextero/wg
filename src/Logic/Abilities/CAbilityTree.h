#ifndef __CABILITYTREE_H__
#define __CABILITYTREE_H__

/**
 * Liosan, 07.07:
 * klasa przechowujaca dostepne dla gracza umiejetnosci,
 * ich zaleznosci i wymagania
 */

#include "../../Utils/CXml.h"
#include <vector>
#include <string>

class CAbility;

struct SAbilityNode{
    std::string code;
	std::string abilityFilename;
    CAbility *ability;
    int startLevel;
    int minPlayerLevel;
    std::vector<std::pair<int, int> > preqs;    // pierwszy int = numer wymaganej umiejki w drzewie; drugi - potrzebny level
	bool sharedTrigger;
    int animCode;
    int row;
	bool doExport;
	std::string exportName;

    SAbilityNode(): ability(NULL), startLevel(0), minPlayerLevel(0), sharedTrigger(false),
		animCode(-1),row(-1),doExport(false),exportName(""){}
};

// mozna przerobic na zasob (IResource), gdy ktos bedzie potrzebowal
// np. gdy bedzie kilku graczy, ktorzy chcieliby wspoldzielic drzewo (z jakiegos powodu...)

class CAbilityTree{
private:
    std::vector<SAbilityNode> mAbilityNodes;
    std::wstring mName, mBackground;
    
    int FindByCode(const std::string &code);

	int mRowCount;
public:
    CAbilityTree();
    ~CAbilityTree();

    inline const std::vector<SAbilityNode> &GetAbilityNodes() const { return mAbilityNodes; }
    inline const std::wstring &GetName() const { return mName; }
    inline const std::wstring &GetBackground() const { return mBackground; }

    void Load(const std::string &filename);
    void Load(CXml &xml, rapidxml::xml_node<> *node = NULL);

    int FindAbility(const std::string &code);

	inline int GetRowCount(){
		return mRowCount;
	}
};

#endif /* __CABILITYTREE_H__ */


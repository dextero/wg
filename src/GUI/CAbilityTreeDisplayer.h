/* GUI - pojedyncze drzewko umiejetnosci */

#ifndef _CABILITYTREEDISPLAER_H_
#define _CABILITYTREEDISPLAER_H_

#include "CGUIObject.h"
#include <string>
#include <vector>

class CHudSprite;
class CAbilityTree;
class CPlayer;
struct SAbilityNode;

namespace System{
    namespace Resource{
        class CImage;
    }
}

namespace GUI
{
    class CWindow;
    class CImageBox;
    class CButton;
    class CTextArea;
    class CAbilitySlot;

	class CAbilityTreeDisplayer : public CGUIObject
	{
	public:
        void SetDependencyLineImage(const std::string &filename);
        void SetHolderImage(const std::string &filename, float padding);
		inline void SetActiveHolderImage(const std::string &filename){
			mHolderActiveImage = filename;
		};
		inline void SetPotentialHolderImage(const std::string &filename){
			mHolderPotentialImage = filename;
		};
        inline void SetLvlTextFont(const std::string &font,float size){
            mLvlTextFont=font;
            mLvlTextFontSize=size;
        }
		inline void ForceReload(){ mForceReload = true; }
	private:
        static const int ROW_COUNT = 5;
        static const float HOLDER_SIZE;
        static const float DEPLINE_WIDTH;

		friend class CGUIObject;

        bool mForceReload;

		CAbilityTreeDisplayer( const std::string& name, CGUIObject* parent, unsigned int zindex, int player, const std::string &tree );
		virtual ~CAbilityTreeDisplayer();
		virtual void UpdateSprites( float secondsPassed );

        const std::string mTreeName;
        CAbilityTree *mTree;
        int mPlayer;
        System::Resource::CImage *mDepLineImage;
        std::string mHolderImage;
        std::string mHolderActiveImage;
        std::string mHolderPotentialImage;
        std::string mLvlTextFont;
        float mLvlTextFontSize;
        float mHolderPadding;

        struct CAbilityNodeMirror{
            const SAbilityNode *abiNode;
            CTextArea *lvlText; // powinien byc dzieckiem CAbilityTreeDisplayera
            CImageBox *holder; // powinien byc dzieckiem CAbilityTreeDisplayera
            CButton *button; // powinien byc dzieckiem holdera
            std::vector<CHudSprite*> depLines; // rysowane recznie

            CAbilityNodeMirror(const SAbilityNode *an): 
                abiNode(an),lvlText(NULL),holder(NULL),button(NULL),leveled(0),canBuy(false){}
            virtual ~CAbilityNodeMirror(){}
            void Release(); // zasoby GUI-owe zwalniane recznie

            void AddAbilityCallback();
            void GetAbilityAddress(int &outTreeIdx, int &outAbiIdx);
	
			// cache'owane level i kupowalnosci
			int leveled;
			bool canBuy;
        };

        void LoadAbilityNode(const SAbilityNode &an);
        std::vector<CAbilityNodeMirror> mNodes;

        // indeksy CAbilityNodeMirrorow w poszczegolnych wierszach
        std::vector< std::vector<int> > mRows;

        // ramka
        CImageBox* mHoveredAbiBorder;

        friend class CAbilitySlot;
	};
};

#endif// _CABILITYTREEDISPLAER_H_


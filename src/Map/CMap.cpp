#include <cstdio>
#include <cstring>
#include <sstream>
#include <errno.h>

#include "CMap.h"
#include "../CGame.h"
#include "../ResourceManager/CResourceManager.h"
#include "../Rendering/CDisplayable.h"
#include "../Rendering/CDrawableManager.h"
#include "../Rendering/ZIndexVals.h"
#include "../Console/CConsole.h"
#include "../Utils/StringUtils.h"
#include "../Utils/CXml.h"
#include "../Logic/Factory/CPhysicalTemplate.h"
#include "SceneManager/CSceneNode.h"
#include "SceneManager/CQuadTreeSceneManager.h"
#include "CMapManager.h"
#include "CTile.h"
#include "CMapObjectDescriptor.h"
#include "CDoodahDescriptor.h"
#include "CRegionDescriptor.h"
#include "../Logic/MapObjects/CRegion.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>

#include <boost/filesystem.hpp>

namespace Map{

    class CMapTileType{
    public:
        std::string code;
        std::string img;
        int number; // indeks w atlasie lub -1 -> caly obrazek

        CMapTileType():  number(-1) {}
    };

	class CMapObjectType{
	public:
		std::string code;
		CPhysicalTemplate *templ;

		CMapObjectType(): templ(NULL) {}
	};

	CMap::CMap(): mMapHeader(NULL), mViewRectangle(0,0,0,0), mMaxLivingMonsters(MAX_MONSTERS_DEFAULT)
	{
		fprintf( stderr, "CMap::CMap()\n" );
	}

	CMap::~CMap()
	{
		fprintf( stderr, "CMap::~CMap()\n" );
	}

	sf::Vector2<int> CMap::GetSize(){
		return m_size;
	}

	std::vector<CTile*> * CMap::GetMapTiles(){
		return mFields;
        // TODO: dopisac czyszczenie wektorow
	}

	/* IResource members */
	// ====================
	std::string CMap::GetType()
	{
		return "CMap";
	};

	bool CMap::Load(std::map<std::string,std::string>& argv)
	{
        return LoadFromXML(argv["name"]);
	}

	void CMap::Drop()
	{
        fprintf( stderr, "CMap::Drop()\n" );
		size_t tileCount = mFields->size();
		for(size_t i=0;i<tileCount;i++){
			delete (*mFields)[i];
			(*mFields)[i]=NULL;
		}
		mFields->clear();
		delete mFields;
		delete mMapHeader;

        ClearTiles();
		ClearMapObjects();
		ClearDoodahs();

		delete this;
	}

	//======================

	/* Get / Set functions */
	//======================

	int CMap::GetVersion(){
		return mMapHeader->Version;
	}

	const std::string & CMap::GetFilename(){
		return mMapFilename;
	}

    sf::Vector2f CMap::GetExitPos()
    {
        for (std::vector<CRegionDescriptor*>::iterator i = mRegionDescriptors.begin();
             i != mRegionDescriptors.end(); ++i)
            if ((*i)->nextMap != "")
                return (*i)->pos;
        return sf::Vector2f (0.0f,0.0f);
    };

    
    void CMap::ShowInvisibleWalls(bool show)
    {
        for (std::vector<CQuadTreeSceneNode>::iterator i = gScene.GetQuadTreeNodes().begin(); i != gScene.GetQuadTreeNodes().end(); ++i)
            for (std::vector<CSceneNode*>::iterator it = i->GetNonMovingPhysicals()->begin(); it != i->GetNonMovingPhysicals()->end(); ++it)
            {
                CPhysical* phys = (*it)->GetPhysical();
                if (phys)
                {
                    if (phys->GetTemplate())
                        if (phys->GetTemplate()->GetFilename().find("invisible-wall") != std::string::npos)
                        {
                            // znalezlismy invisible-walla, upewniamy sie, ze ma on obrazek
                            if (!phys->GetDisplayable())
                                phys->SetImage("data/physicals/walls/invisible-wall.png");
                            if (phys->GetDisplayable())
                                phys->GetDisplayable()->SetVisible(show);
                        }
                }
            }
    }

    bool CMap::Resize(int x, int y, const std::string& defaultTile)
    {
        if (x <= 0 || y <= 0)
        {
            // jakis wtf-rozmiar mapy
            fprintf(stderr, "ERROR: Invalid new map size: %d x %d (CMap::Resize())\n", x, y);
            return false; 
        }

        // przestawiamy wszystkie kafle poza zasieg wzroku
        sf::FloatRect rect(0.f, 0.f, 0.f, 0.f);
        CullVisibility(rect);

        if (x < m_size.x || y < m_size.y)
        {
            // usuwanie obiektow poza obszarem (0, 0)-(x, y)
            // regiony
            for (size_t i = 0; i < mRegionDescriptors.size(); ++i)
                if (mRegionDescriptors[i]->pos.x >= (float)x || mRegionDescriptors[i]->pos.y >= (float)y)
                {
                    mRegionDescriptors.erase(mRegionDescriptors.begin() + i);
                    --i;
                }
            // doodahy
            for (size_t i = 0; i < mDoodahDescriptors.size(); ++i)
                if (mDoodahDescriptors[i]->x >= (float)x || mDoodahDescriptors[i]->y >= (float)y)
                {
                    mDoodahDescriptors.erase(mDoodahDescriptors.begin() + i);
                    --i;
                }
            // obiekty
            for (size_t i = 0; i < mMapObjectDescriptors.size(); ++i)
                if (mMapObjectDescriptors[i]->x >= (float)x || mMapObjectDescriptors[i]->y >= (float)y)
                {
                    mMapObjectDescriptors.erase(mMapObjectDescriptors.begin() + i);
                    --i;
                }

            // kafle
            size_t eraseFromRow = 0, eraseRows = 0;
            if (x < m_size.x)
                eraseFromRow = (size_t)(m_size.x - x);
            if (y < m_size.y)
                eraseRows = (size_t)(m_size.y - y);

            // usun ostatnie "wiersze" kafli
            if (eraseRows > 0)
            {
                mFields->erase(mFields->begin() + (size_t)y * (size_t)m_size.x, mFields->end());

                m_size.y = y;
                mMapHeader->Height = y;
            }
            // usun ostatnie "kolumny" kafli
            if (eraseFromRow > 0)
            {
                for (size_t i = x; i < mFields->size(); i += x)
                    mFields->erase(mFields->begin() + i, mFields->begin() + i + eraseFromRow);

                m_size.x = x;
                mMapHeader->Width = x;
            }
        }

        if (x > m_size.x || y > m_size.y)
        {
            // powiekszanie mapy, trzeba dodac kafle - jakie?
            std::string tileCode;
            if (defaultTile == "")
            {
                if (!mTileTypes.size() || !mTileTypes[0])
                {
                    // nie ma zadnego typu kafla? ;x
                    fprintf(stderr, "ERROR: couldn't find any tile type! (CMap::Resize())\n");
                    return false;
                }
                else
                    tileCode = mTileTypes[0]->code;
            }
            else
                tileCode = GetOrCreateTileCode(defaultTile);


            size_t addToRow = 0, addRows = 0;
            if (x > m_size.x)
                addToRow = x - m_size.x;
            if (y > m_size.y)
                addRows = y - m_size.y;

            // dodaj "kolumny"
            if (addToRow > 0)
            {
                // zaklepanie miejsca z gory
                mFields->reserve(mFields->size() + m_size.y * x);
                for (size_t i = m_size.x; i < mFields->size(); i += x)
                    for (size_t j = 0; j < addToRow; ++j)
                        mFields->insert(mFields->begin() + i + j, new CTile(tileCode));

                m_size.x = x;
                mMapHeader->Width = x;
            }

            // dodaj "wiersze"
            if (addRows > 0)
            {
                // zaklepanie miejsca z gory
                mFields->reserve(mFields->size() + addRows * y);
                for (size_t i = 0; i < addRows * y; ++i)
                    mFields->push_back(new CTile(tileCode));

                m_size.y = y;
                mMapHeader->Height = y;
            }
        }

        return true;
    }


	//========================

	/* Load  */
	//======================

    bool CMap::LoadFromXML(const std::string &filename){

        mMapFilename = filename;

        CXml		xml( filename, "map" );
        xml_node<>* node;
		std::string str;
        mFields = new std::vector<CTile*>();

        // naglowek
        mMapHeader = new MapHeader();

        mMapHeader->Version = xml.GetInt( "version" );
        if(mMapHeader->Version != CURRENT_MAP_VERSION){
            gConsole.Printf(L"Invalid map version (%d, expected %d) for map %s",mMapHeader->Version,CURRENT_MAP_VERSION,filename.c_str());
			return false;
        }

        mMapHeader->Width = xml.GetInt( "width" );
        mMapHeader->Height = xml.GetInt( "height" );
		m_size.x = mMapHeader->Width;
		m_size.y = mMapHeader->Height;

        // maksymalna ilosc zywych potworow
        if ((node = xml.GetChild(0, "max-living-monsters")))
            mMaxLivingMonsters = xml.GetInt(node);
        else
            mMaxLivingMonsters = MAX_MONSTERS_DEFAULT;

        // typy kafli
        CMapTileType *mtt;
        for (node=xml.GetChild(0,"tiletype"); node; node=xml.GetSibl(node,"tiletype")) {
            mtt = new CMapTileType();
			mtt->img = xml.GetString(node,"image");
			mtt->code = xml.GetString(node,"code");
            mtt->number = xml.GetInt(node,"index");
			if(gResourceManager.LoadImage( mtt->img )==false){
				gConsole.Printf(L"Cannot load tile image: %ls", StringUtils::ConvertToWString(mtt->img).c_str());
                ClearTiles();
                delete mtt;
				return false;
			}
            mTileTypes.push_back(mtt);
        }

        // rozmieszczenie kafli
        node = xml.GetChild(0,"tiles");
        if (!node){
            gConsole.Printf(L"failed to load tiles for map %s",filename.c_str());
            return false;
        }
        const std::string &cdata = xml.GetString(node);
        std::stringstream ss(cdata);
        int tilesNum = mMapHeader->Width *  mMapHeader->Height;
		for(int i=0;i<tilesNum;i++){
            ss >> str;
			CTile * tile = new CTile(str);
			int imageIndex = GetTileTypeIndex(str);

            tile->InitializeTile( mTileTypes[ imageIndex ]->img, mTileTypes[ imageIndex ]->number);
            tile->SetPosition( ( i % mMapHeader->Width ) + 0.5f, (i / mMapHeader->Width) + 0.5f );

			mFields->push_back(tile);
		}

        // typy physicali
		CMapObjectType* mpt;
		for (node=xml.GetChild(0,"objtype"); node; node=xml.GetSibl(node,"objtype") ){
			str = /*"data/physicals/" + */ xml.GetString(node,"file");
            if(gResourceManager.LoadPhysicalTemplate( str )==false){
				gConsole.Printf(L"Cannot load object template file: %ls", StringUtils::ConvertToWString(str).c_str());
                ClearTiles();
				return false;
			}
			mpt = new CMapObjectType();
			mpt->code = xml.GetString(node,"code");
			mpt->templ = gResourceManager.GetPhysicalTemplate( str );
			mMapObjectTypes.push_back( mpt );
        }

        // physicale
        CMapObjectDescriptor *mapObj;
        for (node=xml.GetChild(0,"obj"); node; node=xml.GetSibl(node,"obj") ){
            str = xml.GetString(node,"code");
            int i = GetMapObjectTypeIndex(str);
            if (i < 0)
                return false;
            mapObj = new CMapObjectDescriptor();
            mapObj->code = str;
            mapObj->name = xml.GetString(node,"name");
            mapObj->x = xml.GetFloat(node,"x");
            mapObj->y = xml.GetFloat(node,"y");
            mapObj->rot = xml.GetInt(node,"rot");
            mapObj->templ = mMapObjectTypes[i]->templ;
            mapObj->bossRadius = xml.GetFloat(node, "trigger-radius");
            mapObj->bossAI = xml.GetString(node, "trigger-ai");
            mapObj->bossPlaylist = xml.GetString(node, "trigger-playlist");
            if (node->first_node() != NULL)
                mapObj->param = mMapObjectTypes[i]->templ->ReadParam(xml,node);
            mMapObjectDescriptors.push_back(mapObj);
        }

        // regiony
        CRegionDescriptor *rDesc;
        float x,y,scale = 1.f;
        for (node=xml.GetChild(0,"region"); node; node=xml.GetSibl(node,"region") ){
            str = xml.GetString(node,"name");
            x = xml.GetFloat(node,"x");
            y = xml.GetFloat(node,"y");
            if (node->first_attribute("scale") != NULL)
                scale = xml.GetFloat(node,"scale");
            rDesc = new CRegionDescriptor(str,sf::Vector2f(x,y),scale);
            rDesc->Load(xml,node);
            mRegionDescriptors.push_back(rDesc);
        }

		// sprite'y - rosliny etc
		CDoodahDescriptor * doodah;
		for (node=xml.GetChild(0,"sprite"); node; node=xml.GetSibl(node,"sprite") ){
			doodah = new CDoodahDescriptor();
			doodah->file = /*std::string("data/maps/") + */ xml.GetString(node,"file");
			doodah->x = xml.GetFloat(node,"x");
			doodah->y = xml.GetFloat(node,"y");
			doodah->zindex = ( xml.GetString(node,"z") == "foreground" ? Z_MAPSPRITE_FG : Z_MAPSPRITE_BG );
            doodah->anim = xml.GetString(node,"animname");

            // damork : extremely temp (szybka zmiana zindexow)
			/*int zi = doodah->zindex;
			std::vector<std::string> zbg;
			zbg.push_back( "data/maps/doodahs/foliage1.png" );
			zbg.push_back( "data/maps/doodahs/foliage2.png" );
			zbg.push_back( "data/maps/doodahs/foliage3.png" );
			zbg.push_back( "data/maps/doodahs/foliage4.png" );
			zbg.push_back( "data/maps/doodahs/foliage5.png" );
			zbg.push_back( "data/maps/doodahs/foliage6.png" );
			zbg.push_back( "data/maps/doodahs/foliage7.png" );
			zbg.push_back( "data/maps/doodahs/wood_stack_one.png" );
			zbg.push_back( "data/maps/doodahs/wood_stack_big.png" );
			zbg.push_back( "data/maps/doodahs/tree_bark_one.png" );
			zbg.push_back( "data/maps/doodahs/tree_bark_two.png" );

			for (unsigned i = 0; i < zbg.size(); i++)
				if ( doodah->file == zbg[i] )
					zi = Z_MAPSPRITE_BG;

			doodah->zindex = zi;*/

			// ============================

			doodah->rotation = xml.GetFloat(node,"rot");
            doodah->scale = xml.GetFloat(node,"scale");
			mDoodahDescriptors.push_back( doodah );
		}

        return true;
    }

    void CMap::RespawnMapObjects(bool loadCompleteMap){
        for (unsigned int i = 0; i < mMapObjectDescriptors.size(); i++)
        {
            if (loadCompleteMap || mMapObjectDescriptors[i]->CoreMapObject())
                mMapObjectDescriptors[i]->Create();
        }
        for (unsigned int i = 0; i < mRegionDescriptors.size(); i++)
#ifndef __EDITOR__
            mRegionDescriptors[i]->Create();
#else
            mRegionDescriptors[i]->Create()->SetDisplay(true);
#endif
    }

	void CMap::RespawnDoodahs(){
		for (unsigned int i = 0; i < mDoodahDescriptors.size(); i++)
			mDoodahDescriptors[i]->Create();
	}

	void CMap::CullVisibility( const sf::FloatRect &  viewRectangle ) {
		for (int y = mViewRectangle.Top; y <= mViewRectangle.Bottom; y++)
			for (int x = mViewRectangle.Left; x <= mViewRectangle.Right; x++)
				(*mFields)[y*mMapHeader->Width+x]->SetVisible(false);

		mViewRectangle.Left = (int) viewRectangle.Left;
		mViewRectangle.Top = (int) viewRectangle.Top;
		mViewRectangle.Right = (int) viewRectangle.Right + 1;
		mViewRectangle.Bottom = (int) viewRectangle.Bottom + 1;

		mViewRectangle.Left = mViewRectangle.Left < 0 ? 0 : mViewRectangle.Left;
		mViewRectangle.Top = mViewRectangle.Top < 0 ? 0 : mViewRectangle.Top;
		mViewRectangle.Right = mViewRectangle.Right >= mMapHeader->Width ? mMapHeader->Width - 1 : mViewRectangle.Right;
		mViewRectangle.Bottom = mViewRectangle.Bottom >= mMapHeader->Height ? mMapHeader->Height - 1 : mViewRectangle.Bottom;

		for (int y = mViewRectangle.Top; y <= mViewRectangle.Bottom; y++)
			for (int x = mViewRectangle.Left; x <= mViewRectangle.Right; x++)
				(*mFields)[y*mMapHeader->Width+x]->SetVisible(true);
	}

    void CMap::ClearTiles(){
        for(unsigned int i=0;i<mTileTypes.size();i++){
            delete mTileTypes[i];
        }
        mTileTypes.clear();
    }

	void CMap::ClearMapObjects(){
		for (unsigned i = 0; i < mMapObjectDescriptors.size(); i++)
			delete mMapObjectDescriptors[i];
		mMapObjectDescriptors.clear();

		for (unsigned i = 0; i < mRegionDescriptors.size(); i++)
			delete mRegionDescriptors[i];
		mRegionDescriptors.clear();
	}

	void CMap::ClearDoodahs(){
		for (unsigned i = 0; i < mDoodahDescriptors.size(); i++)
			delete mDoodahDescriptors[i];
		mDoodahDescriptors.clear();
	}

    int CMap::GetTileTypeIndex(const std::string &str){
        for (unsigned int i = 0; i < mTileTypes.size(); i++)
            if (mTileTypes[i]->code == str)
                return i;
        fprintf(stderr,"failed to find image code %s\n",str.c_str());
        return 0;
    }

    int CMap::GetMapObjectTypeIndex(const std::string &str){
        for (unsigned int i = 0; i < mMapObjectTypes.size(); i++)
			if (mMapObjectTypes[i]->code == str)
                return i;
        // zwraca -1, wiec to chyba nie jest niezbedne
        //fprintf(stderr,"failed to find object template code %s\n",str.c_str());
        return -1;
    }

    bool CMap::Save(const std::string &filename){
        // plik tymczasowy
        std::ofstream file( (filename + ".tmp").c_str() );

        if(!file.is_open())
            return false;

        Serialize(file);

        file.close();

        // przenoszenie z .tmp do wlasciwego pliku
        if (boost::filesystem::exists(filename + ".tmp"))
        {
            if (boost::filesystem::exists(filename))
                boost::filesystem::remove(filename);
            boost::filesystem::rename(filename + ".tmp", filename);
        }
        else
            return false;

        return true;
    }

    void CMap::Serialize(std::ostream &out)
    {
        out << "<map>\n";
        out << "\t<version>" << GetVersion() << "</version>\n";
        out << "\t<width>" << GetSize().x << "</width>\n";
        out << "\t<height>" << GetSize().y << "</height>\n";
        if (mMaxLivingMonsters != MAX_MONSTERS_DEFAULT)
            out << "<max-living-monsters>" << mMaxLivingMonsters << "</max-living-monsters>\n";
        // typy kafli
        for (unsigned int i = 0; i < mTileTypes.size(); i++){
            out << "\t<tiletype code=\"" << mTileTypes[i]->code << "\" image=\"" << mTileTypes[i]->img << "\"/>\n";
        }
        // kafle
        out << "\t<tiles>";
        for (unsigned int i = 0; i < mFields->size(); i++){
            if (i % GetSize().x == 0) out << "\n\t\t";
            out << mFields->at(i)->GetCode() << " ";
        }
        out << "\n\t</tiles>\n";
        // typy obiektow
        for (unsigned int i = 0; i < mMapObjectTypes.size(); i++){
            if (mMapObjectTypes[i]->templ)
			    out << "\t<objtype code=\"" << mMapObjectTypes[i]->code << "\" file=\"" << mMapObjectTypes[i]->templ->GetFilename() << "\"/>\n";
            else
                fprintf(stderr, "ERROR: null objtype template pointer @ CMap::Serialize!\n");
        }
        // obiekty
        for (unsigned int i = 0; i < mMapObjectDescriptors.size(); i++){
            out << "\t<obj code=\"" << mMapObjectDescriptors[i]->code << "\" x=\"" << mMapObjectDescriptors[i]->x
                << "\" y=\"" << mMapObjectDescriptors[i]->y << "\"";
            if (mMapObjectDescriptors[i]->rot != 0.0f)
                out << " rot=\"" << mMapObjectDescriptors[i]->rot << "\"";
            if (mMapObjectDescriptors[i]->param != NULL){
                out << ">\n";
                if (mMapObjectDescriptors[i]->templ)
                    mMapObjectDescriptors[i]->templ->SerializeParam(out, mMapObjectDescriptors[i]->param, 1);
                else
                    fprintf(stderr, "ERROR: null object descriptor's template pointer @ CMap::Serialize!\n");
                out << "\t</obj>\n";
            } else {
                out << "/>\n";
            }
        }

        // regiony
        /*  <region name="exit" x="6.0" y="43.5">
  	        <next-map>data/maps/level2.xml</next-map>
  	        <next-map-region>entry</next-map-region>
            </region> */
        for (unsigned int i = 0; i < mRegionDescriptors.size(); i++){
            mRegionDescriptors[i]->Serialize(out, 1);
        }

        // doodahy
        // <sprite out="data/maps/doodahs/tree2.png"           x="0.27" y="40.8" z="foreground" scale="1.4" rot="35" />
        for (unsigned int i = 0; i < mDoodahDescriptors.size(); i++){
            out << "\t<sprite file=\"" << mDoodahDescriptors[i]->file << "\" x=\"" << mDoodahDescriptors[i]->x
                << "\" y=\"" << mDoodahDescriptors[i]->y << "\" z=\""
                << (mDoodahDescriptors[i]->zindex == Z_MAPSPRITE_FG ? "foreground" : "background")
                << "\" scale=\"" << mDoodahDescriptors[i]->scale << "\" rot=\"" << mDoodahDescriptors[i]->rotation << "\"/>\n";
        }
        out << "</map>";
    }

    const std::string &CMap::GetOrCreateTileCode(const std::string &path){
        // szukamy istniejacego kodu
        for (unsigned int i = 0; i < mTileTypes.size(); i++)
			if ( mTileTypes[i]->img == path)
                return mTileTypes[i]->code;

        // nie znaleziono kodu, wiec probujemy zsyntetyzowac nowy
		unsigned maxLength = 2;
		std::vector<char> chars;
		for (char ch = 'a'; ch <= 'z'; ch++)
			chars.push_back(ch);

        for (int i = 0; i < 500; i++){
            std::string code = StringUtils::GenerateCode(chars,maxLength,i);
            bool found = false;
            for (unsigned int i = 0; i < mTileTypes.size(); i++){
				if (mTileTypes[i]->code == code)
                    found = true;
            }
            if (!found){
                CMapTileType *mtt = new CMapTileType();
                mtt->img = path;
                mtt->code = code;
                mTileTypes.push_back(mtt);
                return mtt->code;
            }
        }

        static const std::string standardCode = "error";
        return standardCode;
    }

    void CMap::SetTile(int x,int y, const std::string &code){
        int index = y * mMapHeader->Width + x;

        CTile * tile = new CTile(code);
        int imageIndex = GetTileTypeIndex(code);

        tile->InitializeTile(mTileTypes[imageIndex]->img, mTileTypes[imageIndex]->number);
        tile->SetPosition( ( index % mMapHeader->Width ) + 0.5f, (index / mMapHeader->Width) + 0.5f );

		if (mFields->at(index)) delete mFields->at(index);
		mFields->at(index) = tile;
    }

	void CMap::AddDoodah(const std::string& file,float x,float y,int z,float rot,float scale,bool create){
		CDoodahDescriptor* doodah = new CDoodahDescriptor();
		doodah->file = file;
		doodah->x = x;
		doodah->y = y;
		doodah->rotation = rot;
		doodah->scale = scale;
		doodah->zindex = z;
		mDoodahDescriptors.push_back(doodah);

		if (create)
			doodah->Create();
	}

	void CMap::EraseDoodah(int doodahDescriptorId) {
		for (unsigned i = 0; i < mDoodahDescriptors.size(); i++ )
		if (mDoodahDescriptors[i]->uniqueId == doodahDescriptorId) {
			mDoodahDescriptors.erase( mDoodahDescriptors.begin() + i );
			return;
		}
	}

	const std::string& CMap::GetOrCreateMapObjectCode(CPhysicalTemplate *t) {
        for (unsigned int i = 0; i < mMapObjectTypes.size(); i++)
			if ( mMapObjectTypes[i]->templ == t )
				return mMapObjectTypes[i]->code;

		unsigned maxLength = 2;
		std::vector<char> chars;
		for (char ch = 'a'; ch <= 'z'; ch++)
			chars.push_back(ch);

        for (int i = 0; i < 500; i++){
            std::string code = StringUtils::GenerateCode(chars,maxLength,i);
			if (GetMapObjectTypeIndex(code) < 0) {
				CMapObjectType *mot = new CMapObjectType();
				mot->code = code;
				mot->templ = t;
                mMapObjectTypes.push_back(mot);
                return mot->code;
            }
        }

        static const std::string standardCode = "error";
        return standardCode;
	}

    void CMap::AddMapObject(const std::string &code, float x, float y, float rot, bool create, const std::string& name) {

		int i = GetMapObjectTypeIndex(code);

		if (i >= 0) {
			CMapObjectDescriptor* mapObject = new CMapObjectDescriptor();
			mapObject->code = code;
			mapObject->name = name;
			mapObject->param = NULL;
			mapObject->rot = (int) rot;
			mapObject->x = x;
			mapObject->y = y;
			mapObject->templ = mMapObjectTypes[i]->templ;
			mMapObjectDescriptors.push_back(mapObject);

			if (create)
				mapObject->Create();
		}
	}

	void CMap::EraseMapObject(int mapObjectDescriptorId) {

		for (unsigned i = 0; i < mMapObjectDescriptors.size(); i++ )
		if (mMapObjectDescriptors[i]->uniqueId == mapObjectDescriptorId) {
			mMapObjectDescriptors.erase( mMapObjectDescriptors.begin() + i );
			return;
		}
	}

    void CMap::AddRegion(CRegionDescriptor* region, bool show)
    {
        mRegionDescriptors.push_back(region);
        CDynamicRegion* rgn = region->Create();
        rgn->SetDisplay(true);
    }

    void CMap::EraseRegion(int regionDescriptorId)
    {
        for (std::vector<CRegionDescriptor*>::iterator i = mRegionDescriptors.begin(); i != mRegionDescriptors.end(); ++i)
            if ((*i)->uniqueId == regionDescriptorId)
            {
                delete (*i);
                mRegionDescriptors.erase(i);
                return;
            }
    }

    CRegionDescriptor* CMap::GetRegionDescriptorById(int regionDescriptorId)
    {
        for (std::vector<CRegionDescriptor*>::iterator i = mRegionDescriptors.begin(); i != mRegionDescriptors.end(); ++i)
            if ((*i)->uniqueId == regionDescriptorId)
                return (*i);

        return NULL;
    }
}

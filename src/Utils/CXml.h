#ifndef _XML_H_
#define _XML_H_

#include <RapidXML/rapidxml_utils.hpp>
#include <string>
using namespace rapidxml;

//tox, 6 kwietnia, cos mi sie nie kompilowalo:
// enum EAspect;
#include "../Logic/Stats/EAspect.h"

class CXml
{
public:
	CXml( std::string filename, std::string root );
	CXml( const std::string& text );
	~CXml();

	/* pobierz wezel dokumentu xml */

	xml_node<>* GetChild( xml_node<>* parent, const std::string &nodeName );
	xml_node<>* GetSibl( xml_node<>* sibling, const std::string &nodeName );    

	/* pobierz atrybut wskazanego typu */

	float		GetFloat( xml_node<>* parent, const std::string &attrName = "", float def = 0.0f );
	int			GetInt( xml_node<>* parent, const std::string &attrName = "", int def = 0 );
	std::string	GetString( xml_node<>* parent, const std::string &attrName = "" );
	EAspect 	GetAspect( xml_node<>* parent, const std::string &attrName = "" );

	/* wzgledem roota */

    xml_node<>* GetRootNode();
	float		GetFloat(const std::string &nodeName, const std::string &attrName = "", float def = 0.0f );
	int			GetInt(const std::string &nodeName, const std::string &attrName = "", int def = 0 );
	std::string	GetString(const std::string &nodeName, const std::string &attrName = "" );
    EAspect     GetAspect(const std::string &nodeName, const std::string &attrName = "");

    const std::string &GetFilename() const { return mFilename; };
private:
	xml_document<>	mXmlDoc;
	xml_node<>*		mXmlRoot;
	std::string     mFilename;
	file<char>*		mFile;
};

#endif

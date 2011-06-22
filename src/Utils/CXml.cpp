#include "CXml.h"
#include "StringUtils.h"
#include <iostream>
#include "../CGameOptions.h"
#include <string>
#include "FileUtils.h"

using namespace StringUtils;
using namespace rapidxml;

CXml::CXml(std::string filename, std::string root)
: mXmlRoot( NULL ), mFilename(filename), mFile( NULL )
{
	std::string filepath = CGameOptions::GetModDir() + filename;
	if (!FileUtils::FileExists (filepath.c_str()))
		filepath = filename;
    try{
        mFile = new file<>( filepath.c_str() );
        fprintf(stderr, "Parsing XML file: %s\n", filepath.c_str());
	    mXmlDoc.parse< parse_declaration_node >( mFile->data() );
	    mXmlRoot = mXmlDoc.first_node( root.c_str() );
    } catch(std::runtime_error &ex){
        fprintf(stderr,"ERROR while reading file %s: %s\n",filepath.c_str(),ex.what());
    }
}

CXml::~CXml()
{
    if (mFile != NULL)
        delete mFile;
}

xml_node<>* CXml::GetChild(xml_node<> *parent, const std::string &nodeName)
{
	xml_node<>* realParent = parent ? parent : mXmlRoot;
	if (realParent)
		return realParent->first_node( nodeName.c_str() );
	return NULL;
}

xml_node<>* CXml::GetSibl(xml_node<> *sibling, const std::string &nodeName)
{
	if (sibling)
		return sibling->next_sibling( nodeName.c_str() );
	return NULL;
}

xml_node<>* CXml::GetRootNode() { return mXmlRoot; }

float CXml::GetFloat(xml_node<> *parent, const std::string &attrName, float def)
{
	float out = def;
	xml_node<>* realParent = parent ? parent : mXmlRoot;
    std::string attrValue;
    if (realParent){
        if (attrName == ""){
            if (realParent->first_node()){
                attrValue = realParent->first_node()->value();
                FromString( attrValue, out );
            }
        }
        else
            if ( xml_attribute<>* attr = realParent->first_attribute(attrName.c_str()) ) {
                attrValue = attr->value();
                FromString( attrValue, out );
            }
    }
	return out;
}

int CXml::GetInt(xml_node<> *parent, const std::string &attrName, int def)
{
	int out = def;
	xml_node<>* realParent = parent ? parent : mXmlRoot;
    std::string attrValue;
    if (realParent) {
        if (attrName == ""){
            if (realParent->first_node()){
                attrValue = realParent->first_node()->value();
                FromString( attrValue, out );
            }
        }
        else
            if ( xml_attribute<>* attr = realParent->first_attribute(attrName.c_str()) ) {
                attrValue = attr->value();
                FromString( attrValue, out );
            }
    }
    return out;
}

std::string CXml::GetString(xml_node<> *parent, const std::string &attrName)
{
    xml_node<>* realParent = parent ? parent : mXmlRoot;

    if (realParent){
        if (attrName == ""){
            if (realParent->first_node())
                return realParent->first_node()->value();
        }
        else if ( xml_attribute<>* attr = realParent->first_attribute(attrName.c_str()) ) {
            std::string out = attr->value();
            return out;
        }
    }
    // tox, 16 sierpnia: jest sens zwracac stringa z pustym znakiem zamiast pustego stringa?
    // return "\0";
    return "";
}

EAspect CXml::GetAspect(xml_node<> *parent, const std::string &attrName)
{
    xml_node<>* realParent = parent ? parent : mXmlRoot;
    std::string out = "";
    if (realParent){
        if (attrName == ""){
            if (realParent->first_node())
                out = realParent->first_node()->value();
        }
        else{
            if ( xml_attribute<>* attr = realParent->first_attribute(attrName.c_str()) ) {
                out = attr->value();
            }
        }
    }
    return parseAspect(ConvertToWString(out));
}

float CXml::GetFloat(const std::string &nodeName, const std::string &attrName, float def)
{
	if (!mXmlRoot) return def;
    xml_node<> * parent = mXmlRoot->first_node(nodeName.c_str());
    if (!parent) return def;

    return GetFloat(parent, attrName, def);
}

int CXml::GetInt(const std::string &nodeName, const std::string &attrName, int def)
{
	if (!mXmlRoot) return def;
    xml_node<> * parent = mXmlRoot->first_node(nodeName.c_str());
    if (!parent) return def;

	return GetInt(parent, attrName, def );
}

std::string CXml::GetString(const std::string &nodeName, const std::string &attrName)
{
	if (!mXmlRoot) return "";
    xml_node<> * parent = mXmlRoot->first_node(nodeName.c_str());
    if (!parent) return "";

    return GetString(parent, attrName);
}

EAspect CXml::GetAspect(const std::string &nodeName, const std::string &attrName)
{
	if (!mXmlRoot) return aNoAspect;
    xml_node<> * parent = mXmlRoot->first_node(nodeName.c_str());
    if (!parent) return aNoAspect;

    return GetAspect(parent, attrName);
}


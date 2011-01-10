/* Wzorzec sciany */

#ifndef _WALL_TEMPLATE_H_
#define _WALL_TEMPLATE_H_

#include "CPhysicalTemplate.h"
#include "../CPhysicalManager.h"
#include "../MapObjects/CWall.h"
#include <string>
#include <vector>

class CXml;

class CWallTemplate : public CPhysicalTemplate
{
public:
	CWallTemplate() {}
	~CWallTemplate() { mImages.clear(); }

	/* IResource */

	std::string GetType();
	bool Load(CXml &xml);
	void Drop();

	/* ========= */

	virtual void PrepareEditorPreviewer( CDisplayable* d );

    CWall* Create(std::wstring id = L"");

    virtual bool CoreMapObject() const { return true; }

    virtual void SetImageFilename(const std::string& path);

private:
	std::vector<std::string> mImages;
	sf::Vector2f mSize;
};

#endif


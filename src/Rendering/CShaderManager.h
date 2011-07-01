#ifndef CSHADERMANAGER_H_
#define CSHADERMANAGER_H_

#ifdef WG_SHADERS

#include <vector>

#include <GL/glew.h>

#include "../Utils/CSingleton.h"
#include "ZIndexVals.h"

namespace sf {
	class RenderWindow;
}

class IDrawable;
class CHudSprite;
class CHudStaticText;
class CDisplayable;

typedef std::vector< IDrawable* > DrawableList;
typedef std::vector< DrawableList > DrawableLists;

#define gShaderManager CShaderManager::GetSingleton()

class CShaderManager: public CSingleton<CShaderManager> {
public:
    CShaderManager();
    ~CShaderManager();
	
	void activate(std::string const & programName);
private:
	void load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName);

	GLcharARB * readFile(std::string const & filename);

	GLenum my_program;
};

#endif /* WG_SHADERS */

#endif /*CSPRITEMANAGER_H_*/

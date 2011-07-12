#ifndef CSHADERMANAGER_H_
#define CSHADERMANAGER_H_

#ifdef WG_SHADERS

#include <vector>
#include <string>
#include <map>

#include <GL/glew.h>

#include "../Utils/CSingleton.h"
#include "../IKeyListener.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

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

class CShaderManager: public CSingleton<CShaderManager>, public IKeyListener {
public:
    CShaderManager();
    ~CShaderManager();
	
	void prepareToDraw(IDrawable *drawable);
	// returns programId (to allow uniform binding)
	int activate(std::string const & programName);

    bool setUniform(int programId, const std::string& name, float value);
    bool setUniform(int programId, const std::string& name, sf::Vector2f value);
    bool setUniform(int programId, const std::string& name, sf::Vector3f value);
    bool setUniform(int programId, const std::string& name, sf::Color value);
	// program must be active!
	// remember to call clearBoundTextures() later on
	bool bindTexture(int programId, const std::string & name, sf::Image const * image);
	void clearBoundTextures();
	
    virtual void KeyPressed( const sf::Event::KeyEvent &e ) {};
    virtual void KeyReleased( const sf::Event::KeyEvent &e );

	void reloadAll();
private:
	void load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName);

	GLcharARB * readFile(std::string const & filename);

	std::vector<GLenum> programs;

	int getProgramId(std::string const & name);
	std::map<std::string, unsigned int> programNames;
	
	bool needToClearBoundTextures;
};

#endif /* WG_SHADERS */

#endif /*CSPRITEMANAGER_H_*/

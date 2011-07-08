#ifndef CSHADERMANAGER_H_
#define CSHADERMANAGER_H_

#ifdef WG_SHADERS

#include <vector>
#include <string>

#include <GL/glew.h>

#include "../Utils/CSingleton.h"
#include "../IKeyListener.h"

#include <SFML/Graphics/Color.hpp>
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
	
	void activate(std::string const & programName);

    bool setUniform(const std::string& name, float value);
    bool setUniform(const std::string& name, sf::Vector2f value);
    bool setUniform(const std::string& name, sf::Vector3f value);
    bool setUniform(const std::string& name, sf::Color value);
	
    virtual void KeyPressed( const sf::Event::KeyEvent &e ) {};
    virtual void KeyReleased( const sf::Event::KeyEvent &e );

	void reloadAll();
private:
	void load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName);

	GLcharARB * readFile(std::string const & filename);

	GLenum my_program;
};

#endif /* WG_SHADERS */

#endif /*CSPRITEMANAGER_H_*/

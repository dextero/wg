#ifndef CSHADERMANAGER_H_
#define CSHADERMANAGER_H_

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

class IDrawable;
namespace sf { class Matrix3; }

#define gShaderManager CShaderManager::GetSingleton()

class CShaderManager: public CSingleton<CShaderManager>, public IKeyListener {
public:
    CShaderManager();
    ~CShaderManager();

	inline bool shadersAvailable() {
		return GLEW_VERSION_2_0 != NULL;
	}
	
	// returns programId (to allow uniform binding)
	int activate(std::string const & programName);
	void activateDefault();

    bool setUniform(int programId, const std::string& name, float value);
    bool setUniform(int programId, const std::string& name, const sf::Vector2f& value);
    bool setUniform(int programId, const std::string& name, const sf::Vector3f& value);
    bool setUniform(int programId, const std::string& name, const sf::Color& value);
	bool setUniform(int programId, const std::string& name, const sf::Matrix3& value);
	// program must be active!
	// remember to call clearBoundTextures() later on
	bool bindTexture(int programId, const std::string & name, sf::Image const * image);
	void clearBoundTextures();
	
    virtual void KeyPressed( const sf::Event::KeyEvent &e ) {};
    virtual void KeyReleased( const sf::Event::KeyEvent &e );

	void reloadAll();
	void loadNormalMaps();
private:
	void load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName);

	GLcharARB * readFile(std::string const & filename);
	bool verifyShaderCompiled(GLenum shader, const std::string & shaderName);
	int getProgramId(std::string const & name);

	std::map<std::string, unsigned int> programNames;
	std::vector<GLenum> programs;

	int currentProgram;
	bool needToClearBoundTextures;
};

#endif /*CSPRITEMANAGER_H_*/

#ifdef WG_SHADERS

#include "CShaderManager.h"

#include "CHudSprite.h"
#include "CHudStaticText.h"
#include "CDisplayable.h"

#include "IDrawable.h"

#include "../CGame.h"
#include "../ResourceManager/CResourceManager.h"
#include "../ResourceManager/CImage.h"
#include "CDisplayable.h"
#include <SFML/Graphics/Image.hpp>
#include "ZIndexVals.h"
#include "../Utils/Maths.h"
#include "../Utils/CXml.h"

#include "GLCheck.h"

#include <iostream>
#include <cstdio>

template<> CShaderManager* CSingleton<CShaderManager>::msSingleton = 0;

CShaderManager::CShaderManager():
	needToClearBoundTextures(false)
{
    fprintf(stderr,"CShaderManager::CShaderManager()\n");
	this->reloadAll();
	this->loadNormalMaps();
    gGame.AddKeyListener( this );
}

CShaderManager::~CShaderManager(){
    fprintf(stderr,"CShaderManager::~CShaderManager()\n");
}

void CShaderManager::prepareToDraw(IDrawable * drawable){
	int z = drawable->GetZIndex();
	CDisplayable * displayable = dynamic_cast<CDisplayable *>(drawable);
	if (displayable != NULL){
		const sf::Image * img = displayable->GetSFSprite()->GetImage();
		const sf::Image * normalmap = displayable->GetSFSprite()->GetNormalMap();
		bool useNM = (img != NULL) && (normalmap != NULL) && ((z <= Z_TILE && z >= Z_PLAYER && z != Z_SHADOWS) || z == Z_MAPSPRITE_FG);
		if (useNM){
			int id = this->activate("normal-map");
			if (id >= 0){
				float rot = displayable->GetRotation();
				this->setUniform(id, "lpos", sf::Vector3f(
					Maths::Rotate(Maths::VectorUp(), -rot).x, 
					Maths::Rotate(Maths::VectorUp(), -rot).y, 
					0.5f)
				);
				this->setUniform(id, "lcolor", sf::Color(255,255,255,255));
				this->bindTexture(id, "normalmap", normalmap);
			}
			/*int id = this->activate("normal-sobel");
			if (id >= 0){
				this->setUniform(id, "uTexSize", sf::Vector2f(img->GetWidth(), img->GetHeight()));
				float rot = displayable->GetRotation();
				this->setUniform(id, "lpos", sf::Vector3f(
					Maths::Rotate(Maths::VectorUp(), -rot).x, 
					Maths::Rotate(Maths::VectorUp(), -rot).y, 
					0.3f)
				);
				this->setUniform(id, "lcolor", sf::Color(255,255,255,255));
				float normalStrength = 1.0f;
				if (z == Z_PHYSICAL) normalStrength = 3.0f;
				else if (z == Z_TILE) normalStrength = 0.5f;
				this->setUniform(id, "normalStrength", normalStrength);
			}*/
		} else if (z == Z_PLAYER && img != NULL) {
			fprintf(stderr, "Using freeze\n");
			int id = this->activate("freeze");
			if (id >= 0){
				this->setUniform(id, "uTexSize", sf::Vector2f(img->GetWidth(), img->GetHeight()));
				this->setUniform(id, "uImageSize", 64.f);
			}
		}
	} 
}

int CShaderManager::activate(std::string const & programName){
	if (programName.empty()){
		glUseProgram(0);
		return -1;
	} else {
		int id = getProgramId(programName);
		if (id >= 0){
			glUseProgram(this->programs[id]);
		} else {
			fprintf(stderr, "shader ERROR: couldn't get program id during activate: %s\n", programName.c_str());
		}
		return id;
	}
}

bool CShaderManager::setUniform(int programId, const std::string& name, float value)
{
    GLint location = glGetUniformLocation(this->programs[programId], name.c_str());
    if (location < 0) {
        fprintf(stderr, "shader ERROR: couldn't get uniform location: %s\n", name.c_str());
        return false;
    }
    
    glUniform1f(location, value);
    return true;
}

bool CShaderManager::setUniform(int programId, const std::string& name, sf::Vector2f value)
{
    GLint location = glGetUniformLocation(this->programs[programId], name.c_str());
    if (location < 0) {
        fprintf(stderr, "shader ERROR: couldn't get uniform location: %s\n", name.c_str());
        return false;
    }
    
    glUniform2fv(location, 1, &value.x);
    return true;
}

bool CShaderManager::setUniform(int programId, const std::string& name, sf::Vector3f value){	
    GLint location = glGetUniformLocation(this->programs[programId], name.c_str());
    if (location < 0) {
        fprintf(stderr, "shader ERROR: couldn't get uniform location: %s\n", name.c_str());
        return false;
    }
    
    glUniform3fv(location, 1, &value.x);
    return true;
}

bool CShaderManager::setUniform(int programId, const std::string& name, sf::Color value)
{
    GLint location = glGetUniformLocation(this->programs[programId], name.c_str());
    if (location < 0) {
        fprintf(stderr, "shader error: couldn't get uniform location: %s\n", name.c_str());
        return false;
    }
    
    float col[4] = {
        (float)value.r / 255.f,
        (float)value.g / 255.f,
        (float)value.b / 255.f,
        (float)value.a / 255.f
    };
    glUniform4fv(location, 1, col);
    return true;
}

// program must be active!
bool CShaderManager::bindTexture(int programId, const std::string & name, sf::Image const * image)
{
	GLint location = glGetUniformLocation(this->programs[programId], name.c_str());
	if (location < 0) {
		fprintf(stderr, "shader ERROR: couldn't get uniform location: %s\n", name.c_str());
		return false;
	}
	GLCheck(glActiveTexture(GL_TEXTURE1));
	image->Bind();
	GLCheck(glActiveTexture(GL_TEXTURE0));
	GLCheck(glUniform1i(location, 1));
	this->needToClearBoundTextures = true;
	return true;
}

void CShaderManager::clearBoundTextures()
{
	if (this->needToClearBoundTextures){
		this->needToClearBoundTextures = false;
		GLCheck(glActiveTexture(GL_TEXTURE1));
		GLCheck(glBindTexture(GL_TEXTURE_2D, 0));
		GLCheck(glActiveTexture(GL_TEXTURE0));
	}
}

void CShaderManager::KeyReleased( const sf::Event::KeyEvent &e ){
	if (e.Code == sf::Key::F9){
		this->reloadAll();
	}
}

void CShaderManager::reloadAll(){
	this->load("data/effects/normalmap.frag", "data/effects/normalmap.vert", "normal-map");
	this->load("data/effects/normal.frag", "data/effects/default.vert", "normal-sobel");
	this->load("data/effects/blue.frag", "data/effects/default.vert", "freeze");
}

void CShaderManager::load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName){
	const GLcharARB * my_fragment_shader_source = readFile(fragmentShaderName);
	const GLcharARB * my_vertex_shader_source = readFile(vertexShaderName);
	
	if (!my_fragment_shader_source || !my_vertex_shader_source){
		return;
	}

	if (glCreateProgram == NULL){
	    fprintf(stderr, "Warning - attempting to load shader but glCreateProgram not present!\n");
	}

	// Create Shader And Program Objects
	this->programs.push_back(glCreateProgram());
	GLenum my_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLenum my_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load Shader Sources
	GLCheck(glShaderSource(my_vertex_shader, 1, &my_vertex_shader_source, NULL));
	GLCheck(glShaderSource(my_fragment_shader, 1, &my_fragment_shader_source, NULL));

	// Compile The Shaders
	int isCompiled;
	GLCheck(glCompileShader(my_vertex_shader));
	glGetShaderiv(my_vertex_shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == false)
	{
		int maxLength;
		glGetShaderiv(my_vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);
 
		/* The maxLength includes the NULL character */
		char * vertexInfoLog = new char[maxLength];
 
		glGetShaderInfoLog(my_vertex_shader, maxLength, &maxLength, vertexInfoLog);
		fprintf(stderr, "Vertex shader %s compilation failed! Log:\n%s\n", vertexShaderName.c_str(), vertexInfoLog);
 
		delete [] vertexInfoLog;
		return;
	}
	GLCheck(glCompileShader(my_fragment_shader));
	glGetShaderiv(my_fragment_shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == false)
	{
		int maxLength;
		glGetShaderiv(my_fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
 
		/* The maxLength includes the NULL character */
		char * fragmentInfoLog = new char[maxLength];
 
		glGetShaderInfoLog(my_fragment_shader, maxLength, &maxLength, fragmentInfoLog);
		fprintf(stderr, "Fragment shader %s compilation failed! Log:\n%s\n", fragmentShaderName.c_str(), fragmentInfoLog);
 
		delete [] fragmentInfoLog;
		return;
	}

	// Attach The Shader Objects To The Program Object
	GLCheck(glAttachShader(this->programs.back(), my_vertex_shader));
	GLCheck(glAttachShader(this->programs.back(), my_fragment_shader));

	// Link The Program Object
	GLCheck(glLinkProgram(this->programs.back()));

	this->programNames[programName] = this->programs.size() - 1;

	delete[] my_fragment_shader_source;
	delete[] my_vertex_shader_source;
}

GLcharARB * CShaderManager::readFile(std::string const & filename){
	// TODO push through resource manager to get all the benefits
	FILE *fp = fopen(filename.c_str(),"rb");
	if (fp == NULL){
		fprintf(stderr, "ERROR: failed to load shader file %s\n", filename.c_str());
		return NULL;
	}
	fseek(fp,0,SEEK_END); //go to end
	long len = ftell(fp); //get position at end (length)
	fseek(fp,0,SEEK_SET); //go to beg.
	GLcharARB *buf = new GLcharARB[len + 1]; //malloc buffer
	fread(buf, len, 1, fp); //read into buffer
	buf[len] = 0;
	fclose(fp);
	fprintf(stderr, "read shader file %s\n", filename.c_str());
	return buf;
}

int CShaderManager::getProgramId(std::string const & name) 
{
	if (this->programNames.count(name) > 0){
		return this->programNames[name];
	} else {
		return -1;
	}
}

void CShaderManager::loadNormalMaps()
{
	CXml xml( "data/effects/normalmaps/normalmaps-desc.xml", "root" );
	xml_node<>* node;
	for (node = xml.GetChild(0,"mat"); node; node = xml.GetSibl(node, "mat"))
	{
		System::Resource::CImage* img = gResourceManager.GetImage(xml.GetString(node, "tex"));
		if (img != NULL)
		{
			System::Resource::CImage* nm = gResourceManager.GetImage(xml.GetString(node, "nm"));
			img->SetNormalMap(nm);
		}
	}
}

#endif /* WG_SHADERS */

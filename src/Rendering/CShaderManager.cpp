#ifdef WG_SHADERS

#include "CShaderManager.h"

#include "CHudSprite.h"
#include "CHudStaticText.h"
#include "CDisplayable.h"

#include "IDrawable.h"
#include "ZIndexVals.h"

#include "../CGame.h"
 
#include <iostream>
#include <cstdio>

template<> CShaderManager* CSingleton<CShaderManager>::msSingleton = 0;

CShaderManager::CShaderManager():
	my_program(NULL)
{
    fprintf(stderr,"CShaderManager::CShaderManager()\n");
	this->load("data/effects/blue.frag", "data/effects/default.vert", "test");
}

CShaderManager::~CShaderManager(){
    fprintf(stderr,"CShaderManager::~CShaderManager()\n");
}

void CShaderManager::activate(std::string const & programName){
	if (programName.empty()){
		glUseProgram(0);
	} else {
		glUseProgram(this->my_program);
	}
}


// TODO - copied from WGSprite.cpp

#ifdef SFML_DEBUG

    // In debug mode, perform a test on every OpenGL call
    #define GLCheck(Func) ((Func), GLCheckError(__FILE__, __LINE__))

#else

    // Else, we don't add any overhead
    #define GLCheck(Func) (Func)

#endif

#define GL_CLAMP_TO_EDGE    0x812F


////////////////////////////////////////////////////////////
/// Check the last OpenGL error
///
////////////////////////////////////////////////////////////
inline void GLCheckError(const std::string& File, unsigned int Line)
{
    // Get the last error
    GLenum ErrorCode = glGetError();

    if (ErrorCode != GL_NO_ERROR)
    {
        std::string Error = "unknown error";
        std::string Desc  = "no description";

        // Decode the error code
        switch (ErrorCode)
        {
            case GL_INVALID_ENUM :
            {
                Error = "GL_INVALID_ENUM";
                Desc  = "an unacceptable value has been specified for an enumerated argument";
                break;
            }

            case GL_INVALID_VALUE :
            {
                Error = "GL_INVALID_VALUE";
                Desc  = "a numeric argument is out of range";
                break;
            }

            case GL_INVALID_OPERATION :
            {
                Error = "GL_INVALID_OPERATION";
                Desc  = "the specified operation is not allowed in the current state";
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                Error = "GL_STACK_OVERFLOW";
                Desc  = "this command would cause a stack overflow";
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                Error = "GL_STACK_UNDERFLOW";
                Desc  = "this command would cause a stack underflow";
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                Error = "GL_OUT_OF_MEMORY";
                Desc  = "there is not enough memory left to execute the command";
                break;
            }

/*            case GL_INVALID_FRAMEBUFFER_OPERATION_EXT :
            {
                Error = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
                Desc  = "the object bound to FRAMEBUFFER_BINDING_EXT is not \"framebuffer complete\"";
                break;
            }*/
        }

        // Log the error
        std::cerr << "An internal OpenGL call failed in "
                  << File.substr(File.find_last_of("\\/") + 1) << " (" << Line << ") : "
                  << Error << ", " << Desc
                  << std::endl;
    }
}

void CShaderManager::load(std::string const & fragmentShaderName, std::string const & vertexShaderName, std::string const & programName){
	const GLcharARB * my_fragment_shader_source = readFile(fragmentShaderName);
	const GLcharARB * my_vertex_shader_source = readFile(vertexShaderName);
	
	if (!my_fragment_shader_source || !my_vertex_shader_source){
		return;
	}

	// Create Shader And Program Objects
	this->my_program = glCreateProgram();
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
 
		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete [] vertexInfoLog;
		return;
	}
	GLCheck(glCompileShader(my_fragment_shader));
	if(isCompiled == false)
	{
		int maxLength;
		glGetShaderiv(my_fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
 
		/* The maxLength includes the NULL character */
		char * fragmentInfoLog = new char[maxLength];
 
		glGetShaderInfoLog(my_fragment_shader, maxLength, &maxLength, fragmentInfoLog);
		fprintf(stderr, "Fragment shader %s compilation failed! Log:\n%s\n", fragmentShaderName.c_str(), fragmentInfoLog);
 
		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete [] fragmentInfoLog;
		return;
	}

	// Attach The Shader Objects To The Program Object
	GLCheck(glAttachShader(this->my_program, my_vertex_shader));
	GLCheck(glAttachShader(this->my_program, my_fragment_shader));

	// Link The Program Object
	GLCheck(glLinkProgram(this->my_program));

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

#endif /* WG_SHADERS */

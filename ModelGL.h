/*

	with shaders
*/


#ifndef MODEL_GL_H
#define MODEL_GL_H

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
//#pragma comment(lib, "freetype253.lib")
//#pragma comment(lib, "freetype253_D.lib")

#include <gl/glew.h>  //  =>   will  #include <GL/gl.h>  #include <GL/glu.h>
// see C:\Users\Pete\Documents\glm-0.9.5.2\glm\doc
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>  //
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"

//#include <ft2build.h>
//#include FT_FREETYPE_H
#include "resource.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <Xinput.h> // Gamecontroller API

using namespace std;

struct Triangle
{
	glm::vec3 vert[3]; // the three verts of the triangle
};

class ModelGL
{
public:
    ModelGL();
    void hookUpShaders();    // initialize OpenGL states
	bool hello_Gamepad();    // initialize OpenGL states
 //   void setViewport(int width, int height);
    void resizeWindow(int width, int height);
	void hiFromDW(int page);
	std::vector<Triangle> cube_Faces( int grid );
	std::vector<Triangle> equilateral( int grid );
	std::vector<Triangle> icosahedron( int grid );
	void Translate(int Triangle, int number_of_Triangle, float translate, float x, float y, float z);
	void Rotate(int Triangle, int number_of_Triangle, float angle, float x, float y, float z);

    void drawGL();
	void generateGL_Buffers();
	void load_obj();
	//void load_obj(const char* filename,	vector<glm::vec4> &vertices, vector<glm::vec3> &normal,								vector<GLushort> &elements);
	glm::vec3 origin;
	glm::vec3 xaxis;
	glm::vec3 yaxis;
	glm::vec3 zaxis;

	glm::mat4 mCurrent;
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 Spin;
	glm::mat4 Spin2;
	glm::mat4 Size;
	glm::mat4 Move;
	glm::mat4 mvp;

// shaders
	class ShaderLoad
	{
	public:
		bool LoadShader(string sFile, int a_iType);
		void DeleteShader();
		bool IsLoaded();
		UINT GetShaderID();
	private:
		UINT uiShader;	// ID of shader 
		int iType;		// GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
		bool bLoaded;	// Flag shader as compiled and loaded.
		string ShaderLog;
	};
	class Shader
	{
	public:
		void CreateProgram();
		void DeleteProgram();
		bool AddShaderToProgram(ShaderLoad* shShader);
		bool LinkProgram();
		void UseProgram();
		UINT GetProgramID(){ return uiProgram;}

		
	private:
		UINT uiProgram; // ID of program
		bool bLinked;	// Flag program as linked and ready to use.
	};



private:
	float angle;
	int equi_triangle_Count;
	int cube_triangle_Count;
    int windowWidth;
    int windowHeight;
    bool windowResized;
	int GLroutine;       // Used to switch between drawing routines
	// GLuint programAlias;  ????? never used
	// uniform locations
		GLuint routineNumberLocation;
		GLuint offsetLocation;
		GLuint frustumScaleUnif;
		GLuint zNearUnif;
		GLuint zFarUnif;
		GLuint iModelViewLoc;

	float bgColor[4];  // Screen bacground color

	static const int cPOINT_COORD = 3;
	GLsizei cPOINT_BUFFER_SIZE;
	GLuint uiVBO[25];	// Buffer handles
	GLuint uiVAO[10];	// Array handles
	
	struct pointCoordinates{ float x; float y; float z; };

	pointCoordinates point_1;

};// end class ModelGL

#endif

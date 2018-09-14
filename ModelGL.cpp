/*
						ModelGL.cpp

Runs different GL demos, using the routine number associated with a page in the current document.  

class ModelGL{

    void hookUpShaders();   // compile - link - attach our shaders
    void resizeWindow(int width, int height);
	void hiFromDW(int page); // Receives the GLroutine number from DW via ContollerMain
    void drawGL(); //  draws GLroutine

	class ShaderLoad{
		bool LoadShader(string sFile, int a_iType);
		void DeleteShader();
		bool IsLoaded();
		UINT GetShaderID();  }

	class Shader{
		void CreateProgram();
		void DeleteProgram();
		bool AddShaderToProgram(ShaderLoad* shShader);
		bool LinkProgram();
		void UseProgram();
		UINT GetProgramID(){ return uiProgram;   }
}

***************************************************************
	Matrix uniform hookup psuedo-code

(prog.cpp)		GLuint uiProgram;   \\ Shader program ID
				GLuint offset_Uniform;  \\ Shader uniform IDs
				GLuint scale_Matrix_Uniform;

				glm::mat4 mScale;  \\Our scale matix to pass to shader
				mScale = glm::scale(glm::mat4(1.0f), glm::vec3(u, u, u));

				uiProgram = glCreateProgram();  \\ Create shader program ret. the																shader	ID uint

(Shader.vert)	uniform vec2 offset;
				uniform mat4 scale_Matrix;

(prog.cpp)		offsetUniform = glGetUniformLocation(uiProgram, "offset");
				glUniform2f(offsetUniform, 0.5f, 0.5f);				
				scale_Matrix_Uniform = glGetUniformLocation(uiProgram, "scale_Matrix");
				glUniformMatrix4fv(scale_Matrix_Uniform, 1, GL_FALSE, mScale);

(Shader.vert)

		layout(location = 0) in vec4 position;
		layout(location = 1) in vec4 color;


		void main()
		{
			gl_Position = scale_Matrix * position;
			theColor = color;
		}
*/

#include "ModelGL.h"

const float half_PI = asin(1.0f);
const float whole_PI = 2.0f*half_PI;       // 90 deg
const float third_PI =  whole_PI/3.0f;  //60.0 deg
const float epsilon = 0.6523581f;
const float icosaH = sin(epsilon); // height of icosa vert off pentagon base
const float beta = 2.0f*whole_PI/5.0f; // pentagon angle 
const float dihedral_Angle = acos(-(sqrt(5.0f)/3.0f));	// for icosahedron 138.18969;
const float dihedral_Comp = whole_PI - dihedral_Angle;
const float icosaR = sin(2.0f*whole_PI/5.0f); // radius of circumscribed icosa-sphere
const float sin_third_PI = sin(third_PI);

ModelGL::ShaderLoad shVertex, shFragment;
ModelGL::Shader spMain;
//static int number_of_triangles = 0;

ModelGL::ModelGL() : windowWidth(0), windowHeight(0), windowResized(false),  GLroutine(-1), angle(30.0f)
{
		// set background color to black
    bgColor[0] = bgColor[1] = bgColor[2] = bgColor[3] = 0.0;
	origin = glm::vec3 (0.0f,0.0f,0.0f);
	xaxis  = glm::vec3 (1.0f,0.0f,0.0f);
	yaxis  = glm::vec3 (0.0f,1.0f,0.0f);
	zaxis  = glm::vec3 (0.0f,0.0f,1.0f);

}

std::vector<Triangle> ModelGL::icosahedron( int N ){
//Win::log(L"void ModelGL::icosahedron(int grid) = %i", N);

	size_t No_of_Equilateral_Triangles = 20*N*N;
    std::vector<Triangle> T_icosahedron(No_of_Equilateral_Triangles);

	glm::mat4 x_Lat;
	glm::vec3 hold;
	glm::vec4 temp;
	float angle = third_PI;
	float height = (1.0f/(float)N)*0.866025f;
    float h = height;
	float b = 1.0f/(float)N; // for base
	float bHalf = b/2.0f;
	Triangle T;  // This is the seed triangle

	T.vert[0].x =0.0f;T.vert[0].y = 0.0f;T.vert[0].z = 0.0f;
	T.vert[1].x =b/2.0f;T.vert[1].y = h;T.vert[1].z = 0.0f;
	T.vert[2].x =b;T.vert[2].y = 0.0f; T.vert[2].z = 0.0f;
	int count(0);
//	Create T0 (red) the seed triangle in the xy plane
	for(int row = 0; row < N; row++){
		count = 0;
//		Win::log(L"Row %i", row );
		for(int n = 2*N*row - row*row; n < 2*N*row - row*row + 2*N - 2*row - 1; n++){	
			
	//		std::cout <<" n = " << n  <<" row = " << row   <<"in the  loop \n";
			if (count % 2 == 0 ){ // just translate triangle is even in the row
				x_Lat = glm::translate(glm::mat4(), glm::vec3((count+row)*bHalf, row*height, 0.0f));
				for(int i = 0; i < 3; i++){
					temp = glm::vec4(T.vert[i], 1.0);
					temp = x_Lat*temp;
					T_icosahedron[n].vert[i] = temp.xyz;
				}						
			}
			else{ // rotate then translate
				x_Lat = glm::translate(glm::mat4(), glm::vec3((count+1+row)*bHalf, row*height, 0.0f));
				Spin = glm::rotate( glm::mat4(1.0), third_PI, zaxis ); 
				for(int i = 0; i < 3; i++){
					temp = glm::vec4(T.vert[i], 1.0);
					temp = x_Lat*Spin*temp;
					T_icosahedron[n].vert[i] = temp.xyz;
				}
			}
//			for(int i = 0; i<3;i++){
//Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );				
//			}
			count++;
		}
	}
// Create T1 (blue) from the T0 verts: rotate T0 pi/3 around z then 
// bend the dihedral to T0
	glm::vec3 axis(0.5f, sin_third_PI, 0.0f);
	Spin = glm::rotate( glm::mat4(1.0),  third_PI, zaxis );  //just rotate
	Spin2 = glm::rotate( glm::mat4(1.0), -dihedral_Comp, axis );
	for(int n = 0; n < N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin2*Spin*temp;
			T_icosahedron[N*N + n].vert[i] = temp.xyz;
		}						
	}

// xlate everything by -1.0 along x and bend down by epsilon from the xy plane
	x_Lat = glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, 0.0f));
	Spin2 = glm::rotate( glm::mat4(1.0), -third_PI, zaxis );
	Spin = glm::rotate( glm::mat4(1.0), -epsilon, xaxis );  //just rotate
	for(int n = 0; n < 2*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin*Spin2*x_Lat*temp;
			T_icosahedron[n].vert[i] = temp.xyz;
		}						
	}
// Create T2 (green) & T3 (brown) from the T0 & T1 verts: rotate 2pi/5 around z then 

	Spin = glm::rotate( glm::mat4(1.0), beta, zaxis );  //just rotate
	for(int n = 0; n < 2*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin*temp;
			T_icosahedron[n+2*N*N].vert[i] = temp.xyz;
		}						
	}
	//	for(int n = 0; n < 2*N*N; n++){
	//	for(int i = 0; i < 3; i++){
	////		Win::log(L"T_icosahedron[%i].vert[%i] = %f  %f  %f  ",n,i,T_icosahedron[n].vert[i].x,T_icosahedron[n].vert[i].y,T_icosahedron[n].vert[i].z);	
	//	}						
	//}
// Create T3 (green) thru T9 (yellow) from the T0 & T1 verts: rotate 2*2pi/5 around z then 

	Spin = glm::rotate( glm::mat4(1.0), 2*beta, zaxis );  //just rotate
	for(int n = 0; n < 2*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin*temp;
			T_icosahedron[n+4*N*N].vert[i] = temp.xyz;
		}						
	}
	Spin = glm::rotate( glm::mat4(1.0), 3*beta, zaxis );  //just rotate
	for(int n = 0; n < 2*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin*temp;
			T_icosahedron[n+6*N*N].vert[i] = temp.xyz;
		}						
	}
	Spin = glm::rotate( glm::mat4(1.0), 4*beta, zaxis );  //just rotate
	for(int n = 0; n < 2*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = Spin*temp;
			T_icosahedron[n+8*N*N].vert[i] = temp.xyz;
		}						
	}
	x_Lat = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, icosaR));
	for(int n = 0; n < 10*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(T_icosahedron[n].vert[i], 1.0);
			temp = x_Lat*temp;
			T_icosahedron[n].vert[i] = temp.xyz;
		}						
	}

// invert all the verts and reverse for cw winding
// this creates the other half of the icosahedron from the first 10 triangles
	for(int n = 0; n < 10*N*N; n++){
		for(int i = 0; i < 3; i++){ // invert
			T_icosahedron[n+10*N*N].vert[i].x = -T_icosahedron[n].vert[i].x;
			T_icosahedron[n+10*N*N].vert[i].y = -T_icosahedron[n].vert[i].y;
			T_icosahedron[n+10*N*N].vert[i].z = -T_icosahedron[n].vert[i].z;
		}
		hold = T_icosahedron[n+10*N*N].vert[0];// reverse
		T_icosahedron[n+10*N*N].vert[0] = T_icosahedron[n+10*N*N].vert[2];
		T_icosahedron[n+10*N*N].vert[2] = hold;
	}
//Sphereify
	for(int i = 0; i < 20*N*N; i++){
		for(int j = 0; j < 3; j++){
	   float length_of_v = sqrt((T_icosahedron[i].vert[j].x * T_icosahedron[i].vert[j].x) + (T_icosahedron[i].vert[j].y * T_icosahedron[i].vert[j].y) + (T_icosahedron[i].vert[j].z * T_icosahedron[i].vert[j].z));
	   T_icosahedron[i].vert[j].x = T_icosahedron[i].vert[j].x/length_of_v;
	   T_icosahedron[i].vert[j].y = T_icosahedron[i].vert[j].y/length_of_v;
	   T_icosahedron[i].vert[j].z =T_icosahedron[i].vert[j].z/length_of_v;
		}
	}

// Stelate
	   T_icosahedron[N*N-1].vert[1].x = T_icosahedron[N*N-1].vert[1].x*2.0f;
	   T_icosahedron[N*N-1].vert[1].y = T_icosahedron[N*N-1].vert[1].y*2.0f;
	   T_icosahedron[N*N-1].vert[1].z = T_icosahedron[N*N-1].vert[1].z*2.0f;



	return T_icosahedron;
}
std::vector<Triangle> ModelGL::cube_Faces(int grid){
	Win::log(L"void ModelGL::cube_Faces(int grid) = %i)", grid );
	size_t No_of_Cube_Triangles(12*(2*grid )*(2*grid));
	Win::log(L"number_of_triangles = %i",No_of_Cube_Triangles);
	int number_of_GLverts(3*No_of_Cube_Triangles);
	Win::log(L"number_of_GLverts = %i",number_of_GLverts);
	size_t number_of_face_points(6*(2*grid +1 )*(2*grid+1));
	Win::log(L"number_of_face_points = %i", number_of_face_points);
	std::vector<glm::vec3> face_points(number_of_face_points);
	std::vector<Triangle> glTriangle(No_of_Cube_Triangles);


	int l(0);
	for(int i = 0; i < 6; i++){
		for(int j = -grid; j <= grid; j++){
			for(int k = -grid; k <= grid; k++){// Below "if" strips out all interior cube verts.
				if( i == 0){ // do yz faces
					face_points[l].x = (float)(-grid); //x
					face_points[l].y = (float)j; //y
					face_points[l].z = (float)k;//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);
				}
				if( i == 1){ // do yz faces
					face_points[l].x = (float)grid; //x
					face_points[l].y = (float)j; //y
					face_points[l].z = (float)k;//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);
				}
				if( i == 2){ // do zx faces
					face_points[l].x = (float)j; //x
					face_points[l].y = (float)(-grid); //y
					face_points[l].z = (float)k;//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);

				}
				if( i == 3){ // do zx faces
					face_points[l].x = (float)j; //x
					face_points[l].y = (float)(grid); //y
					face_points[l].z = (float)k;//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);
				}
				if( i == 4){ // do xy faces
					face_points[l].x = (float)j; //x
					face_points[l].y = (float)k; //y
					face_points[l].z = (float)(-grid);//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);
				}
				if( i == 5){ // do xy faces
					face_points[l].x = (float)j; //x
					face_points[l].y = (float)k; //y
					face_points[l].z = (float)(grid);//z
				//	Win::log(L"face_points[%i]  = %f  %f  %f",l,face_points[l].x, face_points[l].y, face_points[l].z);
				}
				l++;
			}
		}
	}
//Win::log(L"l = %i", l);

	int s = 2*grid;
	int q = 2*s*s;  // no triangles per face
	int a = (s+1)*(s+1);
	int f(0);
	int r(0);
	int h(0);


	// Note:  This loop uses integer division so q*(n/q) is not = to n
	for( int n=0; n < q; ){  //The x = -n, yz face
		   // do the even indicies	
		   glTriangle[n].vert[0] = face_points[h];
		   glTriangle[n].vert[1] = face_points[h+s+1];
		   glTriangle[n].vert[2] = face_points[h+s+2];
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h, h+s+1, h+s+2, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   // do the odd indicies
		   glTriangle[n].vert[0] = face_points[h+s+2];
		   glTriangle[n].vert[1] = face_points[h+1];
		   glTriangle[n].vert[2] = face_points[h];
		 //  Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, h+s+2, h+1, h, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
	for( int n=q; n < 2*q; ){  //The x = +n, yz face	
		   glTriangle[n].vert[0] = face_points[h+s+1]; //even
		   glTriangle[n].vert[1] = face_points[h];
		   glTriangle[n].vert[2] = face_points[h+1];
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h+s+1, h, h+1, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   glTriangle[n].vert[0] = face_points[h+1]; //odd
		   glTriangle[n].vert[1] = face_points[h+s+2];
		   glTriangle[n].vert[2] = face_points[h+s+1];
		//   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n,  h+1,  h+s+2, h+s+1, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
	for( int n=2*q; n < 3*q; ){  //The y = -n, zx face
		   glTriangle[n].vert[0] = face_points[h]; //even
		   glTriangle[n].vert[1] = face_points[h+1];
		   glTriangle[n].vert[2] = face_points[h+s+2];
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h, h+1, h+s+2,  f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   glTriangle[n].vert[0] = face_points[h+s+2]; //odd
		   glTriangle[n].vert[1] = face_points[h+s+1];
		   glTriangle[n].vert[2] = face_points[h];
		//   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, h+s+2, h+s+1, h,  f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
	for( int n=3*q; n < 4*q; ){  //The y = n, zx face
		   glTriangle[n].vert[0] = face_points[h+1]; //even
		   glTriangle[n].vert[1] = face_points[h];
		   glTriangle[n].vert[2] = face_points[h+s+1];
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n,  h+1, h, h+s+1,  f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   glTriangle[n].vert[0] = face_points[h+s+1]; //odd
		   glTriangle[n].vert[1] = face_points[h+s+2];
		   glTriangle[n].vert[2] = face_points[h+1];		   
		//   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, h+s+1, h+s+2,  h+1,  f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
	for( int n=4*q; n < 5*q; ){  //The z = -n, xy face
		   glTriangle[n].vert[0] = face_points[h]; //even
		   glTriangle[n].vert[1] = face_points[h+s+1];
		   glTriangle[n].vert[2] = face_points[h+s+2];
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h, h+s+1, h+s+2, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   glTriangle[n].vert[0] = face_points[h+s+2]; //odd
		   glTriangle[n].vert[1] = face_points[h+1];
		   glTriangle[n].vert[2] = face_points[h];		   
		//   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, h+2+s,  h+1,  h, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
	for( int n=5*q; n < 6*q; ){  //The z = n, xy face
		   glTriangle[n].vert[0] = face_points[h+s+1]; //even
		   glTriangle[n].vert[1] = face_points[h];
		   glTriangle[n].vert[2] = face_points[h+1];		   
		//   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h+s+1, h,  h+1, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   glTriangle[n].vert[0] = face_points[h+1]; //odd
		   glTriangle[n].vert[1] = face_points[h+s+2];
		   glTriangle[n].vert[2] = face_points[h+s+1];		   
		//   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, h+1,  h+s+2, h+s+1, f);
		   n++; f= q*(n/q); r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
	}
//// reverse order for CW winding s + 1 + h, h, h+1   1+h, h, s+2+h, s+1+h
//		if(  n >  5*q - 1 || n <  3*q && n > q - 1 ){
//		   Win::log(L"3ORglTriangle[%i] = %i  %i  %i n/q = %i",n, s + 1 + h, h, h+1, f);
//		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
//		   // do the odd indicies
//		   Win::log(L"4OEglTriangle[%i] = %i  %i  %i n/q = %i",n,  h+1, s+2+h, s+1+h, f);
//		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
//		}
//	}// end for( int n=0; n < number_of_triangles; )


/* This 
	for( int n=0; n < No_of_Cube_Triangles; ){  //The x = grid, yz face
		   // do the even indicies
		if(  n <  q || n <  5*q && n > 3*q - 1 ){			
		//	f= q*(n/q); r = a*(n/q);  h = (n-f)/2 + (n-f)/(2*s) + r;
		   Win::log(L"1EglTriangle[%i] = %i  %i  %i f = %i",n, h, s + 1 + h, s + 2 + h, f);
		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   // do the odd indicies
		   Win::log(L"2OglTriangle[%i] = %i  %i  %i f = %i",n, s + 2 + h,  1 + h, h, f);
		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;// note integer division here
		}
// reverse order for CW winding 
		if(  n >  5*q - 1 || n <  3*q && n > q - 1 ){
		   Win::log(L"3ORglTriangle[%i] = %i  %i  %i n/q = %i",n, s + 2 + h, s + 1 + h, h, f);
		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		   // do the odd indicies
		   Win::log(L"4OEglTriangle[%i] = %i  %i  %i n/q = %i",n, h, 1 + h, s + 2 + h, f);
		   n++;f= q*(n/q);r = a*(n/q); h = (n-f)/2 + (n-f)/(2*s) + r;
		}
	}// end for( int n=0; n < number_of_triangles; )
*/
//	f=0; //reset these correction factors
//	r=0;
//
//	for( int n=0; n < No_of_Cube_Triangles; ){  
//		   // do the even indicies
//		if( n <  q || n <  5*q && n > 3*q - 1  ){
//		   f= q*(n/q); r = a*(n/q);  h = (n-f)/2 + (n-f)/(2*s) + r;
//		   glTriangle[n].vert[0] = face_points[h];
//		//   Win::log(L"glTriangle[%i].vert[0]  = %f  %f  %f %i",n, glTriangle[n].vert[0].x, glTriangle[n].vert[0].y, glTriangle[n].vert[0].z, n/q);
//		   glTriangle[n].vert[1] = face_points[s + 1 + h];
//		//   Win::log(L"glTriangle[%i].vert[1]  = %f  %f  %f",n, glTriangle[n].vert[1].x, glTriangle[n].vert[1].y, glTriangle[n].vert[1].z);
//		   glTriangle[n].vert[2] = face_points[s + 2 + h];
//		//   Win::log(L"glTriangle[%i].vert[2]  = %f  %f  %f",n, glTriangle[n].vert[2].x, glTriangle[n].vert[2].y, glTriangle[n].vert[2].z);
//		   n++;f= q*(n/q);r = a*(n/q);	 h = (n-f)/2 + (n-f)/(2*s) + r;
//
//		   // do the odd indicies
//		   glTriangle[n].vert[0] = face_points[s + 2 + h];
//		//   Win::log(L"glTriangle[%i].vert[0]  = %f  %f  %f",n, glTriangle[n].vert[0].x, glTriangle[n].vert[0].y, glTriangle[n].vert[0].z);
//		   glTriangle[n].vert[1] = face_points[ 1 + h];
////		   Win::log(L"(n+1)/2 + (n-1)/(4*grid)  = %i", (n+1)/2 + (n-1)/(4*grid));
//		//   Win::log(L"glTriangle[%i].vert[1]  = %f  %f  %f",n, glTriangle[n].vert[1].x, glTriangle[n].vert[1].y, glTriangle[n].vert[1].z);
//		   glTriangle[n].vert[2] = face_points[h];
//		//   Win::log(L"glTriangle[%i].vert[2]  = %f  %f  %f",n, glTriangle[n].vert[2].x, glTriangle[n].vert[2].y, glTriangle[n].vert[2].z);
//		   n++;f= q*(n/q);r = a*(n/q);	h = (n-f)/2 + (n-f)/(2*s) + r;
//
//		}
//// reverse order for CW winding
//		if( n >  5*q - 1 || n <  3*q && n > q - 1 ){
//		   glTriangle[n].vert[0] = face_points[s + 2 + h];
//		//   Win::log(L"glTriangle[%i].vert[0]  = %f  %f  %f %i",n, glTriangle[n].vert[0].x, glTriangle[n].vert[0].y, glTriangle[n].vert[0].z, n/q);
//		   glTriangle[n].vert[1] = face_points[s + 1 + h];
//		//   Win::log(L"glTriangle[%i].vert[1]  = %f  %f  %f",n, glTriangle[n].vert[1].x, glTriangle[n].vert[1].y, glTriangle[n].vert[1].z);
//		   glTriangle[n].vert[2] =  face_points[h];
//		//   Win::log(L"glTriangle[%i].vert[2]  = %f  %f  %f",n, glTriangle[n].vert[2].x, glTriangle[n].vert[2].y, glTriangle[n].vert[2].z);
//		   n++;f= q*(n/q);r = a*(n/q);	h = (n-f)/2 + (n-f)/(2*s) + r;
//
//		   // do the odd indicies
//		   glTriangle[n].vert[0] = face_points[h];
//		//   Win::log(L"glTriangle[%i].vert[0]  = %f  %f  %f",n, glTriangle[n].vert[0].x, glTriangle[n].vert[0].y, glTriangle[n].vert[0].z);
//		   glTriangle[n].vert[1] = face_points[1 + h];
////		   Win::log(L"(n+1)/2 + (n-1)/(4*grid)  = %i", (n+1)/2 + (n-1)/(4*grid));
//		//   Win::log(L"glTriangle[%i].vert[1]  = %f  %f  %f",n, glTriangle[n].vert[1].x, glTriangle[n].vert[1].y, glTriangle[n].vert[1].z);
//		   glTriangle[n].vert[2] = face_points[s + 2 + h];
//		//   Win::log(L"glTriangle[%i].vert[2]  = %f  %f  %f",n, glTriangle[n].vert[2].x, glTriangle[n].vert[2].y, glTriangle[n].vert[2].z);
//		   n++;f= q*(n/q);r = a*(n/q);	 h = (n-f)/2 + (n-f)/(2*s) + r;
//
//		}
//	}

// normalize
for(int i = 0; i < No_of_Cube_Triangles; i++){
	glTriangle[i].vert[0].x = glTriangle[i].vert[0].x/(2.0*(float)grid);
	glTriangle[i].vert[0].y = glTriangle[i].vert[0].y/(2.0*(float)grid);
	glTriangle[i].vert[0].z = glTriangle[i].vert[0].z/(2.0*(float)grid);
	glTriangle[i].vert[1].x = glTriangle[i].vert[1].x/(2.0*(float)grid);
	glTriangle[i].vert[1].y = glTriangle[i].vert[1].y/(2.0*(float)grid);
	glTriangle[i].vert[1].z = glTriangle[i].vert[1].z/(2.0*(float)grid);
	glTriangle[i].vert[2].x = glTriangle[i].vert[2].x/(2.0*(float)grid);
	glTriangle[i].vert[2].y = glTriangle[i].vert[2].y/(2.0*(float)grid);
	glTriangle[i].vert[2].z = glTriangle[i].vert[2].z/(2.0*(float)grid);
}

//Spherify

//for(int i = 0; i < No_of_Cube_Triangles; i++){
//    for(int j = 0; j < 3; j++){
//   float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].y * glTriangle[i].vert[j].y) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
//   glTriangle[i].vert[j].x = glTriangle[i].vert[j].x/length_of_v;
//   glTriangle[i].vert[j].y = glTriangle[i].vert[j].y/length_of_v;
//   glTriangle[i].vert[j].z = glTriangle[i].vert[j].z/length_of_v;
//	}
//}

//Column
	//for(int i = 0; i < No_of_Cube_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//		if( glTriangle[i].vert[j].y < 0.5f  && glTriangle[i].vert[j].y  > -0.5f  ){ 
	//		float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
	//		glTriangle[i].vert[j].x = 0.5f*glTriangle[i].vert[j].x/length_of_v;
	//		glTriangle[i].vert[j].z = 0.5f*glTriangle[i].vert[j].z/length_of_v;
	//		}
	//	}
	//}
//Cylinder
	//for(int i = 0; i < No_of_Cube_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//		if( glTriangle[i].vert[j].y == 0.5f || glTriangle[i].vert[j].y == -0.5f ){ glTriangle[i].vert[j].x = 0.0f; glTriangle[i].vert[j].z = 0.0f;}
	//		else{
	//			float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
	//			glTriangle[i].vert[j].x = 0.5f*glTriangle[i].vert[j].x/length_of_v;
	//			glTriangle[i].vert[j].z = 0.5f*glTriangle[i].vert[j].z/length_of_v;
	//		}
	//	}
	//}
//Cone capped cylinder
	//for(int i = 0; i < No_of_Cube_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//		if( glTriangle[i].vert[j].y == 0.5f || glTriangle[i].vert[j].y == -0.5f ){ glTriangle[i].vert[j].x = 0.0f; glTriangle[i].vert[j].z = 0.0f; glTriangle[i].vert[j].y = 0.40f*glTriangle[i].vert[j].y;}
	//		else{
	//			float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
	//			glTriangle[i].vert[j].x = 0.5f*glTriangle[i].vert[j].x/length_of_v;
	//			glTriangle[i].vert[j].z = 0.5f*glTriangle[i].vert[j].z/length_of_v;
	//		}
	//	}
	//}

//fluted column
	//for(int i = 0; i < No_of_Cube_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//		if( glTriangle[i].vert[j].y < 0.5f  && glTriangle[i].vert[j].y  > -0.5f  ){ 
	//			if( glTriangle[i].vert[j].x * glTriangle[i].vert[j].x != glTriangle[i].vert[j].z * glTriangle[i].vert[j].z)
	//			{
	//				float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
	//				glTriangle[i].vert[j].x = 0.5f*glTriangle[i].vert[j].x/length_of_v;
	//				glTriangle[i].vert[j].z = 0.5f*glTriangle[i].vert[j].z/length_of_v;
	//			}
	//			else 
	//			{
	//				float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
	//				glTriangle[i].vert[j].x = 0.6f*glTriangle[i].vert[j].x/length_of_v;
	//				glTriangle[i].vert[j].z = 0.6f*glTriangle[i].vert[j].z/length_of_v;
	//			}
	//		}
	//	}
	//}
// fluted column with a twist
	glm::vec4 temp;
	float degree;
	Spin = glm::rotate( glm::mat4(1.0), third_PI/10.0f, yaxis ); 
	for(int i = 0; i < No_of_Cube_Triangles; i++){
		for(int j = 0; j < 3; j++){

			if( glTriangle[i].vert[j].y < 0.5f  && glTriangle[i].vert[j].y  > -0.5f  ){ 
				if( glTriangle[i].vert[j].x * glTriangle[i].vert[j].x != glTriangle[i].vert[j].z * glTriangle[i].vert[j].z)
				{
					float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
					glTriangle[i].vert[j].x = 0.5f*glTriangle[i].vert[j].x/length_of_v;
					glTriangle[i].vert[j].z = 0.5f*glTriangle[i].vert[j].z/length_of_v;
				}
				else 
				{
					float length_of_v = sqrt((glTriangle[i].vert[j].x * glTriangle[i].vert[j].x) + (glTriangle[i].vert[j].z * glTriangle[i].vert[j].z));
					glTriangle[i].vert[j].x = 0.6f*glTriangle[i].vert[j].x/length_of_v;
					glTriangle[i].vert[j].z = 0.6f*glTriangle[i].vert[j].z/length_of_v;
				}
			}
					glm::vec4 temp = glm::vec4(glTriangle[i].vert[j], 1.0f);// give it a twist
					degree = (glTriangle[i].vert[j].y + 0.5f)* half_PI;
					Spin = glm::rotate( glm::mat4(1.0), degree, yaxis ); 
					temp = Spin*temp;
					glTriangle[i].vert[j] = temp.xyz;
		}
	}

//Archway
	//glm::mat4 x_Lat;
	//glm::vec4 temp;
	//float degree;
	//x_Lat = glm::translate(glm::mat4(), glm::vec3(0.9f, 0.5f, 0.0f));  // just move the whole cube to sit on the x axis
	//for(int i = 0; i < No_of_Cube_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//		temp = glm::vec4(glTriangle[i].vert[j], 1.0f);
	//		temp = x_Lat*temp;
	//		glTriangle[i].vert[j] = temp.xyz;
	//	}								
	//}

	//for(int i = 0; i < No_of_Cube_Triangles; i++){  // spin around z
	//	for(int j = 0; j < 3; j++){
	//		degree = (glTriangle[i].vert[j].y + 0.5f)*whole_PI - half_PI;
	//		x_Lat = glm::translate(glm::mat4(), glm::vec3(0.0f, -glTriangle[i].vert[j].y, 0.0f));
	//		Spin = glm::rotate( glm::mat4(1.0), degree, zaxis );
	//		temp = glm::vec4(glTriangle[i].vert[j], 1.0f);
	//		temp = Spin*x_Lat*temp;
	//		glTriangle[i].vert[j] = temp.xyz;
	//	}								
	//}

return glTriangle;
}

std::vector<Triangle>  ModelGL::equilateral(int N){
//	Win::log(L"void ModelGL::equilateral(int grid) = %i)", grid );
	size_t No_of_Equilateral_Triangles = N*N;
//	Win::log(L"number_of_triangles = %i",No_of_Equilateral_Triangles);
//	int number_of_GLverts(3*number_of_triangles);
//	Win::log(L"number_of_GLverts = %i",number_of_GLverts);
	//size_t number_of_face_points(6*(2*grid +1 )*(2*grid+1));
	//Win::log(L"number_of_face_points = %i", number_of_face_points);
	//std::vector<glm::vec3> face_points(number_of_face_points);
	std::vector<Triangle> tri_Angles(4*No_of_Equilateral_Triangles);
//	glm::mat4 Spin;
	glm::mat4 x_Lat;
	glm::vec4 temp;
	float height = (1.0f/(float)N)*0.866025f;
    float h = height;
	float b = 1.0f/(float)N; // for base
	float bHalf = b/2.0f;
	Triangle T;  // This is the seed triangle

	T.vert[0].x =0.0f;T.vert[0].y = 0.0f;T.vert[0].z = 0.0f;
	T.vert[1].x =b/2.0f;T.vert[1].y = h;T.vert[1].z = 0.0f;
	T.vert[2].x =b;T.vert[2].y = 0.0f; T.vert[2].z = 0.0f;
	int count(0);
	for(int row = 0; row < N; row++){
		count = 0;
//		Win::log(L"Row %i", row );
		Spin = glm::rotate( glm::mat4(1.0), third_PI, zaxis ); 
		for(int n = 2*N*row - row*row; n < 2*N*row - row*row + 2*N - 2*row - 1; n++){	
			
	//		std::cout <<" n = " << n  <<" row = " << row   <<"in the  loop \n";
			if (count % 2 == 0 ){ // just translate triangle is even in the row
				x_Lat = glm::translate(glm::mat4(), glm::vec3((count+row)*bHalf, row*height, 0.0f));
				for(int i = 0; i < 3; i++){
					temp = glm::vec4(T.vert[i], 1.0);
					temp = x_Lat*temp;
					tri_Angles[n].vert[i] = temp.xyz;
				}						
			}
			else{ // rotate then translate 
				x_Lat = glm::translate(glm::mat4(), glm::vec3((count+1+row)*bHalf, row*height, 0.0f));				
				for(int i = 0; i < 3; i++){
					temp = glm::vec4(T.vert[i], 1.0);
					temp = x_Lat*Spin*temp;
					tri_Angles[n].vert[i] = temp.xyz;
				}
			}
//			for(int i = 0; i<3;i++){
//Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );				
//			}
			count++;
		}
	}
	//artan(2*srqrt2)) = 70.5288f degrees known as the dihedral angle, the angle
	//between two faces
	float tetra_Dihedral = atan(2*sqrt(2));
    Win::log(L"tetra_Dihedral = %f  %f  %f  ", tetra_Dihedral);	
// --- Second face
//	Spin = glm::rotate( glm::mat4(1.0), -70.5288f, xaxis );  //just rotate
	Spin = glm::rotate( glm::mat4(1.0), -tetra_Dihedral, xaxis );  //just rotate
	for(int n = 0; n < N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(tri_Angles[n].vert[i], 1.0);
			temp = Spin*temp;
			tri_Angles[N*N + n].vert[i] = temp.xyz;
//			Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",N*N + n,i,tri_Angles[N*N + n].vert[i].x,tri_Angles[N*N + n].vert[i].y,tri_Angles[N*N + n].vert[i].z );
		}						
	}
//The rotation gives CCW verts so need need to make them CW again
	for(int n = N*N; n < 2*N*N; n++){
		glm::vec3 temp = tri_Angles[n].vert[2];
		tri_Angles[n].vert[2] = tri_Angles[n].vert[0];
		tri_Angles[n].vert[0] = temp;
			for(int i = 0; i < 3; i++){
	//		Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );
			}						
	}

// --- Third face

//	Spin = glm::rotate( glm::mat4(1.0),  70.5288f ,glm::vec3(0.5f,0.866025f,0.0f));  //just rotate
	Spin = glm::rotate( glm::mat4(1.0),  tetra_Dihedral ,glm::vec3(0.5f,0.866025f,0.0f));  //just rotate
	for(int n = 0; n < N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(tri_Angles[n].vert[i], 1.0);
			temp = Spin*temp;

			tri_Angles[2*N*N + n].vert[i] = temp.xyz;
//			Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[2*N*N + n].vert[i].x,tri_Angles[2*N*N + n].vert[i].y,tri_Angles[2*N*N + n].vert[i].z );
		}						
	}
//need to make it CW again
	for(int n = 2*N*N; n < 3*N*N; n++){
	glm::vec3 temp = tri_Angles[n].vert[2];
	tri_Angles[n].vert[2] = tri_Angles[n].vert[0];
	tri_Angles[n].vert[0] = temp;
		for(int i = 0; i < 3; i++){
	//		Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );
			}						
	}

// --- Forth face
	x_Lat = glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, 0.0f));
//	Spin = glm::rotate( glm::mat4(1.0), -70.5288f, glm::vec3(-0.5f,0.866025f,0.0f));
	Spin = glm::rotate( glm::mat4(1.0), -tetra_Dihedral, glm::vec3(-0.5f,0.866025f,0.0f));
	for(int n = 0; n < N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(tri_Angles[n].vert[i], 1.0);
			temp = glm::translate(glm::mat4(), glm::vec3(1.0f, 0.0f, 0.0f))*Spin*x_Lat*temp;
//			temp = Spin*temp;
			tri_Angles[3*N*N + n].vert[i] = temp.xyz;
//			Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[2*N*N + n].vert[i].x,tri_Angles[2*N*N + n].vert[i].y,tri_Angles[2*N*N + n].vert[i].z );
		}						
	}
//need to make it CW again
	for(int n = 3*N*N; n < 4*N*N; n++){
	glm::vec3 temp = tri_Angles[n].vert[2];
	tri_Angles[n].vert[2] = tri_Angles[n].vert[0];
	tri_Angles[n].vert[0] = temp;
	for(int i = 0; i < 3; i++){
//		Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );
		}						
	}
// put the base in the xz plane rotate 90 - dihedral angle
//	Spin = glm::rotate( glm::mat4(1.0),  -19.4712f, xaxis);
	Spin = glm::rotate( glm::mat4(1.0),  tetra_Dihedral - half_PI, xaxis);
	for(int n = 0;  n < 4*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(tri_Angles[n].vert[i], 1.0);
			temp = Spin*temp;
//			temp = Spin*temp;
			tri_Angles[n].vert[i] = temp.xyz;
//			Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );
		}						
	}


// Put the center at the origin, y axis passing through the apex
// sqrt(3/8) =  0.6124 is the distance from vertext to center of tetrahedron side = 1
// 1/2(sqrt(3)) = 0.28867 from a vertex to the center of a side
//	x_Lat = glm::translate(glm::mat4(), glm::vec3(-0.5f, -0.6124f, -1.73205f));

		x_Lat = glm::translate(glm::mat4(), glm::vec3(-0.5f, -0.6124f, 0.28867f));
	for(int n = 0; n < 4*N*N; n++){
		for(int i = 0; i < 3; i++){
			temp = glm::vec4(tri_Angles[n].vert[i], 1.0);
			temp = x_Lat*temp;
//			temp = Spin*temp;
			tri_Angles[n].vert[i] = temp.xyz;
	//		Win::log(L"tri_Angles[%i].vert[%i] = %f  %f  %f  ",n,i,tri_Angles[n].vert[i].x,tri_Angles[n].vert[i].y,tri_Angles[n].vert[i].z );
		}						
	}
	
//Sphereify
	//for(int i = 0; i < 4*No_of_Equilateral_Triangles; i++){
	//	for(int j = 0; j < 3; j++){
	//   float length_of_v = sqrt((tri_Angles[i].vert[j].x * tri_Angles[i].vert[j].x) + (tri_Angles[i].vert[j].y * tri_Angles[i].vert[j].y) + (tri_Angles[i].vert[j].z * tri_Angles[i].vert[j].z));
	//   tri_Angles[i].vert[j].x = tri_Angles[i].vert[j].x/length_of_v;
	//   tri_Angles[i].vert[j].y = tri_Angles[i].vert[j].y/length_of_v;
	//   tri_Angles[i].vert[j].z = tri_Angles[i].vert[j].z/length_of_v;
	//	}
	//}
	return tri_Angles;
}
bool ModelGL::hello_Gamepad(){
		Win::log(L"void ModelGL::hello_Gamepad{" );
	XINPUT_STATE state;
	int controllerId = -1;

	// Check for game controller connected to USB ports

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
	{
	//XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
 
	if (XInputGetState(i, &state) == ERROR_SUCCESS){
		controllerId = i;
		Win::log(L"Have game controller connected on USB port %i.", i);
		return TRUE;
		}
	}
	if (controllerId == -1){
		Win::log(L"No game controller connected");
		return FALSE;}

		//if( (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
		//Win::log(L"A button is depressed");


}

void ModelGL::load_obj(){
	
		const char* filename;	
		vector<glm::vec4> vertices; 
		vector<glm::vec3> normal;								
		vector<GLushort> elements;


	filename = "C:\\Users\\Pete\\Documents\\glWin\\glWin\\src\\BlenderObj.obj";
	ifstream in(filename, ios::in);
    if (!in)
    {
        Win::log(L"ModelGL::load_obj can not open .obj file");
		
    }
	else Win::log(L"ModelGL::load_obj openend BlenderObj.obj successfully");
    string line;
    while (getline(in, line))
    {
  //std::string str="We think in generalities, but we live in details.";
  //                                         // (quoting Alfred N. Whitehead)

  //std::string str2 = str.substr (3,5);     // "think"

  //std::size_t pos = str.find("live");      // position of "live" in str

  //std::string str3 = str.substr (pos);     // get from "live" to the end
        if (line.substr(0,2) == "v ") //get the first two characters of the line
        {
			Win::log(L"ModelGL::load_obj openend BlenderObj.obj successfully");
            istringstream iss;
			istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
			Win::log(L"v.x = %f, v.w = %f"), v.x, v.w;
            vertices.push_back(v);
			Win::log(L"v.x = %f, v.w = %f"), v.x, v.w;
			//std::istringstream iss;
			//std::string value="32 40 50 80 902";

			//iss.str (value); //what does this do???
			//for(int i=0;i<5;i++){
			//    cout<< " " <<i<<"\n";
			//    int val;
			//    iss >> val;
		}
    } // end while
	
}	

void ModelGL::hookUpShaders(){ // initialize OpenGL states and scene

	shVertex.LoadShader("C:\\Users\\Pete\\Documents\\glWin\\glWin\\bin\\data\\shaders\\shader.vert", GL_VERTEX_SHADER);
	shFragment.LoadShader("C:\\Users\\Pete\\Documents\\glWin\\glWin\\bin\\data\\shaders\\shader.frag", GL_FRAGMENT_SHADER);
//	shVertex.LoadShader("\\data\\shaders\\shader.vert", GL_VERTEX_SHADER);
//	shFragment.LoadShader("\\data\\shaders\\shader.frag", GL_FRAGMENT_SHADER);
	spMain.CreateProgram();
	spMain.AddShaderToProgram(&shVertex);
	spMain.AddShaderToProgram(&shFragment);
	spMain.LinkProgram();
	spMain.UseProgram();
	shVertex.DeleteShader();  // once the shader objects have been
	shFragment.DeleteShader(); // linked no longer need them
	generateGL_Buffers();

}
void ModelGL::hiFromDW( int routine ){	// Get the graphics mode: DW_MODE if routine == -1 else GL_MODE with routine in ModelGL::drawGL() == routine
	Win::log(L"void ModelGL::hiFromDW(routine = %i)", routine );
	GLroutine = routine;
}


void ModelGL::generateGL_Buffers(){

	glGenVertexArrays(1, uiVAO); // Get 1 name for one vertex array uiVAO.
								// this object stores all the state neede for rendering
								// glVertexAttribPointer(, andglEnable/DisableVertexAttribArray change the state
								// stored in glGenVertexArrays
	glBindVertexArray(uiVAO[0]); 
	glGenBuffers(3, uiVBO);

//	glBindVertexArray(uiVAO[0]); 
}
void ModelGL::drawGL(){ // called at the frame rate Win::log(L"void ModelGL::drawGL()" );
	bool gamepad_Connected;  // flag to start pollong if we have a gamepad
	if(GLroutine == -1 ) return; // have DW_MODE 
    if(windowResized)
    {
		glViewport(0, 0, windowWidth, windowHeight);
        windowResized = false;
    }
	static int count = -1; // initialization of static variable only takes place on the first call to the funcion
	routineNumberLocation= glGetUniformLocation(spMain.GetProgramID(), "routineNumber");
	glUniform1i(routineNumberLocation, GLroutine);  // The GLroutine is passed to the shaders so they can switch on it.
	switch(GLroutine){
	case 0: {// Cover page;  just a point			
		if( count != 0 ){  // count flags  the first pass, Just do this if block once per call
	//		Win::log(L"ModelGL::drawGL case 0:");
			count = 0;
	//		glPointSize(0.1f);
			glPointSize(2.1f);
			GLfloat fPoint[cPOINT_COORD] = { 0.0f,  0.0f,  0.0f };
			GLfloat fColor[cPOINT_COORD] = { 0.0f,  0.0f,  1.0f };
			glClearColor(0.5, bgColor[1], bgColor[2], bgColor[3]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fPoint), fPoint, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
/*   glVertexAttribPointer( attribute index of input to shader,
				number of values for each position,
				data type of the position data  eg GL_FLOAT,
				???   GL_FALSE,
				spacing between values,
				byte offset from beginning of buffer for
*/
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fColor), fColor, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		}
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT );
		glDrawArrays(GL_POINTS, 0, 1);  // start at 0, and read 1 vertex
		glFlush();
		break;}
	case 1: {// This is the clock
		static int increment = 1;

		if( count != 1 ){ //  Init stuff, just do it once
	//		Win::log(L"ModelGL::drawGL case 1:");
			gamepad_Connected = hello_Gamepad(); // if a Gamepad?  set flag to start polling its state
			count = 1;
			static int increment = 0;
			glClearColor(0.0, 0.0, 0.0, 0.0); //black
			glPointSize(5.2f);
			cPOINT_BUFFER_SIZE = cPOINT_COORD*sizeof(float);
			GLfloat fPoint[cPOINT_COORD] = { -0.9f,  -0.9f,  0.0f };

			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fPoint, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		if(gamepad_Connected){
			
			}
			GLfloat fColor[cPOINT_COORD] = {0.0f,  0.5f,  0.0f };
			increment++;
			if(increment == 140) {
				increment = 0;
				glPointSize(25.0f);
				fColor[0] = 1.0f; fColor[1] = 0.0f;}
			else { glPointSize(5.2f); fColor[0] = 0.0f; fColor[1] = 0.5f;}
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);// attribute index 1 is assigned to color in the fragment shader
									//layout (location = 1) in vec3 inColor
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);		
			glClear(GL_COLOR_BUFFER_BIT);
			glDrawArrays(GL_POINTS, 0, 1);
			glFlush();
			break;}
	case 2: {// A line
		if( count != 2 ){  
	//		Win::log(L"ModelGL::drawGL case 2:");
			count = 2;
			struct Vertex {
			public:
				glm::vec3 Position;
			};
			std::vector<glm::vec3> position(2);  // just trying vectors out here
			position[0].x =0.3f; position[0].y =0.5f; position[0].z =0.0f;
			position[1].x =0.5f; position[1].y =0.9f; position[1].z =0.0f;
			glLineWidth(0.1f);
			cPOINT_BUFFER_SIZE = 6*sizeof(float);
			//GLfloat fPoint[6] = { 0.3f, 0.5f, 0.0f , 0.5, 0.9, 0.0};
			GLfloat fColor[6] = { 1.0f, 0.2f, 0.3f, 1.0f, 0.3f, 0.4f};
			glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);			
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, position.size()*sizeof(position), &position[0], GL_STATIC_DRAW);
//			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fPoint, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,0);}
	//	glBindVertexArray(uiVAO[0]);}
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_LINES, 0, 2);
		glFlush();

		break;}
	case 3: {// move a point with glBufferSubData
//		Win::log(L"ModelGL::drawGL case 3:");

		static GLfloat fPoint[cPOINT_COORD] = { 0.3f,  0.5f,  0.0f };
		static GLfloat fColor[cPOINT_COORD] = {0.0f,  1.0f,  0.0f };
		static int increment = 1;

		if( count != 3 ){  
	//		Win::log(L"ModelGL::drawGL case 3:");
			count = 3;       // init on first draw
			fPoint[0] = -0.9f;
			glPointSize(5.2f);
			cPOINT_BUFFER_SIZE = cPOINT_COORD*sizeof(float);
			glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);			
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STREAM_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//	glBindVertexArray(uiVAO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fPoint, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
			// use this to increment point positions
		fPoint[0] = 0.00001*increment + fPoint[0]; // move the point
		if( increment > 1000 ){ increment = 0; fPoint[0] = -0.9f; }  // reset the count
		increment++;
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, cPOINT_BUFFER_SIZE, fPoint);
        glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT );
	//	glBindVertexArray(uiVAO[0]);
		glDrawArrays(GL_POINTS, 0, 1);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glFlush();
		break;}
	case 4: {// using uniforms with shader GetProgramID() to change color and move point
		//Win::log(L"ModelGL::drawGL case 4:");
			point_1.x = -1.0f;	point_1.y = 0.0f;
			GLfloat fPoint[cPOINT_COORD] = { point_1.x,  point_1.y,  point_1.y };
		    static float fxOffset = 0.0f;
			static int increment = 1;
			if( count != 4 ){  
				count = 4;       // init on first draw only
				glPointSize(5.2f);
				cPOINT_BUFFER_SIZE = cPOINT_COORD*sizeof(float);
				GLfloat fColor[cPOINT_COORD] = { 1.0f,  0.0f,  0.0f };
				glClearColor(0.2, 0.1, 0.0, bgColor[3]);
				offsetLocation = glGetUniformLocation(spMain.GetProgramID(), "increment");
				glUniform1f(offsetLocation, 0); 
				glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
				glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fPoint, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
				glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STATIC_DRAW);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//	glBindVertexArray(uiVAO[0]);
				glUniform1f(offsetLocation, 0); // reset location 
				fxOffset = 0.0f;
			}
			
			// use this to increment point positions
		fxOffset = 0.00001*increment + fxOffset; // move the point
		if( increment > 1000 ){ increment = 0; fxOffset = 0.0f; }  // reset the count
		increment++;
		glUniform1f(offsetLocation, fxOffset); //Send fxOffset to shader.vert
		glClear(GL_COLOR_BUFFER_BIT );
		glDrawArrays(GL_POINTS, 0, 1); 
		glFlush();
		break;}
	case 5: {// 3 lines using indexed drawing glDrawElements
		if( count != 5 ){  
	//		Win::log(L"ModelGL::drawGL case 5:");
			count = 5;       // init on first draw only	
			glLineWidth(1.1f);
			static const GLushort vert_Indecies[] = { 0, 1, 1, 2, 1, 3, 0, 2, 0, 3, 3, 2 };
			GLfloat fLines[]  =  {	0.0, 0.0, 0.0,  // v0  vertex positions
									0.5, 0.5, 0.0,  // v1
									0.4, 0.5, 0.0,  // v2
									0.5, 0.4, 0.0, }; // v3};
			GLfloat fColor[]  =  {	1.0, 0.0, 0.0,  // v0  vertex colors
									1.0, 1.0, 1.0,  // v1
									0.0, 0.0, 1.0,  // v2
									0.0, 1.0, 0.0, }; // v3};
			glClearColor(0.16, 0.0, 0.0, 1.0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiVBO[2]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vert_Indecies), vert_Indecies, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fLines), fLines, GL_STATIC_DRAW);
			glEnableVertexAttribArray(COLOR);
			glVertexAttribPointer(COLOR , 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fColor), fColor, GL_STATIC_DRAW);
			glEnableVertexAttribArray(POSITION);// attribute index 1 is assigned to color in the  shader.vert									
			glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiVBO[2]);}
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_LINES,  12, GL_UNSIGNED_SHORT, NULL);
		glFlush();

		break;}
	case 6: {   // scales the 3 lines using indexed drawing glDrawElements from routine 5
				// using glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5));
		if( count != 6 ){  
	//		Win::log(L"ModelGL::drawGL case 6:");
			count = 6;       // init on first draw only	
			iModelViewLoc = glGetUniformLocation(spMain.GetProgramID(), "modelViewMatrix");
			glLineWidth(1.1f);
			static const GLushort vert_Indecies[] = { 0, 1, 1, 2, 1, 3, 0, 2, 0, 3, 3, 2 };
			GLfloat fLines[]  =  {	0.0, 0.0, 0.0,  // v0  vertex positions
									0.5, 0.5, 0.0,  // v1
									0.4, 0.5, 0.0,  // v2
									0.5, 0.4, 0.0, }; // v3};
			GLfloat fColor[]  =  {	1.0, 0.0, 0.0,  // v0  vertex colors
									1.0, 1.0, 1.0,  // v1
									0.0, 0.0, 1.0,  // v2
									0.0, 1.0, 0.0, }; // v3};
			glClearColor(0.16, 0.0, 0.0, 1.0);			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiVBO[2]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vert_Indecies), vert_Indecies, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fLines), fLines, GL_STATIC_DRAW);
			glEnableVertexAttribArray(COLOR);
			glVertexAttribPointer(COLOR , 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fColor), fColor, GL_STATIC_DRAW);}

		glEnableVertexAttribArray(POSITION);// attribute index 1 is assigned to color in the														fragment shader
								//layout (location = 1) in vec3 inColor
		glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glBindVertexArray(uiVAO[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiVBO[2]);
		mCurrent = glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5));
		glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_LINES,  12, GL_UNSIGNED_SHORT, NULL);
		glFlush();
		break;}
	case 7: {// Cover page;  just a point
	//	Win::log(L"void FT_Library::initGL()");	

/*				FT_Library ft;
	Win::log(L"void FT_Library::initGL() count == 0 ");
	if(FT_Init_FreeType(&ft) )Win::log(L"Free Type init failed");
	FT_Face ftFace; 
	if(FT_New_Face(ft, "C:\\Windows\\Fonts\\arial.ttf", 0, &ftFace))Win::log(L"FT_New_Face failed");
	if(FT_Set_Pixel_Sizes(ftFace, 0, 32))Win::log(L"FT_Set_Pixel_Sizes failed");
	if(FT_Load_Char(ftFace, 'X', FT_LOAD_RENDER)) Win::log(L"FT_Load_Char failed");
	FT_GlyphSlot g = ftFace->glyph;
*/
			if( count != 7 ){  
	//		Win::log(L"ModelGL::drawGL case 7:");
			count = 7;       // init on first draw only	

//			count++;
			glPointSize(0.2f);
			cPOINT_BUFFER_SIZE = cPOINT_COORD*sizeof(float);
			point_1.x = 0.0f;	point_1.y = 0.0f;
			GLfloat fPoint[cPOINT_COORD] = { point_1.x,  point_1.y,  point_1.y };
			GLfloat fColor[cPOINT_COORD] = { 1.0f,  0.0f,  0.0f };
			glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fPoint, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STATIC_DRAW);}
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT );
		glDrawArrays(GL_POINTS, 0, 1);
		glFlush();
		break;}
	case 8: {   // draw another point
		
			if( count != 8 ){  // count flags  the first pass, Just do this if block once per call
			Win::log(L"ModelGL::drawGL case 8:");
			count = 8;
			glPointSize(30.2f);
			point_1.x = 0.0f;	point_1.y = 0.0f;
			GLfloat fPoint[cPOINT_COORD] = { point_1.x,  point_1.y,  point_1.y };
			GLfloat fColor[cPOINT_COORD] = { 1.0f,  0.0f,  1.0f };
			glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fPoint), fPoint, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, cPOINT_BUFFER_SIZE, fColor, GL_STATIC_DRAW);}
//			Win::log(L"ModelGL::drawGL case 8: count = %i", count);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT );
		glDrawArrays(GL_POINTS, 0, 1);
		glFlush();
		break;}
		case 9: {		
			if( count != 9 ){ 
				glDisable(GL_CULL_FACE);// In case it was turned on in previous routine
				Win::log(L"ModelGL::drawGL case 9:");
				count = 9;
				glClearColor(0.0, 0.1, 0.1, bgColor[3]);
				GLfloat vertices[6][2] = {
					{ -0.90, -0.90 },   // Triangle 1
					{  0.85, -0.90 },
					{ -0.90,  0.85 },
					{  0.90, -0.85 },   // Triangle 2
					{  0.90,  0.90 },
					{ -0.85,  0.90 },};
				glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			    glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
				}

			glClear(GL_COLOR_BUFFER_BIT );
			glEnableVertexAttribArray(0);
	//		glEnableVertexAttribArray(1);  // color is done in shader
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(0);
			glFlush();
		break;
		}// end case 9:{
	case 10: {// Ortho cube	with translation in the shader by uniform	
		if( count != 10 ){  
			Win::log(L"ModelGL::drawGL case 10:");
			count = 10;

const float vertexData[] = {
	 0.25f,  0.25f, 0.75f, 1.0f,
	 0.25f, -0.25f, 0.75f, 1.0f,
	-0.25f,  0.25f, 0.75f, 1.0f,

	 0.25f, -0.25f, 0.75f, 1.0f,
	-0.25f, -0.25f, 0.75f, 1.0f,
	-0.25f,  0.25f, 0.75f, 1.0f,

	 0.25f,  0.25f, -0.75f, 1.0f,
	-0.25f,  0.25f, -0.75f, 1.0f,
	 0.25f, -0.25f, -0.75f, 1.0f,

	 0.25f, -0.25f, -0.75f, 1.0f,
	-0.25f,  0.25f, -0.75f, 1.0f,
	-0.25f, -0.25f, -0.75f, 1.0f,

	-0.25f,  0.25f,  0.75f, 1.0f,
	-0.25f, -0.25f,  0.75f, 1.0f,
	-0.25f, -0.25f, -0.75f, 1.0f,

	-0.25f,  0.25f,  0.75f, 1.0f,
	-0.25f, -0.25f, -0.75f, 1.0f,
	-0.25f,  0.25f, -0.75f, 1.0f,

	 0.25f,  0.25f,  0.75f, 1.0f,
	 0.25f, -0.25f, -0.75f, 1.0f,
	 0.25f, -0.25f,  0.75f, 1.0f,

	 0.25f,  0.25f,  0.75f, 1.0f,
	 0.25f,  0.25f, -0.75f, 1.0f,
	 0.25f, -0.25f, -0.75f, 1.0f,

	 0.25f,  0.25f, -0.75f, 1.0f,
	 0.25f,  0.25f,  0.75f, 1.0f,
	-0.25f,  0.25f,  0.75f, 1.0f,

	 0.25f,  0.25f, -0.75f, 1.0f,
	-0.25f,  0.25f,  0.75f, 1.0f,
	-0.25f,  0.25f, -0.75f, 1.0f,

	 0.25f, -0.25f, -0.75f, 1.0f,
	-0.25f, -0.25f,  0.75f, 1.0f,
	 0.25f, -0.25f,  0.75f, 1.0f,

	 0.25f, -0.25f, -0.75f, 1.0f,
	-0.25f, -0.25f, -0.75f, 1.0f,
	-0.25f, -0.25f,  0.75f, 1.0f,




	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

};
			size_t colorData = sizeof(vertexData)/2;
			glClearColor(0.2, bgColor[1], bgColor[2], bgColor[3]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData,		GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData);
			offsetLocation = glGetUniformLocation(spMain.GetProgramID(), "increment");
			glUniform1f(offsetLocation, 0.5); // tell vert shader to move the cube off center up and to the right 
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glEnable(GL_CULL_FACE);  //enable face culling
			//The triangle data in the tutorial is specifically ordered so that 
			//the clockwise facing of the triangles face out. This prevents the drawing of the
			//rear-facing faces.

			glFrontFace(GL_CW); // Set the clockwise winding side of triangle
								// as the front face
			glCullFace(GL_BACK);// cull the back face of triangles

		
		} // if( count != 10 )
		glClear(GL_COLOR_BUFFER_BIT );
//		size_t colorData = sizeof(vertexData)/2;
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glFlush();
		break; 
	}// end case 10:{
	case 11: {// Procedural cube vertices	
		if( count != 11 ){  
			Win::log(L"ModelGL::drawGL case 11:");
			count = 11;

//  ****************************** Get the positions into uiVBO[0] ********************************
			const GLuint triangle_Count = 12;
			glm::vec3 cubeVerts[3*triangle_Count];
/* v0 */	cubeVerts[0].x = -0.5; cubeVerts[0].y =  0.5; cubeVerts[0].z =  0.5;
/* v1 */	cubeVerts[1].x =  0.5; cubeVerts[1].y =  0.5; cubeVerts[1].z =  0.5;
/* v2 */	cubeVerts[2].x =  0.5; cubeVerts[2].y = -0.5; cubeVerts[2].z =  0.5;
			cubeVerts[3] = cubeVerts[2];
			cubeVerts[4].x = -0.5; cubeVerts[4].y = -0.5; cubeVerts[4].z =  0.5;
			cubeVerts[5] = cubeVerts[0];
			GLfloat vertElements[9*triangle_Count];  // 3 verts/triangle * 3 ver elements/vert
			//for( int j = 0; j < 3; j ++){ // cycle thru the vectors
			int j = 0;
			for( int i = 0; i < 6; i++ ){ // cycle thru the vectors x,y,z components
					vertElements[j] = cubeVerts[i].x;
					vertElements[j+1] = cubeVerts[i].y;
					vertElements[j+2] = cubeVerts[i].z;
					j = j + 3;
				}
			for( int i = 0; i < 18; i++ ){ // cycle thru the vectors x,y,z components
				Win::log(L"vertElements[%i], = %f",i, vertElements[i]);

				}


				glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertElements), vertElements, GL_STATIC_DRAW);
			    glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

				
//  ****************************** Get the colors into uiVBO[1] ********************************
			glm::vec3 vertex_Color[3*triangle_Count];  //  each vertex of each triangle gets a color


/* v0 */	vertex_Color[0].r =	 0.5; vertex_Color[0].g =  0.0; vertex_Color[0].b =  0.0;
			vertex_Color[2] = vertex_Color[1] = vertex_Color[0];
			vertex_Color[3].r =	 0.0; vertex_Color[3].g =  0.5; vertex_Color[3].b =  0.0;
			vertex_Color[5] = vertex_Color[4] = vertex_Color[3];
			
//	vertex_Color[1].r =  0.0; vertex_Color[1].g =  0.5; vertex_Color[1].b =  0.0;
//	vertex_Color[2].r =  0.0; vertex_Color[2].g =  0.0; vertex_Color[2].b =  0.5;


			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_Color), vertex_Color, GL_STATIC_DRAW);
//			Win::log(L"ModelGL::drawGL case 8: count = %i", count);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

//  ****************************** Try to get some matrices working  ********************************

			glm::vec3 axis = glm::vec3 (1.0,0.0,0.0);  //  x,y,z




//			glm::rotate(glm::mat4(1.0f), 90, zaxis);
//			 glm::scale(glm::mat4(1.0f), glm::vec3(u, u, u));

			 glm::vec4 Position = glm::vec4(glm:: vec3(0.0f, -1.0, -0.5), 1.0f); 
		//	if(Position.x == Position.x == Position.y == Position.z == 0.0)
			Win::log(L"Position.x %f Position.y %f Position.z %f Position.w %f",Position.x, Position.y, Position.z, Position.w);
//			 glm::mat4 Model = glm::rotate( glm::mat4(1.0f), glm::vec3(1.0f)); 
			 glm::mat4 Model = glm::rotate( glm::mat4(1.0f), 90.0f,  axis); 
			 Position = Model * Position;
			 Win::log(L"Position.x %f Position.y %f Position.z %f Position.w %f",Position.x, Position.y, Position.z, Position.w);
			// glm::vec4 Transformed = Model * Position;

			} // if( count != 11 )
		glClearColor(0.2, bgColor[1], bgColor[2], bgColor[3]);
			glClear(GL_COLOR_BUFFER_BIT );
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
	//		glEnableVertexAttribArray(1);  // color is done in shader
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glFlush();
			break;
		}

/*	Input: Shape; point, line, triangle, square, circle, tetrahedon, cube sphere ....
	Input: type of OGL primative;  point, line, triangle, ...
	Input: Number of shapes to draw, random, grid, spacing etc
	Procedural glBufferData generation */		
	case 12: {
			if( count != 12 ){  // count flags  the first pass, Just do this if block once per call
	//		Win::log(L"ModelGL::drawGL case 0:");
			count = 12;
			const int grid_size(8);
			int i(0);
	//		glPointSize(0.1f);
			glPointSize(12.1f);
			//float fPoint[3*(2*grid_size + 1)^3];
			//float fColor[3*(2*grid_size + 1)^3];
			float fPoint[14739];
			float fColor[14739];

			//int x(1);
			//fPoint[i] = (float)x/grid_size;
			//fColor[i] = fPoint[i]*fPoint[i];
			//Win::log(L"fPoint[i] =  %f fColor[i] =  %f ",fPoint[i], fColor[i]);

	for(int x = -grid_size; x <= grid_size; x++){
		for(int y = -grid_size; y <= grid_size; y++){
			for(int z = -grid_size; z <= grid_size; z++){
				fPoint[i] = (float)x/(2*grid_size);
				fColor[i] = fPoint[i]*fPoint[i];
				fPoint[i+1] = (float)y/(2*grid_size);
				fColor[i+1] = fPoint[i+1]*fPoint[i+1];
				fPoint[i+2] = (float)z/(2*grid_size);
				fColor[i+2] = fPoint[i+2]*fPoint[i+2];
				i = i +3;
			}
		}
	}

offsetLocation = glGetUniformLocation(spMain.GetProgramID(), "offset");
frustumScaleUnif = glGetUniformLocation(spMain.GetProgramID(), "frustumScale");
zNearUnif = glGetUniformLocation(spMain.GetProgramID(), "zNear");
zFarUnif = glGetUniformLocation(spMain.GetProgramID(), "zFar");
glUniform1f(frustumScaleUnif, 1.0f);
glUniform1f(zNearUnif, 1.0f);
glUniform1f(zFarUnif, 3.0f);
//			GLfloat fColor[cPOINT_COORD] = { 0.0f,  0.0f,  1.0f };
			glClearColor(0.5, bgColor[1], bgColor[2], bgColor[3]);
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fPoint), fPoint, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
/*   glVertexAttribPointer( attribute index of input to shader,
				number of values for each position,
				data type of the position data  eg GL_FLOAT,
				???   GL_FALSE,
				spacing between values,
				byte offset from beginning of buffer for
*/
			glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(fColor), fColor, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		}
			glUniform2f(offsetLocation, 0.25f, 0.25f);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(0);
		glClear(GL_COLOR_BUFFER_BIT );
		glDrawArrays(GL_POINTS, 0, 4913);  // start at 0, and read 1 vertex
		glFlush();
	break;}// end case 12:{
	case 14: { //Procedural Cube Faces

	if( count != 14 ){  // count flags  the first pass, Just do this if block once per call
		count = 14;
		Win::log(L"ModelGL::drawGL case 14:");
		glPointSize(0.0f);
//		offsetLocation = glGetUniformLocation(spMain.GetProgramID(), "offset");
//		glUniform1f(offsetLocation, 0.1);

//		mCurrent = glm::scale(glm::mat4(1.0f), glm::vec3(0.5,0.5,0.5));
//		glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mCurrent));
//			oglControl.setProjection3D(45.0f, float(LOWORD(lParam))/float(HIWORD(lParam)), 0.001f, 1000.0f);
//void setProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar);
//void COpenGLControl::setProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar)
// 35.0f, 1.0f, 0.1f, 100.0f
//glm::mat4 projectionMatrix = glm::perspective(
//    glm::radians(FoV), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
//    4.0f / 3.0f,       // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
//    0.1f,              // Near clipping plane. Keep as big as possible, or you'll get precision issues.
//    100.0f             // Far clipping plane. Keep as little as possible.
//);
/*
As a matter of fact, the order above is what you will usually need for game characters and other items : Scale it first if needed; then set its direction, then translate it. For instance, given a ship model (rotations have been removed for simplification) :

in C++, with GLM :

glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScaleMatrix;
glm::vec4 myTransformedVector = myModelMatrix * myOriginalVector;

We’d like to be able to move this model, maybe because the player controls it with the keyboard and the mouse. Easy, you just learnt do do so : translation*rotation*scale, and done. You apply this matrix to all your vertices at each frame (in GLSL, not in C++!) and everything moves. Something that doesn’t move will be at the center of the world.

Your vertices are now in World Space. This is the meaning of the black arrow in the image below : We went from Model Space (all vertices defined relatively to the center of the model) to World Space (all vertices defined relatively to the center of the world).

glm::mat4 CameraMatrix = glm::lookAt(
    cameraPosition, // the position of your camera, in world space
    cameraTarget,   // where you want to look at, in world space
    upVector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
);
*/
//		mCurrent = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.0f);
//glm::vec3 yaxis = glm::vec3 (z,u,z);

iModelViewLoc =  glGetUniformLocation(spMain.GetProgramID(), "modelViewMatrix");
//glm::vec3 yaxis(0.0,1.0,0.0);
Move = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.0f, 0.0f));
Size = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
//View = glm::lookAt(glm::vec3(0, 12, 60), origin, yaxis);
Spin = glm::rotate( glm::mat4(1.0), 0.0f, yaxis );  //just rotate
Model = glm::mat4(1.0f);
// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

Projection = glm::perspective(glm::radians(half_PI/2.0f),  3.5f / 4.0f, 0.1f, 100.0f);
View = glm::lookAt(
    glm::vec3(4,23,60), // Camera is at (4,3,60), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
// Model = glm::mat4(1.0f);
Model = Move*Spin*Size;
 mvp = Projection * View * Model;


	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		int grid(8);
		cube_triangle_Count = (12*(2*grid )*(2*grid));
		std::vector<Triangle> test(cube_triangle_Count);
		test = cube_Faces(grid);
 		glClearColor(0.2, bgColor[1], bgColor[2], bgColor[3]);
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, test.size()*sizeof(Triangle), &test[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
		glEnableVertexAttribArray(0);

		std::vector<glm::vec3> vertex_Color(3*cube_triangle_Count);  //  each vertex of each triangle gets a color
        for( int i =0; i < 3*cube_triangle_Count; ){
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
			vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; i++;
		}
  //      for( int i =0; i < 3*cube_triangle_Count; i++){
		//	vertex_Color[i].r =	 0.9; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; 
		//}
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_Color.size()*sizeof(glm::vec3), &vertex_Color[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	} // if( count != 14 )

	glClear(GL_COLOR_BUFFER_BIT );
	angle = angle +.0003f;
	if( angle > 10000.0f ) angle = 0.0f;
	Spin = glm::rotate( glm::mat4(1.0), angle, yaxis ); 
	Model = Move*Spin*Size;
	 mvp = Projection * View * Model;
	 glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mvp));

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

glDisable(GL_CULL_FACE);
	 glEnableVertexAttribArray(0);
	 glEnableVertexAttribArray(1);

//glCullFace(GL_BACK);
//	glFrontFace(GL_CW);
//		glEnable(GL_CULL_FACE);
			glBindVertexArray(uiVAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3*cube_triangle_Count);
				glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
	glFlush();
			
	break;
	}// end case 14:{

	
	case 15: { //Equialteral grid mesh 

	if( count != 15 ){
		count = 15;
		glPointSize(0.0f);
		Win::log(L"ModelGL::drawGL case 15:");
iModelViewLoc =  glGetUniformLocation(spMain.GetProgramID(), "modelViewMatrix");
//glm::vec3 yaxis(0.0,1.0,0.0);
Move = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
Size = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
//View = glm::lookAt(glm::vec3(0, 12, 60), origin, yaxis);
Spin = glm::rotate( glm::mat4(1.0), 0.0f, yaxis );  //just rotate
Model = glm::mat4(1.0f);
// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

Projection = glm::perspective(glm::radians(half_PI/2.0f),  3.5f / 4.0f, 0.1f, 100.0f);
View = glm::lookAt(
    glm::vec3(4,23,60), // Camera is at (4,3,60), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
// Model = glm::mat4(1.0f);
Model = Move*Spin*Size;
 mvp = Projection * View * Model;
angle = 0.0f;

	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		int grid(6);
		equi_triangle_Count = 4*grid*grid;// for tetrahedron
//		equi_triangle_Count = 20*grid*grid;
		int equi_verticies_Count = 3*equi_triangle_Count;
		std::vector<Triangle> equiT(equi_triangle_Count);
//		equiT = icosahedron(grid);
		equiT = equilateral(grid);
//		Win::log(L"equiT = equilateral(grid)");
 		glClearColor(0.2, bgColor[1], bgColor[2], bgColor[3]);
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, equiT.size()*sizeof(Triangle), &equiT[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
		glEnableVertexAttribArray(0);

		std::vector<glm::vec3> vertex_Color(equi_verticies_Count);  //  each vertex of each triangle gets a color
		for(int j = 0; j < equi_triangle_Count; j++){
        for( int i = 0; i < 3; i++){
			if (j % 2 == 0 ){
			vertex_Color[3*j + i].r =	 0.9; vertex_Color[3*j + i].g =  0.0; vertex_Color[3*j + i].b =  0.0;
			}
			else {
			vertex_Color[3*j + i].r =	 0.0; vertex_Color[3*j + i].g =  0.5; vertex_Color[3*j + i].b =  0.0;
			}
		}
		}
		//int j;
		//for( int i = 0; i < equi_triangle_Count; i++){
		//	if (i % grid*grid == 0 ){
		//		for(int j = 0; j < 3; j++){
		//			vertex_Color[3*i+j].r =	 0.0; vertex_Color[3*i+j].g =  0.4; vertex_Color[3*i+j].b =  0.0;
		//		}
		//	}
		//}
  //     for( int i =0; i < equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 1.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  0.0; 
		//}
  //     for( int i = equi_verticies_Count/10; i < 2*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.0; vertex_Color[i].g =  0.0; vertex_Color[i].b =  1.0; 
		//}
  //     for( int i = 2*equi_verticies_Count/10; i < 3*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.0; vertex_Color[i].g =  1.0; vertex_Color[i].b =  0.0; 
		//}
  //     for( int i = 3*equi_verticies_Count/10; i < 4*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.7; vertex_Color[i].g =  0.5; vertex_Color[i].b =  0.2; 
		//}
  //      for( int i = 4*equi_verticies_Count/10; i < 5*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.8; vertex_Color[i].g =  0.8; vertex_Color[i].b =  1.0; 
		//}
  //     for( int i = 5*equi_verticies_Count/10; i < 6*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.0; vertex_Color[i].g =  0.8; vertex_Color[i].b =  0.5; 
		//}
	 //  for( int i = 6*equi_verticies_Count/10; i < 7*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.0; vertex_Color[i].g =  1.0; vertex_Color[i].b =  0.0; 
		//}
  //     for( int i = 7*equi_verticies_Count/10; i < 8*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.7; vertex_Color[i].g =  0.5; vertex_Color[i].b =  0.2; 
		//}
  //      for( int i = 8*equi_verticies_Count/10; i < 9*equi_verticies_Count/10; i++){
		//	vertex_Color[i].r =	 0.8; vertex_Color[i].g =  0.8; vertex_Color[i].b =  1.0; 
		//}
  //     for( int i = 9*equi_verticies_Count/10; i < equi_verticies_Count; i++){
		//	vertex_Color[i].r =	 0.0; vertex_Color[i].g =  0.8; vertex_Color[i].b =  0.5; 
		//}
  
  //     for( int i = 6*equi_verticies_Count/7; i < equi_verticies_Count; i++){
		//	vertex_Color[i].r =	 0.6; vertex_Color[i].g =  0.8; vertex_Color[i].b =  0.5; 
		//}

		Win::log(L"vertex_Color(equi_triangle_Count);");
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_Color.size()*sizeof(glm::vec3), &vertex_Color[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}
//	glBindVertexArray(uiVAO[0]);
	glClear(GL_COLOR_BUFFER_BIT );
	angle = angle +.0003f;
	if( angle > 10000.0f ) angle = 0.0f;
	Spin = glm::rotate( glm::mat4(1.0), angle, glm::vec3 (0.0f,1.0f,0.0f) ); 
//	Spin = glm::rotate( glm::mat4(1.0), 0.0f, yaxis );
	Model = Move*Spin*Size;
	mvp = Projection * View * Model;
	glUniformMatrix4fv(iModelViewLoc, 1, GL_FALSE, glm::value_ptr(mvp));

//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

glDisable(GL_CULL_FACE);
//glCullFace(GL_BACK);
//	glFrontFace(GL_CW);
//		glEnable(GL_CULL_FACE);
		//glEnableVertexAttribArray(1);
		//		glEnableVertexAttribArray(0);
//glBindVertexArray(uiVAO[0]);
glBindVertexArray(uiVAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3*equi_triangle_Count);
			//	glDisableVertexAttribArray(0);
			//glDisableVertexAttribArray(1);
			glBindVertexArray(0);
	glFlush();

	break;
	}// end case 15:{
	case 16: { //Icosahedral grid mesh 
	if( count != 16 ){
		count = 16;	
		Win::log(L"ModelGL::drawGL case 16:");
	//	 float half_pi = asin(1.0f);
 //float pi = 2*half_pi;
 //float epsilon = atan(3.0f*pi/10.0f);
 //float H = sin(epsilon);
	}
	break;
	}// end case 16:{
	default: {// none of the above.
			//  Win::log(L"ModelGL::drawGL case default:");
			count = -1;
		    glClearColor(0.0, bgColor[1], bgColor[2], bgColor[3]);
			glClear(GL_COLOR_BUFFER_BIT );
			glFlush();
		} 
	} // switch	
}

//   WM_SIZE triggers this call by ControllerGL
void ModelGL::resizeWindow(int w, int h) // flag window for a glViewport call
{ 
	Win::log(L"void resizeWindow(int w = %i, int h = %i)", w, h );
    windowWidth = w;		// Get the width/height of the window
    windowHeight = h;
    windowResized = true;	// Flag viewport call
}

//void ModelGL::setViewport(int w, int h)
//{
//    // assign the width/height of viewport
//    windowWidth = w;
//    windowHeight = h;
//    // set viewport to be the entire window
//    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//}
bool ModelGL::ShaderLoad::LoadShader(string sFile, int a_iType)
{
Win::log(L"ModelGL::LoadShader()");

	FILE* fp = fopen(sFile.c_str(), "rt"); // r = read  t = text

	if(!fp){
		Win::log(L"failed LoadShader()");
		return false;
	}
	// Get all lines from a file

	vector<string> sLines;
	char sLine[255];
	while(fgets(sLine, 255, fp))sLines.push_back(sLine);
	fclose(fp);

	const char** sProgram = new const char*[ (int)sLines.size() ];

	for( int i = 0; i <(int)sLines.size(); i++ )
		sProgram[i] = sLines[i].c_str(); 
// get a name for the shader	
	uiShader = glCreateShader(a_iType); // vertex, geometry, fragment
// Loads source code into shader named uishader
	glShaderSource(uiShader, (int)sLines.size(), sProgram, NULL);
	glCompileShader(uiShader);

	delete[] sProgram;

	int iCompilationStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE){
		if( a_iType == GL_VERTEX_SHADER ){
			Win::log(L"LoadShader() VERTEX_SHADER iCompilationStatus == GL_FALSE");}
		if( a_iType == GL_FRAGMENT_SHADER ){
			Win::log(L"LoadShader() FRAGMENT_SHADER iCompilationStatus == GL_FALSE");}
		GLint log_length = 0;
		glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH, &log_length);
		Win::log(L"GL_INFO_LOG_LENGTH = %i",log_length);
		string glsl_Log;
		char* log = (char*)malloc(log_length);
		glGetShaderInfoLog(uiShader, log_length, NULL, log);
		glsl_Log.clear();
		glsl_Log.append(log);
		free(log);

		if(log_length > 60){ // break up glsl_Log into lines for Log Window
			string temp;  
			int index = 0;
			while( index < log_length - 60 ){
					temp.clear();
					temp.append( glsl_Log, index,  60 );
					index = index + 60;
					Win::log(temp.c_str());
			}
		temp.clear();
		temp.append( glsl_Log, glsl_Log.size() - 60,  60 );
		Win::log(temp.c_str());
		}
		else Win::log(glsl_Log.c_str()); // have one line ShaderInfoLog
		return false;
	}// if(iCompilationStatus == GL_FALSE)
	iType = a_iType;
	bLoaded = true;

	return 1;
}
void ModelGL::ShaderLoad::DeleteShader()
{
	if(!IsLoaded())return;
	bLoaded = false;
	glDeleteShader(uiShader);
}
bool ModelGL::ShaderLoad::IsLoaded()
{
	return bLoaded;
}
UINT ModelGL::ShaderLoad::GetShaderID()
{
	return uiShader;
}
void  ModelGL::Shader::CreateProgram()
{
	uiProgram = glCreateProgram();
 
}
void ModelGL::Shader::DeleteProgram()
{
	if(!bLinked)return;
	bLinked = false;
	glDeleteProgram(uiProgram);
}
bool ModelGL::Shader::AddShaderToProgram(ShaderLoad* shShader)
{
	if(!shShader->IsLoaded())return false;

	glAttachShader(uiProgram, shShader->GetShaderID());

	return true;
}
bool ModelGL::Shader::LinkProgram()
{
	glLinkProgram(uiProgram);
	int iLinkStatus;
	glGetProgramiv(uiProgram, GL_LINK_STATUS, &iLinkStatus);
	bLinked = iLinkStatus == GL_TRUE;
	return bLinked;
}
void ModelGL::Shader::UseProgram()
{
	if(bLinked)glUseProgram(uiProgram);

}
/**
 * Display compilation errors from the OpenGL shader compiler
 */

/*
void print_log(GLuint object)
{
  GLint log_length = 0;
  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }
 
  char* log = (char*)malloc(log_length);
 
  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);
 
  fprintf(stderr, "%s", log);
  free(log);
}
*/
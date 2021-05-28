//Standard libraries include
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Maths library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//include some custom code files
#include "glfunctions.h" //include all OpenGL stuff
#include "Shader.h" // class to compile shaders
#include "imageloader.h"

//Libraries
#include <string>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;
using namespace glm;


//Mouse Variables
float Yaw = -90.0f;
float Pitch = 0.0f;
bool firstMouse = true;
int g_ViewportWidth = 1536; int g_ViewportHeight = 2048;
double mouse_x = 512.0;
double mouse_y = 512.0;
float lastX = 1024 / 2;
float lastY = 1024 / 2;


//Planet information variables
const char *planetBMP[11] = { "assets/sun.bmp", "assets/mercury.bmp","assets/venus.bmp",
							  "assets/earthmap1k.bmp","assets/moonmap1k.bmp","assets/mars.bmp",
							  "assets/jupiter.bmp", "assets/saturn_rings.bmp" , "assets/saturn.bmp","assets/uranus.bmp",
							  "assets/neptune.bmp" };

const char *planetNormalBMP[11] = { "assets/sun.bmp", "assets/mercuryNormal.bmp","assets/venusNormal.bmp",
							  "assets/earthnormal.bmp","assets/moonNormal.bmp","assets/marsNormal.bmp",
							  "assets/jupiterNormal.bmp", "assets/saturn_rings.bmp" , "assets/saturnNormal.bmp","assets/uranusNormal.bmp",
							  "assets/neptuneNormal.bmp" };

const char* planetNames[11] = {"Sun", "Mercury","Venus",
							  "Earth","Moon","Mars",
							  "Jupiter", "Saturn Rings" , "Saturn","Uranus",
							  "Neptune" };

//Planet Variables
GLuint g_simpleShader[11];
GLuint g_Vao[11];
GLuint g_NumTriangles[11];
GLuint texture[11];
GLuint texture_id_normal[11];
GLuint earthSpecId;
GLuint earthNightId;

//Clouds Variables
GLuint g_shaderClouds;
GLuint g_Vao_Clouds;
GLuint g_NumTrianglesClouds;
GLuint textureClouds;


//Planet Details
float radisPlanetes[11] = { 0,  //SUN
						  30, //MERCURY
						  20, //VENUS
						  15, //EARTH
						  8, //MOON
						  20, //MARS
						  11, //JUPITER
						  0,  //ANELLLS D MERDA
						  17, //SATURNO
						  40, //URANO
						  43 }; //NEPTUNO

//Orbital speeds are real orbital speeds divided by 40 (by 10, and by 4)
float orbitalSpeedPlanets[11] = {
						  0.001, //SUN
						  4.787/4, //MERCURY
						  3.502/4, //VENUS
						  2.978/4, //EARTH
						  7.0/2,  //MOON
						  2.4077/4, //MARS
						  1.207/4, //JUPITER
						  0.01/4, //ANELLS D MERDA
						  0.969/4, //SATURNO
						  0.681/4, //URANO
						  0.543/4 }; //NEPTUNO

float scalePlanets[11] = { 10, 	//SUN
						   0.5, //MERCURY
						   1,   //VENUS
						   2, 	//EARTH
						   0.5, //MOON
						   2, 	//MARS
						   5,  	//JUPITER
						   0.0, //ANELLS DE MERDA
						   4, 	//SATURNO
						   2, 	//URANO
						   2 };	//NEPTUNO


//VARIABLES SKYBOX
GLuint g_simpleShader_sky = 0;
GLuint g_Vao_sky = 0;
GLuint g_NumTriangles_sky = 0;
vec3 center = vec3(0.0f, 0.0f, -1.0f);
vec3 eye = vec3(0.0f, 0.0f, 0.0f);
GLuint texture_id_sky;


//Asset Imports
std::string basepath = "assets/";
std::string inputfile = basepath + "sphere.obj";
std::vector< tinyobj::shape_t > shapes;
std::vector< tinyobj::material_t > materials;
std::string err;
bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), basepath.c_str());

string inputfile2 = basepath + "sphere.obj";
vector< tinyobj::shape_t > shapes2;
vector< tinyobj::material_t > materials2;


//Camera variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 cameraPos = glm::vec3(19.9444f, 15.7415f, 73.093f);
glm::vec3 cameraFront = glm::vec3(-0.00349026, -0.0139622, -0.999896);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat* uvs = &(shapes[0].mesh.texcoords[0]);
GLfloat* uvn = &(shapes[0].mesh.texcoords[1]);
GLuint uvs_size = shapes[0].mesh.texcoords.size() * sizeof(GLfloat);

//Light direction variable
vec3 g_light_dir(0, -10, -100);


// ------------------------------------------------------------------------------------------
// This function loads the planet in the position passed as an argument
// ------------------------------------------------------------------------------------------
void loadPlanet(int pos){

	//load the shader
	//If the planet is sun load it's own shader, else load the shader they themselves have
	if(pos == 0){
		Shader simpleShaderSun("src/sunShader.vert", "src/sunShader.frag");
		g_simpleShader[0] = simpleShaderSun.program;
	}else{
		Shader simpleShader("src/shader.vert", "src/shader.frag");
		g_simpleShader[pos] = simpleShader.program;
	}

	// Create the VAO where we store all geometry
	g_Vao[pos] = gl_createAndBindVAO();

	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader[pos], "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	gl_createAndBindAttribute(uvs, uvs_size, g_simpleShader[pos], "a_uv", 2);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[pos], "a_normal", 3);

	//normals
	gl_createAndBindAttribute(uvn, uvs_size, g_simpleShader[pos], "a_normal", 3);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[pos], "a_normal", 3);


	//unbind everything
	gl_unbindVAO();

	//store number of triangles (use in draw())
	g_NumTriangles[pos] = shapes[0].mesh.indices.size() / 3;

	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	//print out number of meshes described in file


	//Draw planet texture
	Image* image = loadBMP(planetBMP[pos]);

	glGenTextures(1, &texture[pos]);
	glBindTexture(GL_TEXTURE_2D, texture[pos]);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);

	//If it's not the sun, draw the normal texture to
	if(pos>0){
		Image* image_normal = loadBMP(planetNormalBMP[pos]);
		glGenTextures(1, &texture_id_normal[pos]); glBindTexture(GL_TEXTURE_2D, texture_id_normal[pos]); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D,0, GL_RGB, image_normal->width, image_normal ->height, 0, GL_RGB, GL_UNSIGNED_BYTE,image_normal->pixels);
	}

}

// ------------------------------------------------------------------------------------------
// This function draws the planet in the position passed as an argument
// ------------------------------------------------------------------------------------------
void drawPlanet(int pos){
	//Enable depth dest
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// activate shader
	glUseProgram(g_simpleShader[pos]);
	mat4 projection_matrix = perspective(
		45.0f,
		1.0f,
		0.1f,
		300.0f
	);

	//Setup projection matrix
	GLuint projection_loc = glGetUniformLocation(g_simpleShader[pos], "u_projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(scalePlanets[pos], scalePlanets[pos], scalePlanets[pos]));

	//Get current position in time
	float x = sin(glfwGetTime() * orbitalSpeedPlanets[pos] / 5) * radisPlanetes[pos];
    float z = cos(glfwGetTime() * orbitalSpeedPlanets[pos] / 5) * radisPlanetes[pos];

	//If it's the moon, get earth's position, and move according to it
	if(pos == 4){
		float xEarth = sin(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * radisPlanetes[3];
    	float zEarth = cos(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * radisPlanetes[3];
		model = glm::translate(model,  glm::vec3((xEarth*4)+x, 0.0f, (zEarth*4)+z));
	}else{
		//If it's not the moon, simply move the planet to the central position, and then apply translate from that position
		model = glm::translate(model,  glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(x, 0.0f, z));
	}

	//Rotate on its axis
	model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(10.5f), glm::vec3(0.0f, 1.0f, 0.0f));

	GLuint model_loc = glGetUniformLocation(g_simpleShader[pos], "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	mat3 normal_mat = mat3(transpose(inverse(model)));
	GLuint normal_loc = glGetUniformLocation(g_simpleShader[pos], "u_normal");
	glUniformMatrix3fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));


	glm::mat4 view_matrix = glm::mat4(1.0f);


	view_matrix = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

	GLuint view_loc = glGetUniformLocation(g_simpleShader[pos], "u_view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

	//Setup light direction
	GLuint u_light_dir = glGetUniformLocation(g_simpleShader[pos], "u_light");
	if(pos==3){
		glUniform3f(u_light_dir, x, 0, z);
	}else if(pos == 4){
		float xEarth = sin(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * radisPlanetes[3];
    	float zEarth = cos(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * radisPlanetes[3];
		glUniform3f(u_light_dir, xEarth*-1, 0, zEarth*(-1));
	}else{
		glUniform3f(u_light_dir, x*-1, 0, z*(-1));
	}

	GLuint u_light_color = glGetUniformLocation(g_simpleShader[pos], "u_light_color");
	glUniform3f(u_light_color, 1.0, 1.0, 1.0);

	GLuint u_cam_pos = glGetUniformLocation(g_simpleShader[pos], "u_cam_pos");
	glUniform3f(u_cam_pos, cameraPos.x, cameraPos.y, cameraPos.z);


	GLuint u_ambient = glGetUniformLocation(g_simpleShader[pos], "u_ambient");
	glUniform1f(u_ambient, 0.1);

	//Bind planet texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[pos]);

	GLuint u_texture = glGetUniformLocation(g_simpleShader[pos], "u_texture");
	glUniform1i(u_texture, 0);


	//Bind normal texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_id_normal[pos]);

	GLuint u_texture_normal = glGetUniformLocation(g_simpleShader[pos], "u_texture_normal");
	glUniform1i(u_texture_normal, 1);


	//If the planet is earth
	if(pos==3){

		//Bind specular texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, earthSpecId);

		GLuint u_texture_spec = glGetUniformLocation(g_simpleShader[pos], "u_texture_spec");
		glUniform1i(u_texture_spec,2);

		//Bind night texture
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, earthNightId);

		GLuint u_texture_night = glGetUniformLocation(g_simpleShader[pos], "u_texture_night");
		glUniform1i(u_texture_night, 3);


	}

	glUseProgram(g_simpleShader[pos]);
	gl_bindVAO(g_Vao[pos]);



	// Draw to screen
	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles[pos], GL_UNSIGNED_INT, 0);
}

// ------------------------------------------------------------------------------------------
// This function draws the sun
// ------------------------------------------------------------------------------------------
void drawSun(){

	//clear the screen
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	//Set position to 0,0,0
	struct pos {
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
	};
	pos p;

	// activate shader
	glUseProgram(g_simpleShader[0]);
	mat4 projection_matrix = perspective(
		45.0f,
		1.0f,
		0.1f,
		300.0f
	);

	//Setup projection matrix
	GLuint projection_loc = glGetUniformLocation(g_simpleShader[0], "u_projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


	glm::mat4 model = glm::mat4(1.0f);
	model = translate(model, vec3(p.X, p.Y, p.Z));

	//Set scale
	model = glm::scale(model, glm::vec3(scalePlanets[0], scalePlanets[0], scalePlanets[0]));

	//Rotate on it's axis
	model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GLuint model_loc = glGetUniformLocation(g_simpleShader[0], "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 view_matrix = glm::mat4(1.0f);

	view_matrix = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

	GLuint view_loc = glGetUniformLocation(g_simpleShader[0], "u_view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


	int u_texture = glGetUniformLocation(g_simpleShader[0], "u_texture");

	glUseProgram(g_simpleShader[0]);
	//bind the geometry
	gl_bindVAO(g_Vao[0]);


	glUniform1i(u_texture, 0);

	//Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);


	// Draw to screen
	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles[0], GL_UNSIGNED_INT, 0);

}

// ------------------------------------------------------------------------------------------
// This function loads the earth
// ------------------------------------------------------------------------------------------
void loadTerra()
{

	//load the shader
	Shader simpleShader("src/shaderEarth.vert", "src/shaderEarth.frag");
	g_simpleShader[3] = simpleShader.program;

	// Create the VAO where we store all geometry (stored in g_Vao)
	g_Vao[3] = gl_createAndBindVAO();


	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader[3], "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	gl_createAndBindAttribute(uvs, uvs_size, g_simpleShader[3], "a_uv", 2);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[3], "a_normal", 3);


	//normals
	gl_createAndBindAttribute(uvn, uvs_size, g_simpleShader[3], "a_normal", 3);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[3], "a_normal", 3);


	//unbind everything
	gl_unbindVAO();

	//store number of triangles (use in draw())
	g_NumTriangles[3] = shapes[0].mesh.indices.size() / 3;

	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	//print out number of meshes described in file

	Image* image = loadBMP("assets/earthmap1k.bmp");

	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);

	//Load normal texture
	Image* image_normal = loadBMP("assets/earthnormal.bmp");
	glGenTextures(1, &texture_id_normal[3]);
	glBindTexture(GL_TEXTURE_2D, texture_id_normal[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D,0, GL_RGB, image_normal->width, image_normal ->height, 0, GL_RGB, GL_UNSIGNED_BYTE,image_normal->pixels);

	//Load specular texture
	Image* image_spec = loadBMP("assets/earthspec.bmp");
	glGenTextures(1, &earthSpecId);
	glBindTexture(GL_TEXTURE_2D, earthSpecId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D,0, GL_RGB, image_spec->width, image_spec ->height, 0, GL_RGB, GL_UNSIGNED_BYTE,image_spec->pixels);

	//Load earth at night texture
	Image* image_night = loadBMP("assets/earthnight.bmp");
	glGenTextures(1, &earthNightId);
	glBindTexture(GL_TEXTURE_2D, earthNightId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D,0, GL_RGB, image_night->width, image_night ->height, 0, GL_RGB, GL_UNSIGNED_BYTE,image_night->pixels);


}

// ------------------------------------------------------------------------------------------
// This function loads the sun
// ------------------------------------------------------------------------------------------
void loadSun(){


	//load the shader
	Shader simpleShaderSun("src/sunShader.vert", "src/sunShader.frag");
	g_simpleShader[0] = simpleShaderSun.program;


	// Create the VAO where we store all geometry (stored in g_Vao)
	g_Vao[0] = gl_createAndBindVAO();

	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader[0], "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	gl_createAndBindAttribute(uvs, uvs_size, g_simpleShader[0], "a_uv", 2);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[0], "a_normal", 3);


	//normals
	gl_createAndBindAttribute(uvn, uvs_size, g_simpleShader[0], "a_normal", 3);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader[0], "a_normal", 3);


	//unbind everything
	gl_unbindVAO();

	//store number of triangles (use in draw())
	g_NumTriangles[0] = shapes[0].mesh.indices.size() / 3;

	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	//print out number of meshes described in file

	Image* image = loadBMP("assets/sun.bmp");

	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);

}

// ------------------------------------------------------------------------------------------
// This function loads the moon
// ------------------------------------------------------------------------------------------
void loadLuna()
{

	//load the shader
	Shader simpleShaderSun("src/shader.vert", "src/shader.frag");
	texture_id_normal[4] = simpleShaderSun.program;


	// Create the VAO where we store all geometry (stored in g_Vao)
	g_Vao[4] = gl_createAndBindVAO();


	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), texture_id_normal[4], "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	gl_createAndBindAttribute(uvs, uvs_size, texture_id_normal[4], "a_uv", 2);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), texture_id_normal[4], "a_normal", 3);


	//normals
	gl_createAndBindAttribute(uvn, uvs_size, texture_id_normal[4], "a_normal", 3);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), texture_id_normal[4], "a_normal", 3);

	//unbind everything
	gl_unbindVAO();

	//store number of triangles (use in draw())
	g_NumTriangles[4] = shapes[0].mesh.indices.size() / 3;

	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	//print out number of meshes described in file

	//Load texture
	Image* imageSun = loadBMP("assets/moonmap1k.bmp");

	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		imageSun->width,
		imageSun->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		imageSun->pixels);

	//Load normal texture
	Image* image_normal = loadBMP("assets/moonNormal.bmp");
	glGenTextures(1, &texture_id_normal[4]); glBindTexture(GL_TEXTURE_2D, texture_id_normal[4]); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D,0, GL_RGB, image_normal->width, image_normal ->height, 0, GL_RGB, GL_UNSIGNED_BYTE,image_normal->pixels);


}

// ------------------------------------------------------------------------------------------
// This function loads the earth's clouds
// ------------------------------------------------------------------------------------------
void loadCloudsEarth(){


	Shader simpleShaderSun("src/shaderClouds.vert", "src/shaderClouds.frag");
	g_shaderClouds = simpleShaderSun.program;

	// Create the VAO where we store all geometry (stored in g_Vao_Clouds)
	g_Vao_Clouds = gl_createAndBindVAO();

	//create vertex buffer for positions, colors, and indices, and bind them to shader
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_shaderClouds, "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));

	gl_createAndBindAttribute(uvs, uvs_size, g_shaderClouds, "a_uv", 2);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_shaderClouds, "a_normal", 3);


	//normals
	gl_createAndBindAttribute(uvn, uvs_size, g_shaderClouds, "a_normal", 3);
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), g_shaderClouds, "a_normal", 3);


	//unbind everything
	gl_unbindVAO();

	//store number of triangles
	g_NumTrianglesClouds = shapes[0].mesh.indices.size() / 3;

	//test it loaded correctly
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	//Load texture
	Image* image = loadBMP("assets/nubeh.bmp");

	glGenTextures(1, &textureClouds);
	glBindTexture(GL_TEXTURE_2D, textureClouds);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);


}

// ------------------------------------------------------------------------------------------
// This function draws the earth's clouds
// ------------------------------------------------------------------------------------------
void drawCloudsEarth(){

	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable alpha blending
	glEnable(GL_BLEND);

	//Make blend func delete the blacks
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	// activate shader
	glUseProgram(g_shaderClouds);
	mat4 projection_matrix = perspective(
		45.0f,
		1.0f,
		0.1f,
		300.0f
	);

	//Setup shader
	GLuint projection_loc = glGetUniformLocation(g_shaderClouds, "u_projection");
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	glm::mat4 model = glm::mat4(1.0f);
	//Set scale slightly bigger than earth
	model = glm::scale(model, glm::vec3(2.05, 2.05, 2.05));


	//Set position to same as earths
	float x = sin(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * (radisPlanetes[3]-0.375);
    float z = cos(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * (radisPlanetes[3]-0.375);

	model = glm::translate(model,  glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(x, 0.0f, z));

	model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(-5.5f), glm::vec3(0.0f, 1.0f, 0.0f));

	GLuint model_loc = glGetUniformLocation(g_shaderClouds, "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	mat3 normal_mat = mat3(transpose(inverse(model)));
	GLuint normal_loc = glGetUniformLocation(g_shaderClouds, "u_normal");
	glUniformMatrix3fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));


	glm::mat4 view_matrix = glm::mat4(1.0f);

	view_matrix = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

	//Setting up shdaer variables
	GLuint view_loc = glGetUniformLocation(g_shaderClouds, "u_view");
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


	float xEarth = sin(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * (radisPlanetes[3]);
    float zEarth = cos(glfwGetTime() * orbitalSpeedPlanets[3] / 5) * (radisPlanetes[3]);

	GLuint u_light_dir = glGetUniformLocation(g_shaderClouds, "u_light");
	glUniform3f(u_light_dir, xEarth*-1, 0, zEarth*(-1));

	GLuint u_light_color = glGetUniformLocation(g_shaderClouds, "u_light_color");
	glUniform3f(u_light_color, 1.0, 1.0, 1.0);

	GLuint u_alpha = glGetUniformLocation(g_shaderClouds, "u_alpha");
	glUniform1f(u_alpha, 1.0);

	GLuint u_cam_pos = glGetUniformLocation(g_shaderClouds, "u_cam_pos");
	glUniform3f(u_cam_pos, cameraPos.x, cameraPos.y, cameraPos.z);


	GLuint u_ambient = glGetUniformLocation(g_shaderClouds, "u_ambient");
	glUniform1f(u_ambient, 0.1);

	//Load texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureClouds);

	GLuint u_texture = glGetUniformLocation(g_shaderClouds, "u_texture");
	glUniform1i(u_texture, 0);


	glUseProgram(g_shaderClouds);
	gl_bindVAO(g_Vao_Clouds);


	// Draw to screen
	glDrawElements(GL_TRIANGLES, 3 * g_NumTrianglesClouds, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);
}

// ------------------------------------------------------------------------------------------
// This function loads the skybox *Not working*
// ------------------------------------------------------------------------------------------
void loadSkyBox(){

	//Load sphere
	ret = tinyobj::LoadObj(shapes2, materials2, err, inputfile2.c_str(), basepath.c_str());

	//Set shader
	Shader simpleShader_sky("src/shader_sky.vert", "src/shader_sky.frag");
	g_simpleShader_sky = simpleShader_sky.program;

	//Bind vao
	g_Vao_sky = gl_createAndBindVAO();

	//Load shader
	gl_createAndBindAttribute(&(shapes2[0].mesh.positions[0]),shapes2[0].mesh.positions.size() * sizeof(float), g_simpleShader_sky, "a_vertex", 3);
	gl_createIndexBuffer(&(shapes2[0].mesh.indices[0]),shapes2[0].mesh.indices.size() * sizeof(unsigned int));
	gl_createAndBindAttribute( &(shapes2[0].mesh.texcoords[0]),shapes2[0].mesh.texcoords.size()*sizeof(GLfloat), g_simpleShader_sky, "a_uv", 2);

	//Set num triangles
	g_NumTriangles_sky = shapes2[0].mesh.indices.size() / 3;

	//Load texture
	Image* image = loadBMP("assets/milkyWay.bmp");
	glGenTextures(1, &texture_id_sky);
	glBindTexture(GL_TEXTURE_2D, texture_id_sky);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB,GL_UNSIGNED_BYTE, image->pixels);

	gl_unbindVAO();

}

// ------------------------------------------------------------------------------------------
// This function is draws the SkyBox *Not working*
// ------------------------------------------------------------------------------------------
void drawSkyBox(){
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glUseProgram(g_simpleShader_sky);

	GLuint u_model = glGetUniformLocation(g_simpleShader_sky, "u_model");
	GLuint u_view = glGetUniformLocation(g_simpleShader_sky, "u_view");
	GLuint u_projection = glGetUniformLocation(g_simpleShader_sky, "u_projection");

	//set MVP
	mat4 model_matrix = translate(mat4(1.0f), eye);
	mat4 view_matrix = lookAt(eye, center, vec3(0, 1, 0));
	mat4 projection_matrix = glm::perspective(120.0f, (float)(g_ViewportWidth / g_ViewportHeight), 0.1f, 50.0f);

	//send all values to shader
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model_matrix));
	glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	GLuint u_texture = glGetUniformLocation(g_simpleShader_sky, "u_texture");
	glUniform1i(u_texture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id_sky);

    gl_bindVAO(g_Vao_sky);

	glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles_sky, GL_UNSIGNED_INT, 0);


	glEnable(GL_DEPTH_TEST);
}

// ------------------------------------------------------------------------------------------
// This function is my attempt at collision detection
// ------------------------------------------------------------------------------------------
GLboolean isNotCollision(glm::vec3 position){

	GLboolean var = true;

	//Iterate for all planets searching if one is colliding
	for(int i = 0; i<11;i++){
		if(i==7||i==4){
			i++;
		}
		float x = sin(glfwGetTime() * orbitalSpeedPlanets[i] / 5) * radisPlanetes[i];
		float z = cos(glfwGetTime() * orbitalSpeedPlanets[i] / 5) * radisPlanetes[i];
		float maxX = abs(round(position.x/scalePlanets[i]))+1;
		float minX = abs(round(position.x/scalePlanets[i]))-1;
		float maxZ = abs(round(position.z/scalePlanets[i]))+1;
		float minZ = abs(round(position.z/scalePlanets[i]))-1;
		//If current position is within bounds of [-1, 1] of the planets current position, i consider it a collision
		if((abs(x)>minX && abs(x)<maxX) && (abs(z)>minZ && abs(z)<maxZ) && (position.y>-5 && position.y<5)){
			var = false;
		}
	}


	return var;
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset;
	float yoffset;
	float sensitivity = 0.1f;

	//If this is the first time mouse is moved, store it's values
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		xoffset = 0;
		yoffset = 0;
		firstMouse = false;
	}else{
		//If it isn't the first time then set offset values and calculate yaw and pitch
		xoffset = xpos - lastX;
		yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;



		Yaw += xoffset;
		Pitch += yoffset;
	}

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (Pitch > 70.0f)
		Pitch = 70.0f;
	if (Pitch < -70.0f)
		Pitch = -70.0f;

	//Calculate new camera front
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);


}

// ------------------------------------------------------------------------------------------
// This function is called every time you press a key
// ------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {

	//If escape key is pressed, close window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Store position before changes
	glm::vec3 oldPos = cameraPos;

	//Set camera speed
	float cameraSpeed = 20 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;

	//Check if moving this way would me colliding with planet
	if(!isNotCollision(cameraPos)){
		//If collision is detected, don't move (set current position to previous)
		cameraPos = oldPos;
	}

}




int main(void)
{

	//setup window, defined in glfunctions.cpp
	GLFWwindow* window;
	if (!glfwInit())return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(g_ViewportWidth, g_ViewportHeight, "Victor Xirau - P3", NULL, NULL);
	if (!window) { glfwTerminate();	return -1; }
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	//input callbacks
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	//Load earth and various miscellaneous like skybox and clouds
	loadTerra();
	loadSkyBox();
	loadLuna();
	loadCloudsEarth();

	//Load entirety of planets
	for(int i=0; i<11 ;i++){
		if(i==7 ||i==3||i==4){
			i++;
		}
		loadPlanet(i);
	}

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		//Clear bit buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Draw skybox
		drawSkyBox();

		//Draw all planets except sun
		for(int i=0; i<11 ;i++){
			if(i==7 ||i==0){
				i++;
			}
			drawPlanet(i);
		}
		//Draw earth's clouds
		drawCloudsEarth();
		//Draw sun
		drawSun();
		// Swap front and back buffers
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();

	}
	//terminate glfw and exit
	glfwTerminate();
	return 0;
}

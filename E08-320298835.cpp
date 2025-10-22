/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Sphere.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Vocho;
Model Llanta_V;
Model Lampara;
Model Cofre;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
// para declarar varias luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];

//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];

//Segundo Arreglo de luces puntuales
PointLight pointLights2[MAX_POINT_LIGHTS];//MAX_POINT_LIGHTS es el mismo o diferente?

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

Sphere sp = Sphere(1.0, 20, 20); // Para la rticulación del cofre

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23
	};

	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
	// average normals

	GLfloat cubo_vertices[] = {
		// Número 1
		//x		y		z	S		T			NX		NY		NZ
		0.0f,  1.0f,  0.0f,  0.246f,  0.501f,   0.0f,   -1.0f,  -1.0f,   // Vértice 1
		1.0f,  0.0f,  0.0f,  0.489f,  0.75f,   0.0f,   -1.0f,  -1.0f,   // Vértice 2
		0.0f,  0.0f,  1.0f,  0.75f,  0.5f,		0.0f,   -1.0f,  -1.0f,   // Vértice 3

		// Número 3
		//x		y		z	S		T		
		0.0f,  1.0f,  0.0f,	0.49f, 0.755f,		-0.58f,  0.0f, 0.0f,
		1.0f,  0.0f,  0.0f,	0.0f, 0.75f,		-0.58f,  0.0f, 0.0f,
		0.0f,  0.0f, -1.0f,	0.24f, 0.501f,		-0.58f,  0.0f, 0.0f,

		// Número 2
		//x		y		z	 S		T
		0.0f,  1.0f,  0.0f,	 0.489f, 0.254f, 	   1.0f,  -1.0f, 0.0f,
		0.0f,  0.0f, -1.0f,		 0.246f, 0.501f,   1.0f,  -1.0f, 0.0f,
		-1.0f,  0.0f,  0.0f,   0.0f, 0.254f,		1.0f,  -1.0f, 0.0f,

		// Número 4
	   //x		y		z	S		T
		0.0f,  1.0f,  0.0f,	0.246f, 0.501f,	   0.58f,  0.0f,  0.58f,
		-1.0f,  0.0f,  0.0f,0.489f, 0.254f,	   0.58f,  0.0f,  0.58f,
		0.0f,  0.0f,  1.0f,	0.75f, 0.5f,	   0.58f,  0.0f,  0.58f,

		// Número 7
	   //x		y		z	S		T
	   0.0f, -1.0f,  0.0f,	0.75f, 0.5f,		0.0f, -1.0f,  -1.0f,
	   1.0f,  0.0f,  0.0f,	1.0f, 0.75f,		0.0f, -1.0f,  -1.0f,
	   0.0f,  0.0f,  1.0f,	0.489f, 0.75f,		0.0f, -1.0f,  -1.0f,

	   // Número 5
	   //x		y		z	S		T
	   0.0f, -1.0f,  0.0f,	0.498f, 0.75f,			-0.58f, 0.0f, 0.0f,
	   0.0f,  0.0f, -1.0f,	1.0f, 0.75f, 			-0.58f, 0.0f, 0.0f,
	   1.0f,  0.0f,  0.0f,		0.75f, 1.0f,		-0.58f, 0.0f, 0.0f,

	   // Número 6 
	   //x		y		z	S		T
	   0.0f, -1.0f,  0.0f,	0.498f, 0.254f, 	   0.58f, 0.58f, -0.58f, // Pico
	   -1.0f,  0.0f,  0.0f,	1.0f, 0.254f, 			0.58f, 0.58f, -0.58f,
	   0.0f,  0.0f, -1.0f,	0.75f, 0.498f,			0.58f, 0.58f, -0.58f,

	   // Número 8 – inferior izquierda frontal
	   //x		y		z	S		T
	   0.0f, -1.0f,  0.0f,	0.498f, 0.25f,	   0.58f, 0.0f,  0.58f,
	   0.0f,  0.0f,  1.0f,	1.0f, 0.25f, 	   0.58f, 0.0f,  0.58f,
	   -1.0f,  0.0f,  0.0f,	0.75f, 0.0f,	   0.58f, 0.0f,  0.58f,
	};


	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 24);
	meshList.push_back(dado);

}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	CrearDado();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado8.png");
	dadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Vocho = Model();
	Vocho.LoadModel("Models/Vochosincofre.fbx");
	Llanta_V = Model();
	Llanta_V.LoadModel("Models/Llanta.fbx");
	Lampara = Model();
	Lampara.LoadModel("Models/Lampara.fbx");
	Cofre = Model();
	Cofre.LoadModel("Models/Cofre.fbx");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz de cofre
	spotLights[2] = SpotLight(0.25f, 0.88f, 0.82f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-5.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//Luz Faro
	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-5.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[4] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		5.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer arreglo de luces puntuales
	//verde
	pointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
		0.7f, 0.7f,
		-6.0f, 1.0f, 5.0f,
		0.3f, 0.3f, 0.3f);
	pointLightCount++;

	// Luz Lámpara
	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,
		0.1f, 4.0f,
		0.0f, 20.0f, 0.0f,
		1.0f, 0.3f, 0.00009f
	);
	pointLightCount++;

	/*
	//Segundo arreglo  de luces puntuales blanca, verde, azul, roja
	pointLights2[0] = pointLights[3];
	//pointLights2[0].SetPos(glm::vec3(7.0f, 1.0f, 2.0f));
	pointLights2[1] = pointLights[0];
	pointLights2[2] = pointLights[1];
	pointLights2[3] = pointLights[2];
	*/

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);


	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//Función de apagar y prender

		unsigned int lucesPuntuales;

		if (mainWindow.getprendida())
		{
			lucesPuntuales = pointLightCount;
		}
		else
		{
			lucesPuntuales = pointLightCount - 1;
		}

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, lucesPuntuales);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		unsigned int lucesActivas = 0;

		// Faro delantero
		if (mainWindow.getprendidaD())
		{
			spotLights[lucesActivas] = spotLights[3];
			lucesActivas++;
		}

		// Faro trasero
		if (mainWindow.getprendidaT())
		{
			spotLights[lucesActivas] = spotLights[4];
			lucesActivas++;
		}

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, lucesActivas);

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Dado de Opengl
		//Ejercicio 1: Texturizar su cubo con la imagen dado_animales ya optimizada por ustedes
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 2.5f, -3.0f));
		modelaux = model;
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Vocho.RenderModel();

		// Funcionamiento de faro
		glm::vec3 posBasefaro(2.0f, 6.0f, -12.0f);
		glm::vec3 posfaro = posBasefaro + glm::vec3(0.0f + mainWindow.getmuevex(), 0.0f, 0.0);
		spotLights[3].SetPos(posfaro);


		// Funcionamiento de faro trasero
		glm::vec3 posBasefarotrasero(27.0f, 6.0f, -12.0f);
		glm::vec3 posfarotrasero = posBasefarotrasero + glm::vec3(0.0f + mainWindow.getmuevex(), 0.0f, 0.0);
		spotLights[4].SetPos(posfarotrasero);

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -11.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, -0.13f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_V.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(27.0f, 0.2f, -11.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, -0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_V.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, 5.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_V.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(27.0f, 0.2f, 5.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.13f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_V.RenderModel();

		// Articulaciones de cofre
		// Limitar rango
		float anguloCofre1 = glm::clamp(mainWindow.getarticulacion3(), 0.0f, 55.0f);
		float anguloCofre2 = glm::clamp(mainWindow.getarticulacion4(), 0.0f, 55.0f);

		// Articulación abierta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(23.0f, -8.0f, 8.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::rotate(model, glm::radians(-anguloCofre1), glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Articulación cerrada
		model = modelaux2;
		model = glm::translate(model, glm::vec3(23.0f, -8.0f, 8.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::rotate(model, glm::radians(anguloCofre2), glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		// Practica 8
		// Ejercicio 1
		// ==================== Cofre ====================
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-anguloCofre1), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(anguloCofre2), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Cofre.RenderModel();

		glm::vec3 posBaseCofre(20.0f, 0.0f, -6.0f);
		glm::vec3 posCofre = posBaseCofre + glm::vec3(0.0f + mainWindow.getmuevex(), 0.0f + mainWindow.getarticulacion3(), 0.0);
		spotLights[2].SetPos(posCofre);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		// Ejercicio Práctica 8
		// Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 9.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		glm::vec3 posBaseLamp(0.0f, 5.0f, 8.0f);
		glm::vec3 posLamp = posBaseLamp + glm::vec3(0.0, 0.0f, 0.0f);
		pointLights[1];

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
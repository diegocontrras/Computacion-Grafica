/*
PROYECTO FINAL
Limite del Mapa 197 en Z
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

//animacion keyframes

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const string nombreArchivo = "KEYFRAMES_FINAL.txt";
bool archivoExiste = false;

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "practica9full.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movglobo;
float movOffset;
float movOffsetglobo;
float rotllanta;
float rotllantaOffset;
bool avanza;
bool avanzaglobo;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
float globoavance = 0.0f;
float rotacionglobo = 0.0f;
float movGloboX = 0.0f;
float movGloboZ = 0.0f;
bool avanzarX = true;
bool avanzarZ = true;


// Variables para animación del ring
float ringPosY = -20.0f;          // Posición Y inicial
const float ringTargetY = -2.0f;  // Posición Y final
bool animacionRingInicia = false; // Bandera para saber si debe animarse
float ringRotY = 0.0f;            // Ángulo de rotación inicial en Y

// Variables para luces en el ring
bool lucesRingEncendidas = true; // Empiezan encendidas
bool tPressed = false;           // Control para la tecla 'T'

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

// maquina sacudida
bool animacionSacude = false;
float sacudeTime = 0.0f;
float sacudeDuration = 0.8f;
float sacudeAmplitude = 0.75f;
float sacudeFrequency = 85.0f;
float sacudeDamping = 2.0f;


float sacudeOffsetX = 0.0f;
float sacudeRotZDeg = 0.0f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture cartelBaxterTexture;

Skybox skyboxDia;
Skybox skyboxNoche;

Model Kiosko;
Model Fuente;
Model Letrero;
Model Lampara;
Model Ring;
Model Hom;
Model ChichenItza;
Model Arbusto;
Model Pasto;
Model FantasticCar; // keyframes
Model Maquina;
Model Baxter;
Model Cartel;
Model Globo;
Model Arbol;
Model CuerpoH;
Model BrazoDH;
Model BrazoIH;
Model SpiderMan;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat deltaTime2 = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat lastTime2 = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
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

Sphere sp = Sphere(1.0, 20, 20);

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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;


//NEW// Keyframes
float posX4 = -100.0, posY4 = -2.0, posZ4 = -100.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 100 //Número de cuadros máximos
int i_max_steps = 100; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float movAvion_zInc;		//Variable para IncrementoZ
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	//Se agregan nuevas líneas para guardar más variables si es necesario

	// Crea el archivo si no existe
	ofstream archivo(nombreArchivo, ios::app); // modo append

	if (!archivo) {
		cerr << "Error al crear el archivo.\n";
	}
	else {
		archivo << "KeyFrame: [" << FrameIndex << "] Movimiento del Fantasticar en x: " << movAvion_x << "\n";
		archivo << "KeyFrame: [" << FrameIndex << "] Movimiento del Fantasticar en y: " << movAvion_y << "\n";
		archivo << "KeyFrame: [" << FrameIndex << "] Movimiento del Fantasticar en z: " << movAvion_z << "\n";
		archivo << "KeyFrame: [" << FrameIndex << "] Giro del Fantasticar: " << giroAvion << "\n";

		archivo.close();
		cout << "Datos añadidos correctamente.\n";
	}



	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_z = KeyFrame[0].movAvion_z;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////

// --- Actualización por frame ---
void updateSacudida(float deltaTime)
{
	if (!animacionSacude) {
		sacudeOffsetX = 0.0f;
		sacudeRotZDeg = 0.0f;
		return;
	}

	sacudeTime += deltaTime;

	if (sacudeTime <= sacudeDuration) {
		float t = sacudeTime;
		float decay = expf(-sacudeDamping * t);
		float shake = sacudeAmplitude * decay * sinf(sacudeFrequency * t);

		sacudeOffsetX = shake;
		sacudeRotZDeg = shake * 10.0f; // leve inclinación
	}
	else {
		animacionSacude = false;
		sacudeOffsetX = 0.0f;
		sacudeRotZDeg = 0.0f;
	}
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	Camera camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0, 1, 0), -90.0f, 0.0f, 6.0f, 0.1f);
	glm::vec3 avatar = glm::vec3(0.0f, -2.0f, 170.0f);
	camera.setAvatarPointer(&avatar);

	// PUNTOS DE INTERES
	camera.addPointOfInterest(glm::vec3(100.0f, 70.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //Kiosko
	camera.addPointOfInterest(glm::vec3(0.0f, 70.0f, -80.0f), glm::vec3(160.0f, -2.0f, -100.0)); // Fuente
	camera.addPointOfInterest(glm::vec3(-100.0f, 70.0f, 25.0f), glm::vec3(-200.0f, 0.0f, 100.0f)); // Chichen Itza
	camera.addPointOfInterest(glm::vec3(-150.0f, 70.0f, 25.0f), glm::vec3(-200.0f, 0.0f, -100.0f)); // Ring
	camera.addPointOfInterest(glm::vec3(100.0f, 70.0f, 25.0f), glm::vec3(180.0f, 0.0f, -16.0f)); // Máquina
	camera.addPointOfInterest(glm::vec3(-100.0f, 70.0f, 0.0f), glm::vec3(-100.0f, 0.0f, -100.0f)); // Fantasticar
	camera.addPointOfInterest(glm::vec3(100.0f, 70.0f, 25.0f), glm::vec3(0.0f, 0.0f, -80.0f)); // Escultura
	camera.addPointOfInterest(glm::vec3(-100.0f, 100.0f, 0.0f), glm::vec3(370.0f, 400.0f, 0.0f)); // BAXTER


	// TEXTURAS
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/Piso.png");
	pisoTexture.LoadTextureA();
	cartelBaxterTexture = Texture("Textures/CartelBaxter.png");
	cartelBaxterTexture.LoadTextureA();

	// MODELOS
	Kiosko = Model();
	Kiosko.LoadModel("Models/Kiosko.obj");
	Fuente = Model();
	Fuente.LoadModel("Models/Fuente.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/Lampara.obj");
	Ring = Model();
	Ring.LoadModel("Models/Ring.obj");
	Hom = Model();
	Hom.LoadModel("Models/Hom.obj");
	ChichenItza = Model();
	ChichenItza.LoadModel("Models/ChichenItza.obj");
	Arbusto = Model();
	Arbusto.LoadModel("Models/Arbusto.obj");
	Pasto = Model();
	Pasto.LoadModel("Models/Pasto.obj");
	FantasticCar = Model();
	FantasticCar.LoadModel("Models/FANTASTICAR.obj");
	Maquina = Model();
	Maquina.LoadModel("Models/Maquina.obj");
	Baxter = Model();
	Baxter.LoadModel("Models/BAXTER.obj");
	Cartel = Model();
	Cartel.LoadModel("Models/Cartel.obj");
	Globo = Model();
	Globo.LoadModel("Models/Globos.obj");
	Arbol = Model();
	Arbol.LoadModel("Models/Pino.obj");
	CuerpoH = Model();
	CuerpoH.LoadModel("Models/CuerpoH.obj");
	BrazoDH = Model();
	BrazoDH.LoadModel("Models/BrazoDH.obj");
	BrazoIH = Model();
	BrazoIH.LoadModel("Models/BrazoIH.obj");
	SpiderMan = Model();
	SpiderMan.LoadModel("Models/Spider_Man.obj");

	// Cargar Skybox de DÍA
	std::vector<std::string> skyboxFacesDia;
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_left.bmp");
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_right.bmp");
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_bottom.bmp");
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_top.bmp");
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_back.bmp");
	skyboxFacesDia.push_back("Textures/Skybox/Daylight Box_front.bmp");
	skyboxDia = Skybox(skyboxFacesDia);

	// Cargar Skybox de NOCHE
	std::vector<std::string> skyboxFacesNoche;
	skyboxFacesNoche.push_back("Textures/Skybox/corona_lf.png");
	skyboxFacesNoche.push_back("Textures/Skybox/corona_rt.png");
	skyboxFacesNoche.push_back("Textures/Skybox/corona_dn.png");
	skyboxFacesNoche.push_back("Textures/Skybox/corona_up.png");
	skyboxFacesNoche.push_back("Textures/Skybox/corona_bk.png");
	skyboxFacesNoche.push_back("Textures/Skybox/corona_ft.png");
	skyboxNoche = Skybox(skyboxFacesNoche);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	// --- PARÁMETROS DE LUZ DE LAS LÁMPARAS ---
	glm::vec3 luzColor = glm::vec3(1.0f, 1.0f, 0.7f);	// Un blanco cálido
	float lampara_luzIntensidad = 6.0f;							// Intensidad difusa
	float lampara_luzAtenuacionCon = 0.3f;
	float lampara_luzAtenuacionLin = 0.2f;
	float lampara_luzAtenuacionExp = 0.1f;
	float luzAltura = 20.0f;

	// Declaración de las luces puntuales de las lámparas
	// Luz 1 (Pos: 50, 27)
	pointLights[0] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		50.0f, luzAltura, 27.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 2 (Pos: 50, -20)
	pointLights[1] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		50.0f, luzAltura, -20.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 3 (Pos: 270, 27)
	pointLights[2] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		270.0f, luzAltura, 27.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 4 (Pos: 270, -20)
	pointLights[3] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		270.0f, luzAltura, -20.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 5 (Pos: -50, 27)
	pointLights[4] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-50.0f, luzAltura, 27.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 6 (Pos: -50, -13)
	pointLights[5] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-50.0f, luzAltura, -13.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 7 (Pos: -250, 27)
	pointLights[6] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-250.0f, luzAltura, 27.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 8 (Pos: -250, -13)
	pointLights[7] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-250.0f, luzAltura, -13.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 9 (Pos: 25, 180)
	pointLights[8] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		25.0f, luzAltura, 180.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 10 (Pos: -20, 180)
	pointLights[9] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-20.0f, luzAltura, 180.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 11 (Pos: 25, -180)
	pointLights[10] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		25.0f, luzAltura, -180.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	// Luz 12 (Pos: -20, -180)
	pointLights[11] = PointLight(luzColor.x, luzColor.y, luzColor.z, 0.1f, lampara_luzIntensidad,
		-20.0f, luzAltura, -180.0f,
		lampara_luzAtenuacionCon, lampara_luzAtenuacionLin, lampara_luzAtenuacionExp);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Luces del ring
	// Colores
	glm::vec3 colorVerde = glm::vec3(0.0f, 0.8f, 0.2f); // Verde árbol
	glm::vec3 colorMorado = glm::vec3(0.7f, 0.2f, 1.0f); // Morado
	// Posición del Ring
	float ringCenterX = -200.0f;
	float ringCenterZ = -100.0f;
	float ringY = -1.0f; // Altura de las luces
	float ringOffset = 50.0f; // Distancia desde el centro del ring
	// Posiciones de las 4 esquinas
	glm::vec3 posSL1 = glm::vec3(ringCenterX - ringOffset, ringY, ringCenterZ - ringOffset); // Esquina 1
	glm::vec3 posSL2 = glm::vec3(ringCenterX + ringOffset, ringY, ringCenterZ - ringOffset); // Esquina 2
	glm::vec3 posSL3 = glm::vec3(ringCenterX - ringOffset, ringY, ringCenterZ + ringOffset); // Esquina 3
	glm::vec3 posSL4 = glm::vec3(ringCenterX + ringOffset, ringY, ringCenterZ + ringOffset); // Esquina 4
	// Dirección: Apuntando "hacia arriba y hacia el centro"
	// Definimos un punto objetivo alto, en el centro del ring
	glm::vec3 targetPos = glm::vec3(ringCenterX, 15.0f, ringCenterZ);
	glm::vec3 dirSL1 = glm::normalize(targetPos - posSL1);
	glm::vec3 dirSL2 = glm::normalize(targetPos - posSL2);
	glm::vec3 dirSL3 = glm::normalize(targetPos - posSL3);
	glm::vec3 dirSL4 = glm::normalize(targetPos - posSL4);
	float luzIntensidad = 6.0f; // Qué tan brillantes son
	float luzAngulo = 20.0f;     // Qué tan abierto es el cono de luz

	// Luz 1 (Verde) - Esquina 1
	spotLights[1] = SpotLight(colorVerde.x, colorVerde.y, colorVerde.z,
		0.0f, luzIntensidad,           // aIntensity, dIntensity
		posSL1.x, posSL1.y, posSL1.z, // pos
		dirSL1.x, dirSL1.y, dirSL1.z, // dir
		1.0f, 0.0f, 0.001f,           // con, lin, exp 
		luzAngulo);                   // ángulo
	spotLightCount++;

	// Luz 2 (Morado) - Esquina 2
	spotLights[2] = SpotLight(colorMorado.x, colorMorado.y, colorMorado.z,
		0.0f, luzIntensidad,
		posSL2.x, posSL2.y, posSL2.z,
		dirSL2.x, dirSL2.y, dirSL2.z,
		1.0f, 0.0f, 0.001f,
		luzAngulo);
	spotLightCount++;

	// Luz 3 (Verde) - Esquina 3
	spotLights[3] = SpotLight(colorVerde.x, colorVerde.y, colorVerde.z,
		0.0f, luzIntensidad,
		posSL3.x, posSL3.y, posSL3.z,
		dirSL3.x, dirSL3.y, dirSL3.z,
		1.0f, 0.0f, 0.001f,
		luzAngulo);
	spotLightCount++;

	// Luz 4 (Morado) - Esquina 4
	spotLights[4] = SpotLight(colorMorado.x, colorMorado.y, colorMorado.z,
		0.0f, luzIntensidad,
		posSL4.x, posSL4.y, posSL4.z,
		dirSL4.x, dirSL4.y, dirSL4.z,
		1.0f, 0.0f, 0.001f,
		luzAngulo);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// Variables para el ciclo del sol
	float anguloSol = 0.0f;          // El ángulo actual del sol
	float velocidadSol = 0.1f;

	float walkTime = 0.0f;

	glm::vec3 dir = camera.getLastMoveDir();
	if (glm::length(dir) > 0.0f)
		walkTime += deltaTime * 6.0f;

	float brazoSwing = sin(walkTime) * glm::radians(25.0f);
	movCoche = 0.0f;
	movOffset = 0.0f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 pos4 = glm::vec3(0.0f, 10.0f, 0.0f);

	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQUÍ

	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].movAvion_z = 0.0f;
	KeyFrame[0].giroAvion = 90.0;

	ifstream verificador(nombreArchivo);
	archivoExiste = verificador.good();
	verificador.close();

	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar moverse");
	printf("\n7.-Presiona 3 para mover en -X\n8.-Presiona 4 para mover en Y\n9.-Presiona 5 para habilitar moverse en Y\n10.-Presiona 6 para mover en -Y\n11.-Presiona 7 para mover Z");
	printf("\n12.-Presiona 8 para habilitar moverse en Z y girar\n13.-Presiona 9 para mover -Z\n14.-Presiona 0 para girar");

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);
	bool cPressed = false, qPressed = false, ePressed = false;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		updateSacudida(deltaTime);

		// Lógica para el movimiento del sol
		anguloSol += velocidadSol * deltaTime;
		if (anguloSol > 360.0f)
		{
			anguloSol -= 360.0f;
		}
		float solDirY = -cos(glm::radians(anguloSol));
		float solDirZ = -sin(glm::radians(anguloSol));
		mainLight.SetDirection(0.0f, solDirY, solDirZ);

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);

		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (mainWindow.getsKeys()[GLFW_KEY_C]) {
			if (!cPressed) { camera.cycleMode(); cPressed = true; std::cout << "Mode: " << (int)camera.getMode() << std::endl; }
		}
		else cPressed = false;

		if (mainWindow.getsKeys()[GLFW_KEY_Q]) {
			if (!qPressed) { camera.previousPOI(); qPressed = true; std::cout << "PuntoInteres: " << camera.getCurrentPOIIndex() << std::endl; }
		}
		else qPressed = false;

		if (mainWindow.getsKeys()[GLFW_KEY_E]) {
			if (!ePressed) { camera.nextPOI(); ePressed = true; std::cout << "PuntoInteres: " << camera.getCurrentPOIIndex() << std::endl; }
		}
		else ePressed = false;

		camera.Update(deltaTime);

		// calcular view y render:
		glm::mat4 view = camera.calculateViewMatrix();

		// Bandera para animación del ring
		if (mainWindow.getsKeys()[GLFW_KEY_R])
		{
			animacionRingInicia = true;
			ringPosY = -20.0f; // Reinicia la posición
			ringRotY = 0.0f;   // Reinicia la rotación
		}

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// *** LÓGICA DE ANIMACIÓN DEL RING ***
		if (animacionRingInicia)
		{
			// Si el ring todavía no llega a su destino
			if (ringPosY < ringTargetY)
			{
				// Moverlo hacia arriba, basado en la velocidad y el deltaTime
				ringPosY += 0.05f * deltaTime;
				// Hacer que gire mientras sube
				ringRotY += 0.05f * deltaTime;
			}
			else
			{
				// Si ya llegó (o se pasó), clavarlo en la posición final
				ringPosY = ringTargetY;
				ringRotY = 0.0f; // Detener la rotación
			}
		}


		static bool mPressed = false;
		bool keyM = mainWindow.getsKeys()[GLFW_KEY_M];

		if (keyM && !mPressed) {
			animacionSacude = true;
			sacudeTime = 0.0f;
		}

		mPressed = keyM;


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (solDirY > 0.0f) // El sol está sobre el horizonte
		{
			// ES DE DÍA
			skyboxDia.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else
		{
			// ES DE NOCHE
			skyboxNoche.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// Calcula la intensidad del sol
		float factorSol = max(0.0f, solDirY); // 0.0 de noche, 1.0 en el punto más alto del día

		// Va cambiando la intensidad del sol conforme avanza y se hace de día y de noche
		float intAmb = 0.05f + (factorSol * (0.3f - 0.05f));
		float intDiff = 0.08f + (factorSol * (0.3f - 0.08f));
		mainLight.SetIntensity(intAmb, intDiff);

		// La luz del sol
		shaderList[0].SetDirectionalLight(&mainLight);

		// Indica cuando se prenden/apagan las luces y cambia el skybox
		if (solDirY > 0.0f) // El sol está sobre el horizonte
		{
			// ES DE DÍA
			shaderList[0].SetPointLights(pointLights, 0); // Lámparas apagadas
		}
		else
		{
			// ES DE NOCHE
			shaderList[0].SetPointLights(pointLights, pointLightCount); // Lámparas encendidas
		}
		// Luces del ring
		if (lucesRingEncendidas)
		{
			// Envía todas las luces (linterna + 4 del ring)
			shaderList[0].SetSpotLights(spotLights, spotLightCount); // spotLightCount es 5
		}
		else
		{
			// Envía solo la primera luz (la linterna en el índice 0)
			shaderList[0].SetSpotLights(spotLights, 1);
		}

		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		modelaux2 = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		// Kiosko

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -2.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kiosko.RenderModel();

		// Fuente

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(160.0f, -2.0f, -100.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Fuente.RenderModel();

		// Ring

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, ringPosY, -100.0));
		model = glm::rotate(model, ringRotY, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ring.RenderModel();

		// Escultura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -80.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hom.RenderModel();

		// Chichen Itza

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, -2.0f, 100.0));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ChichenItza.RenderModel();

		// Pasto

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pasto.RenderModel();

		// ANIMACIÓN POR KEYFRAMES

		model = glm::mat4(1.0);
		pos4 = glm::vec3(posX4 + movAvion_x, posY4 + movAvion_y, posZ4 + movAvion_z);
		model = glm::translate(model, pos4);
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FantasticCar.RenderModel();

		// Animacion basica

		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 basePos = glm::vec3(180.0f, -2.0f, -16.0f);
		model = glm::translate(model, basePos + glm::vec3(sacudeOffsetX, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(sacudeRotZDeg), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Maquina.RenderModel();

		// Edificio Baxter

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(370.0f, -3.0f, 0.0f));
		model = glm::rotate(model, glm::radians(360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Baxter.RenderModel();

		GLfloat now2 = glfwGetTime();
		deltaTime2 = now2 - lastTime2;
		deltaTime2 += (now2 - lastTime2) / limitFPS;
		lastTime2 = now2;

		movOffsetglobo = 0.5f;

		if (avanzarX)
		{
			if (movGloboX < 250.0f)
				movGloboX += movOffsetglobo * deltaTime2;
			else
			{
				avanzarX = false;
				rotacionglobo += 180.0f;
			}
		}
		else
		{
			if (movGloboX > -250.0f)
				movGloboX -= movOffsetglobo * deltaTime2;
			else
			{
				avanzarX = true;
				rotacionglobo += 180.0f;
			}
		}

		if (avanzarZ)
		{
			if (movGloboZ < 150.0f)
				movGloboZ += (movOffsetglobo * 0.6f) * deltaTime2;
			else
				avanzarZ = false;
		}
		else
		{
			if (movGloboZ > -150.0f)
				movGloboZ -= (movOffsetglobo * 0.6f) * deltaTime2;
			else
				avanzarZ = true;
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(movGloboX, 5.0f + sin(glm::radians(angulovaria)), 6.0f + movGloboZ));
		model = glm::rotate(model, glm::radians(rotacionglobo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.3f, 3.0f, 3.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Globo.RenderModel();

		// avatar

		if (glm::length(camera.lastMoveDir) > 0.0f)
			walkTime += deltaTime * 6.0f;
		else
			walkTime = 0.0f;

		float armSwing = sin(walkTime) * 25.0f;

		glm::mat4 modelAvatarBase = glm::mat4(1.0f);
		modelAvatarBase = glm::translate(modelAvatarBase, avatar);
		modelAvatarBase = glm::scale(modelAvatarBase, glm::vec3(3.0f));

		// cuerpo
		glm::mat4 modelCuerpo = modelAvatarBase;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		CuerpoH.RenderModel();

		float brazoSwing = sin(walkTime) * glm::radians(25.0f);

		// Brazo derecho
		model = modelaux2;
		model = glm::translate(model, avatar); // posición del avatar
		model = glm::rotate(model, glm::radians(armSwing), glm::vec3(1, 0, 0)); // rotación
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoDH.RenderModel();

		// brazo izquierdo
		model = modelaux2;
		model = glm::translate(model, avatar);
		model = glm::rotate(model, glm::radians(-armSwing), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoIH.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -2.0f, 25.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SpiderMan.RenderModel();

		// -- COMPILADO DE OBJETOS SECUNDARIOS DEL PARQUE --

		// Lamparas

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -2.0f, 27.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -2.0f, -20.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(270.0f, -2.0f, 27.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(270.0f, -2.0f, -20.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		// MAPA IZQUIERDA

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -2.0f, 27.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, -2.0f, -13.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, -2.0f, 27.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, -2.0f, -13.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -2.0f, 180.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, -2.0f, 180.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -2.0f, -180.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, -2.0f, -180.0));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		// Pinos

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, -2.0f, -50.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, -2.0f, -150.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(210.0f, -2.0f, -100.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, -2.0f, -125.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, -2.0f, 50.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, -2.0f, 150.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(210.0f, -2.0f, 100.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, -2.0f, 125.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, -2.0f, -50.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-70.0f, -2.0f, -150.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-250.0f, -2.0f, -125.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Arbol.RenderModel();

		// Letrero

		toffsetnumerocambiau += 0.0005f;
		if (toffsetnumerocambiau > 1.0f)
			toffsetnumerocambiau = 0.0f;
		glm::vec2 toffset = glm::vec2(0.0f, toffsetnumerocambiau);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(250.0f, -3.0f, 100.0));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cartelBaxterTexture.UseTexture();
		Cartel.RenderModel();
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;

		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("movAvion_z es: %f\n", movAvion_z);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
			reinicioFrame++;
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 50.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}
	if (keys[GLFW_KEY_3])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x -= 50.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_4])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y += 50.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 5 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_5])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 4\n");
		}
	}

	if (keys[GLFW_KEY_6])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y -= 50.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 5 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_7])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_z += 50.0f;
			printf("\n movAvion_z es: %f\n", movAvion_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_8])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 7, 9 o 0\n");
		}
	}

	if (keys[GLFW_KEY_9])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_z -= 50.0f;
			printf("\n movAvion_z es: %f\n", movAvion_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
		}

	}


	if (keys[GLFW_KEY_0])
	{
		if (ciclo < 1)
		{
			if (giroAvion >= 360) {
				giroAvion -= 90.0f;
			}
			else {
				giroAvion += 90.0f;
			}
			printf("\n El ángulo del helicoptero es: %f\n", giroAvion);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
		}

	}

	camera.keyControl(keys, deltaTime);

	if (keys[GLFW_KEY_C])
	{
		camera.cycleMode();
	}
	// cambiar punto de vista con Q/E
	if (keys[GLFW_KEY_Q]) camera.previousPOI();
	if (keys[GLFW_KEY_E]) camera.nextPOI();

	if (mainWindow.getsKeys()[GLFW_KEY_T])
	{
		if (!tPressed)
		{
			lucesRingEncendidas = !lucesRingEncendidas; // Invierte el estado
			tPressed = true;
		}
	}
	else
	{
		tPressed = false;
	}
}
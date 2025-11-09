#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetIntensity(GLfloat aIntensity, GLfloat dIntensity);

	void SetDirection(GLfloat x, GLfloat y, GLfloat z);

	~DirectionalLight();

private:
	glm::vec3 direction;
};

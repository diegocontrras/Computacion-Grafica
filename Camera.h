#pragma once

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>

#include <vector>

enum class CameraMode {
    tercera_persona,
    aerea,
    interes
};

class Camera
{
public:
    Camera();
    Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch,
        GLfloat startMoveSpeed, GLfloat startTurnSpeed);

    void keyControl(bool* keys, GLfloat deltaTime);
    void mouseControl(GLfloat xChange, GLfloat yChange);

    void Update(GLfloat deltaTime);

    glm::mat4 calculateViewMatrix();
    glm::vec3 getCameraPosition() const { return position; }
    glm::vec3 getCameraDirection() const { return glm::normalize(front); }

    // modo control
    void cycleMode(); // ciclar entre modos
    void setMode(CameraMode m) { modeActual = m; }
    CameraMode getMode() const { return modeActual; }

    // para la camara de puntos de inter�s 
    void addPointOfInterest(const glm::vec3& poiPosition, const glm::vec3& poiTarget);
    void nextPOI();
    void previousPOI();
    int getPOICount() const { return (int)pointsOfInterest.size(); }
    int getCurrentPOIIndex() const { return currentPOIIndex; }

    // camara a avatar
    void setAvatarPointer(glm::vec3* avatarPtr) { avatarPositionPtr = avatarPtr; }

    ~Camera();

    GLfloat tpDistance = 60.0f; 
    GLfloat tpHeight = 15.0f;     
    GLfloat tpMoveSpeed = 3.0f; 

    GLfloat aerialHeight = 15.0f;   
    GLfloat aerialSpeed = 10.0f;     
    glm::vec3 getLastMoveDir() const { return lastMoveDir; }
    glm::vec3 lastMoveDir = glm::vec3(0.0f);


private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat pitch;

    GLfloat moveSpeed;
    GLfloat turnSpeed;
    

    // estado del modo
    CameraMode modeActual = CameraMode::tercera_persona;

    glm::vec3* avatarPositionPtr = nullptr;

    // Points of Interest: cada entrada es (cameraPosition, lookAtTarget)
    struct POI { glm::vec3 camPos; glm::vec3 lookTarget; };
    std::vector<POI> pointsOfInterest;
    int currentPOIIndex = -1;

    void updateVectors(); // actualizar front/right/up desde yaw/pitch

    // helpers internos
    void applyThirdPersonImmediate(); // tercera persona
    void applyAerial();               // modo aerea
    void applyPOI();                  // interes
};
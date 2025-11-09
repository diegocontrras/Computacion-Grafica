#include "Camera.h"

#include <cmath>

Camera::Camera()
{
    position = glm::vec3(0.0f, 5.0f, 10.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    moveSpeed = 3.0f;
    turnSpeed = 0.1f;

    updateVectors();
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch,
    GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
    position = startPosition;
    worldUp = startUp;
    yaw = startYaw;
    pitch = startPitch;
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    moveSpeed = startMoveSpeed;
    turnSpeed = startTurnSpeed;

    updateVectors();
}

Camera::~Camera()
{
}

void Camera::updateVectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

// Input: teclas
void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
    GLfloat velocityPlayer = tpMoveSpeed * deltaTime; // velocidad del avatar
    GLfloat velocityAerial = aerialSpeed * deltaTime; // velocidad de la camara aérea
    // Comportamiento según modo
    if (modeActual == CameraMode::tercera_persona)
    {
        // Movimiento del avatar
        if (avatarPositionPtr != nullptr)
        {
            glm::vec3 camForward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            glm::vec3 camRight = glm::normalize(glm::cross(camForward, worldUp));

            glm::vec3 moveDir(0.0f);
            if (keys[GLFW_KEY_W]) moveDir += camForward;
            if (keys[GLFW_KEY_S]) moveDir -= camForward;
            if (keys[GLFW_KEY_A]) moveDir -= camRight;
            if (keys[GLFW_KEY_D]) moveDir += camRight;

            if (glm::length(moveDir) > 0.0f)
            {
                moveDir = glm::normalize(moveDir);
                lastMoveDir = moveDir;                   // <-- guardar SOLO cuando hay movimiento
                *avatarPositionPtr += moveDir * velocityPlayer;
            }
            else
            {
                lastMoveDir = glm::vec3(0.0f);           // <-- sin movimiento
            }

        }
    }
    else if (modeActual == CameraMode::aerea)
    {
        glm::vec3 move(0.0f);
        glm::vec3 camFwdXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 camRightXZ = glm::normalize(glm::cross(camFwdXZ, worldUp));

        if (keys[GLFW_KEY_W]) move += camFwdXZ;
        if (keys[GLFW_KEY_S]) move -= camFwdXZ;
        if (keys[GLFW_KEY_A]) move -= camRightXZ;
        if (keys[GLFW_KEY_D]) move += camRightXZ;

        if (glm::length(move) > 0.0f)
        {
            move = glm::normalize(move);
            position += move * velocityAerial;
        }
    }
    else if (modeActual == CameraMode::interes)
    {

    }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

// Update posición/front según modo y avatar pointer
void Camera::Update(GLfloat deltaTime)
{
    switch (modeActual)
    {
    case CameraMode::tercera_persona:
        applyThirdPersonImmediate();
        break;

    case CameraMode::aerea:
        applyAerial();
        break;

    case CameraMode::interes:
        applyPOI();
        break;
    }
}


glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::cycleMode()
{
    // Orden: tercera_persona -> aerea -> interes
    if (modeActual == CameraMode::tercera_persona) modeActual = CameraMode::aerea;
    else if (modeActual == CameraMode::aerea) modeActual = CameraMode::interes;
    else modeActual = CameraMode::tercera_persona;

    if (modeActual == CameraMode::interes && !pointsOfInterest.empty())
    {
        if (currentPOIIndex < 0) currentPOIIndex = 0;
    }
}

void Camera::addPointOfInterest(const glm::vec3& poiPosition, const glm::vec3& poiTarget)
{
    POI p; p.camPos = poiPosition; p.lookTarget = poiTarget;
    pointsOfInterest.push_back(p);
    if (currentPOIIndex < 0) currentPOIIndex = 0;
}

void Camera::nextPOI()
{
    if (pointsOfInterest.empty()) return;
    currentPOIIndex = (currentPOIIndex + 1) % (int)pointsOfInterest.size();
    applyPOI();
}

void Camera::previousPOI()
{
    if (pointsOfInterest.empty()) return;
    currentPOIIndex = (currentPOIIndex - 1 + (int)pointsOfInterest.size()) % (int)pointsOfInterest.size();
    applyPOI();
}


// Cámara aérea
void Camera::applyAerial()
{
    position.y = aerialHeight;

    glm::vec3 center;
    if (avatarPositionPtr != nullptr) center = glm::vec3(avatarPositionPtr->x, 0.0f, avatarPositionPtr->z);
    else center = glm::vec3(position.x, 0.0f, position.z - 1.0f); // fallback

    front = glm::normalize(center - position);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}


// Cámara puntos de interés
void Camera::applyPOI()
{
    if (pointsOfInterest.empty() || currentPOIIndex < 0) return;

    POI& p = pointsOfInterest[currentPOIIndex];
    position = p.camPos;
    front = glm::normalize(p.lookTarget - position);

    // Recalcular yaw y pitch desde front, si tu cámara los usa
    yaw = glm::degrees(atan2(front.z, front.x)) - 90.0f;
    pitch = glm::degrees(asin(front.y));

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::applyThirdPersonImmediate()
{
    if (!avatarPositionPtr) return;

    // Dirección hacia donde se mueve el avatar
    // Suponemos que "front" del avatar es +Z o lo que tú definas
    glm::vec3 avatarPos = *avatarPositionPtr;

    // La cámara estará detrás del avatar alineada con su dirección (front)
    glm::vec3 target = avatarPos;

    // La cámara se coloca atrás del avatar usando la dirección "front"
    glm::vec3 desiredCamPos = avatarPos - front * tpDistance + glm::vec3(0.0f, tpHeight, 0.0f);

    // Interpolación suave
    float smooth = 0.1f; // 0.05 muy suave, 0.2 más rápido
    position = glm::mix(position, desiredCamPos, smooth);

    // Cámara mira al avatar siempre
    front = glm::normalize(target - position);

    // Recalcula right y up
    updateVectors();
}

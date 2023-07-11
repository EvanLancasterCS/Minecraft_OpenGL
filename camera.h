#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LEFTCLICK,
    RIGHTCLICK
};

// Default camera values
const float YAW = -85.0f;
const float PITCH = 0.0f;
const float SPEED = 3.5f;
const float SENSITIVITY = 0.1f;
const float MOVEMENT_SPEED = 0.3f;

class Camera
{
  public:
    Camera();
    ~Camera();
    void Update(float dt);
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessScrollMovement(float yoffset);

    // euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;

    // camera options
    float aspect = 0;
    float fov = 40.f;

    glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 10.0f);

    glm::vec3 GetCameraFront() { return cameraFront; }
    glm::vec3 GetRealCameraPos() { return glm::vec3((cameraPos.x / 2.f), (cameraPos.y / 2.f), (cameraPos.z / 2.f)); }

  private:

    double x = 0.0;
    double y = 10.0;
    double z = -16.0;

    glm::mat4 projection;
    glm::mat4 view;

    const float cameraSpeed = 3.5f;

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -12.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
};

#endif /* CAMERA_H */

#include "camera.h"
#include "chunkHandler.h"
#include "ray.h"

Camera::Camera()
{
    
}

Camera::~Camera()
{
    
}

void Camera::Update(float dt)
{
    glm::vec3 pos = glm::vec3(glm::floor(cameraPos.x / 2), glm::floor(cameraPos.y / 2), glm::floor(cameraPos.z / 2));
    cameraFront = glm::normalize(cameraFront);
    //std::cout << cameraPos.x/2 << ", " << cameraPos.y/2 << ", " << cameraPos.z/2 << "\n";
    //ChunkHandler::instance->setBlock(pos.x, pos.y, pos.z, 0);
    /*glm::vec2 camFrontVec2 = glm::normalize(glm::vec2(cameraFront.x, cameraFront.z));
    glm::vec2 camRightVec2 = glm::vec2(cameraRight.x, cameraRight.z);
    glm::vec2 zFar = camFrontVec2 * 300.f;
    float halfVSide = 300.f * tanf(fov * .5f);
    float halfHSide = halfVSide * aspect;

    //glm::vec2 halfDiff = zFar * tanf(glm::radians(fov) / 2.f) * aspect * 3.f;
    glm::vec2 p1 = glm::vec2(pos.x, pos.z) - (camFrontVec2 * 32.f);
    glm::vec2 p2 = p1 + zFar + (halfHSide * camRightVec2);
    glm::vec2 p3 = p1 + zFar - (halfHSide * camRightVec2);
    ChunkHandler::instance->updateCameraFrustrum(p1, p2, p3);*/
    
    
    float radius = 5;
    
    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            for (int z = -radius; z <= radius; z++)
            {
                if (x * x + y * y + z * z <= radius * radius && pos.y + y > 0)
                {
                    //ChunkHandler::instance->setBlock(pos.x + x, pos.y + y + 1, pos.z + z, 0);
                }
            }
        }
    }

    int chunkGen = 8;
    for (int x = -chunkGen; x <= chunkGen; x++)
    {
        for (int z = -chunkGen; z <= chunkGen; z++)
        {
            int chunkX = pos.x / 16;
            int chunkZ = pos.z / 16;
            chunkX += x;
            chunkZ += z;
            ChunkHandler::instance->camera_GenerateChunk(chunkX, chunkZ);
        }
    }

    ChunkHandler::instance->sortRebuildsByDistance(pos.x, pos.y, pos.z);
    ChunkHandler::instance->unloadChunksByDistance(pos.x, pos.y, pos.z);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
    projection = glm::perspective(glm::radians(fov), aspect, 0.01f, 500.0f);
}

bool Camera::Initialize(int w, int h)
{
 
  aspect = float(w) / float(h);
  view = glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

  projection = glm::perspective( glm::radians(fov), //the FoV typically 90 degrees is good which is what this is set to
                                 aspect, //Aspect Ratio, so Circles stay Circular
                                 0.01f, //Distance to the near plane, normally a small value like this
                                 100.0f); //Distance to the far plane, 
  return true;
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    //float velocity = MovementSpeed * deltaTime;
    switch (direction)
    {
    case FORWARD:
        cameraPos += cameraFront * MOVEMENT_SPEED;
        break;
    case BACKWARD:
        cameraPos -= cameraFront * MOVEMENT_SPEED;
        break;
    case RIGHT:
        cameraPos += cameraRight * MOVEMENT_SPEED;
        break;
    case LEFT:
        cameraPos -= cameraRight * MOVEMENT_SPEED;
        break;
    case UP:
        cameraPos += glm::vec3(0, 1, 0) * MOVEMENT_SPEED;
        break;
    case DOWN:
        cameraPos -= glm::vec3(0, 1, 0) * MOVEMENT_SPEED;
        break;
    case LEFTCLICK:
    {
        glm::vec3 camPos = GetRealCameraPos();
        Ray testRay = Ray(camPos.x, camPos.y, camPos.z, cameraFront.x, cameraFront.y, cameraFront.z, 10);
        RayHitInfo hit = testRay.Raycast();
        if (hit.hit)
            ChunkHandler::instance->setBlock(hit.blockX, hit.blockY, hit.blockZ, 0);

        break;
    }
    case RIGHTCLICK:
    {
        glm::vec3 camPos = GetRealCameraPos();
        Ray testRay = Ray(camPos.x, camPos.y, camPos.z, cameraFront.x, cameraFront.y, cameraFront.z, 10);
        RayHitInfo hit = testRay.Raycast();
        if (hit.hit)
            ChunkHandler::instance->setBlock(hit.blockX + hit.hitNormalX, hit.blockY + hit.hitNormalY, hit.blockZ + hit.hitNormalZ, 1);

        break;
    }
    default:
        break;
    }
    //UpdateCameraVectors();
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    Yaw += xoffset * SENSITIVITY;
    Pitch += yoffset * SENSITIVITY;

    // Bounds checking
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);

    cameraRight = glm::normalize(glm::cross(cameraFront, up));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::ProcessScrollMovement(float yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.f)
        fov = 45.0f;

   //UpdateCameraVectors();
}
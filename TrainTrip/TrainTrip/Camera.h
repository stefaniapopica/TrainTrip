
#ifndef CAMERA_H
#define CAMERA_H


#define GLM_ENABLE_EXPERIMENTAL

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

#include <iostream>
#include <fstream>
#include <sstream>
#include <GLM.hpp>
#include <vector>


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 200.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM  = 60.0f;


class Camera
{

public:
   
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    
    float Yaw;
    float Pitch;

    
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;



    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
        Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
private:

    void updateCameraVectors();
    glm::vec3 prevPos;








};


#endif

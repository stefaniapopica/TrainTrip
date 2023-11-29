
#ifndef CAMERA_H
#define CAMERA_H


#define GLM_ENABLE_EXPERIMENTAL

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

#include <iostream>
#include <fstream>
#include <sstream>
//#include <GLM.hpp>
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
};


#endif

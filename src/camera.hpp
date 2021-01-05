#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
	UPWARD,
	DOWNWARD,
	ROLLLT,
	ROLLRT,
};

// Default camera values
const float ROLL 		=  0.0f;
const float YAW         =  0.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


class Camera{
	
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
	float Roll;
	float Yaw;
	float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
		Roll = roll;
		updateCameraVectors();
	}
   
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

	glm::mat4 GetCameraMatrix(){
		// Camera Coord Euler Convention: Tait-Bryan YX
		// -> extrinsic: XY (Y*X in premult-matrix)
		// -> inverse: YX (X*Y in premult-matrix)
		glm::mat4 position = glm::translate(glm::mat4(1.0), -Position);
		glm::mat4 camera = glm::transpose(glm::eulerAngleYXZ(
			glm::radians(Yaw), glm::radians(Pitch), glm::radians(Roll)));
		return camera * position;
	}


    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
		if (direction == UPWARD)
			Position += Up * velocity;
		if (direction == DOWNWARD)
			Position -= Up * velocity;
		if (direction == ROLLLT)
			Roll += 10.0 * velocity;
		if (direction == ROLLRT)
			Roll -= 10.0 * velocity;
		// if (Roll > -5.0 && Roll < 5.0){
		// 	if((Roll > 0 && direction == ROLLRT) || (Roll < 0 && direction == ROLLLT))
		// 		Roll = 0.0;
		// }
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
		/**
		 * 	  LT <-- X --> RT
		 * 	  UP <-- Y --> DN
		 * 	Euler Convention: Tait-Bryan YX
		 */ 
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   -= xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }


    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 60.0f)
            Zoom = 60.0f; 
    }

private:
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front(0.0, 0.0, -1.0);
		front = glm::eulerAngleYXZ(glm::radians(Yaw), glm::radians(Pitch), glm::radians(Roll)) * glm::vec4(front, 0.0);
        // front.x = -sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        // front.y = sin(glm::radians(Pitch));
        // front.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp)); 
        Up    = glm::normalize(glm::cross(Right, Front));
    
	}
};

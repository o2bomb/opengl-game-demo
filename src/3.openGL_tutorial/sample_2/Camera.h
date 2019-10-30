/*
    SOURCE(s):
		https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
*/
// CREATES A CAMERA OBJECT THAT DETERMINES THE PLAYERS WORLD COORDINATES AND VIEW DIRECTION
// AND HANDLES CAMERA MOVEMENT BASED ON KEYBOARD INPUT

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <math.h>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	JUMP,
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

bool isJumping = true;

class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.01f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH) : 
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED), 
		MouseSensitivity(SENSITIVITY)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime, bool isJump)
	{
		float velocity = MovementSpeed * deltaTime;
		// the player moves forwards
		if (direction == FORWARD)
		{
			if (isJump)
			{
				Position += Front * velocity;
			}
			else
			{
				// if the player is on the ground
				// then do not let the player clip through the ground
				// and do not let the player fly off of the ground
				Position += Front * velocity;
				Position.y = 0.6;
			}
			
		}
		// the player moves backwards
		if (direction == BACKWARD)
		{
			if (isJump)
			{
				Position -= Front * velocity;
			}
			else
			{
				// if the player is on the ground
				// then do not let the player clip through the ground
				// and do not let the player fly off of the ground
				Position -= Front * velocity;
				Position.y = 0.6;
			}
		}
		// the player moves left
		if (direction == LEFT)
			Position -= Right * velocity;
		// the player moves right
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// PROCESS MOUSE INPUT
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// do not let the player break his neck
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}

private:
	// UPDATE CAMERA Front VECTOR
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif

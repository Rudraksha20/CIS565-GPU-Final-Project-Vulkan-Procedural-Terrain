#include <iostream>

#define GLM_FORCE_RADIANS
// Use Vulkan depth range of 0.0 to 1.0 instead of OpenGL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#define TEST_CAMERA 0
#define WIND_GIF_CAMERA 0

#include "Camera.h"
#include "BufferUtils.h"

#if TEST_CAMERA
Camera::Camera(Device* device, float aspectRatio) : device(device) {
    r = 0.5f;
    theta = 0.0f;
    phi = 0.0f;
    cameraRefPos = glm::vec3(0.0f);
    cameraBufferObject.viewMatrix = glm::lookAt(glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    cameraBufferObject.projectionMatrix[1][1] *= -1; // y-coordinate is flipped
    cameraBufferObject.cameraPos = cameraRefPos;

    BufferUtils::CreateBuffer(device, sizeof(CameraBufferObject), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
    vkMapMemory(device->GetVkDevice(), bufferMemory, 0, sizeof(CameraBufferObject), 0, &mappedData);
    memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}
#else
Camera::Camera(Device* device, float aspectRatio) : device(device) {
    r = 0.5f;
    theta = 0.0f;
    phi = 0.0f;
	cameraRefPos = glm::vec3(0.0f);
    cameraBufferObject.viewMatrix = glm::lookAt(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    cameraBufferObject.projectionMatrix[1][1] *= -1; // y-coordinate is flipped
	cameraBufferObject.cameraPos = cameraRefPos;

    BufferUtils::CreateBuffer(device, sizeof(CameraBufferObject), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
    vkMapMemory(device->GetVkDevice(), bufferMemory, 0, sizeof(CameraBufferObject), 0, &mappedData);
    memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}
#endif // FRUSTUM_CULL_TEST


VkBuffer Camera::GetBuffer() const {
    return buffer;
}

void Camera::UpdateOrbit(float deltaX, float deltaY, float deltaZ) {
    theta += deltaX;
    phi += deltaY;
	r = r - deltaZ; // glm::clamp(r - deltaZ, 1.0f, 50.0f); // Change this to make the camera go furthure 

    float radTheta = glm::radians(theta);
    float radPhi = glm::radians(phi);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), radTheta, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), radPhi, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 finalTransform = glm::translate(glm::mat4(1.0f), cameraRefPos) * rotation * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, r));// glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)) * rotation * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, r));

    cameraBufferObject.viewMatrix = glm::inverse(finalTransform);

    //memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}

void Camera::PanCamera(float deltaX, float deltaY, float deltaZ) {
	cameraRefPos.x += deltaX;
	cameraRefPos.y += deltaY;
	cameraRefPos.z += deltaZ; 

	cameraBufferObject.cameraPos = cameraRefPos;

	//memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}

void Camera::ResetCamera() {
	r = 10.0f;
	theta = 0.0f;
	phi = 0.0f;
	cameraRefPos = glm::vec3(0.0f);
}

const CameraBufferObject& Camera::GetCBO() {
    return cameraBufferObject;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
    cameraBufferObject.projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    cameraBufferObject.projectionMatrix[1][1] *= -1; // y-coordinate is flipped

    //memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}

// Updates camera buffer object
void Camera::UpdateBuffer() {
    memcpy(mappedData, &cameraBufferObject, sizeof(CameraBufferObject));
}

Camera::~Camera() {
  vkUnmapMemory(device->GetVkDevice(), bufferMemory);
  vkDestroyBuffer(device->GetVkDevice(), buffer, nullptr);
  vkFreeMemory(device->GetVkDevice(), bufferMemory, nullptr);
}

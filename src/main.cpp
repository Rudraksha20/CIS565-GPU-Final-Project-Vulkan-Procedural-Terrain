#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Instance.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"
#include "Image.h"
#include <iostream>

Device* device;
SwapChain* swapChain;
Renderer* renderer;
Camera* camera;

namespace {
    void resizeCallback(GLFWwindow* window, int width, int height) {
        if (width == 0 || height == 0) return;

        vkDeviceWaitIdle(device->GetVkDevice());
        swapChain->Recreate();
        renderer->RecreateFrameResources();
    }

    bool leftMouseDown = false;
    bool rightMouseDown = false;
	bool middleMouseDown = false;
    double previousX = 0.0;
    double previousY = 0.0;
	double previousPosX = 0.0;
	double previousPosY = 0.0;
	double previousPosZ = 0.0;
	float stepSize = 1.0;
	bool keyPressedA = false;
	bool keyPressedS = false;
	bool keyPressedD = false;
	bool keyPressedW = false;
	bool keyPressedQ = false;
	bool keyPressedE = false;

	void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_A) {
			if (action == GLFW_PRESS) {
				keyPressedA = true;
				previousPosX = -1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedA = false;
				previousPosX = 0;
			}
		} else if (key == GLFW_KEY_S) {
			if (action == GLFW_PRESS) {
				keyPressedS = true;
				previousPosZ = 1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedS = false;
				previousPosZ = 0;
			}
		} else if (key == GLFW_KEY_D) {
			if (action == GLFW_PRESS) {
				keyPressedD = true;
				previousPosX = 1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedD = false;
				previousPosX = 0;
			}
		} else if (key == GLFW_KEY_W) {
			if (action == GLFW_PRESS) {
				keyPressedW = true;
				previousPosZ = -1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedW = false;
				previousPosZ = 0;
			}
		} else if (key == GLFW_KEY_Q) {
			if (action == GLFW_PRESS) {
				keyPressedQ = true;
				previousPosY = -1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedQ = false;
				previousPosY = 0;
			}
		} else if (key == GLFW_KEY_E) {
			if (action == GLFW_PRESS) {
				keyPressedE = true;
				previousPosY = 1;
			}
			else if (action == GLFW_RELEASE) {
				keyPressedE = false;
				previousPosY = 0;
			}
		} else if (key == GLFW_KEY_R) {
			if (action == GLFW_PRESS) {
				camera->ResetCamera();
				camera->UpdateOrbit(0.0f, 0.0f, 0.0f);
			}
		}

		if (keyPressedA || keyPressedS || keyPressedD || keyPressedW || keyPressedQ || keyPressedE) {
            glm::mat4 invView = glm::inverse(camera->GetCBO().viewMatrix);
            glm::vec4 forward = invView * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
            forward.y = 0.0f;
            forward = glm::normalize(forward);
            glm::vec4 right = invView * glm::vec4(1.0f, 0.0f, 0.0f, 0.0);
            glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
            glm::vec3 dir = glm::vec3((float)previousPosX * right + (float)previousPosY * up + (float)previousPosZ * forward) * stepSize;
            //dir = glm::vec3(camera->GetCBO().viewMatrix * glm::vec4(dir, 0.0f));
			camera->PanCamera(dir.x, dir.y, dir.z);
			camera->UpdateOrbit(0.0f, 0.0f, 0.0f);
		}
	}

    void mouseDownCallback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                leftMouseDown = true;
                glfwGetCursorPos(window, &previousX, &previousY);
            }
            else if (action == GLFW_RELEASE) {
                leftMouseDown = false;
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                rightMouseDown = true;
                glfwGetCursorPos(window, &previousX, &previousY);
            }
            else if (action == GLFW_RELEASE) {
                rightMouseDown = false;
            }
		}
    }

    void mouseMoveCallback(GLFWwindow* window, double xPosition, double yPosition) {
        if (leftMouseDown) {
            double sensitivity = 0.5;
            float deltaX = static_cast<float>((previousX - xPosition) * sensitivity);
            float deltaY = static_cast<float>((previousY - yPosition) * sensitivity);

            camera->UpdateOrbit(deltaX, deltaY, 0.0f);

            previousX = xPosition;
            previousY = yPosition;
        } else if (rightMouseDown) {
            double deltaZ = static_cast<float>((previousY - yPosition) * 0.05);

            camera->UpdateOrbit(0.0f, 0.0f, deltaZ);

            previousY = yPosition;
		}
    }
}

int main() {
    static constexpr char* applicationName = "Vulkan Procedural Terrain Generator";
    InitializeWindow(640, 480, applicationName);

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    Instance* instance = new Instance(applicationName, glfwExtensionCount, glfwExtensions);

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance->GetVkInstance(), GetGLFWWindow(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }

    instance->PickPhysicalDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit | QueueFlagBit::ComputeBit | QueueFlagBit::PresentBit, surface);

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.tessellationShader = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    device = instance->CreateDevice(QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit | QueueFlagBit::ComputeBit | QueueFlagBit::PresentBit, deviceFeatures);

    swapChain = device->CreateSwapChain(surface, 5);

    camera = new Camera(device, 640.f / 480.f);

    VkCommandPoolCreateInfo transferPoolInfo = {};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.queueFamilyIndex = device->GetInstance()->GetQueueFamilyIndices()[QueueFlags::Transfer];
    transferPoolInfo.flags = 0;

    VkCommandPool transferCommandPool;
    if (vkCreateCommandPool(device->GetVkDevice(), &transferPoolInfo, nullptr, &transferCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    VkImage grassImage;
    VkDeviceMemory grassImageMemory;
    Image::FromFile(device,
        transferCommandPool,
        "images/grass.jpg",
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        grassImage,
        grassImageMemory
    );

    float planeDim = 15.f;
    float halfWidth = planeDim * 0.5f;
    Model* plane = new Model(device, transferCommandPool,
        {
            { { -halfWidth, 0.0f, halfWidth }, { 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
            { { halfWidth, 0.0f, halfWidth }, { 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
            { { halfWidth, 0.0f, -halfWidth }, { 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
            { { -halfWidth, 0.0f, -halfWidth }, { 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
        },
        { 0, 1, 2, 2, 3, 0 }
    );
    plane->SetTexture(grassImage);
    
    Blades* blades = new Blades(device, transferCommandPool, planeDim);

    vkDestroyCommandPool(device->GetVkDevice(), transferCommandPool, nullptr);

    Scene* scene = new Scene(device);
    //scene->AddModel(plane);
    scene->AddBlades(blades);

    renderer = new Renderer(device, swapChain, scene, camera);

    glfwSetWindowSizeCallback(GetGLFWWindow(), resizeCallback);
    glfwSetMouseButtonCallback(GetGLFWWindow(), mouseDownCallback);
	glfwSetCursorPosCallback(GetGLFWWindow(), mouseMoveCallback);
	glfwSetKeyCallback(GetGLFWWindow(), keyPressCallback);

    while (!ShouldQuit()) {
        glfwPollEvents();
        scene->UpdateTime();
        renderer->Frame();
    }

    vkDeviceWaitIdle(device->GetVkDevice());

    vkDestroyImage(device->GetVkDevice(), grassImage, nullptr);
    vkFreeMemory(device->GetVkDevice(), grassImageMemory, nullptr);

    delete scene;
    delete plane;
    delete blades;
    delete camera;
    delete renderer;
    delete swapChain;
    delete device;
    delete instance;
    DestroyWindow();
    return 0;
}

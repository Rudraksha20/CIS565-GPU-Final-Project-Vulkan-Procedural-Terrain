#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
    Renderer() = delete;
    Renderer(Device* device, SwapChain* swapChain, Scene* scene, Camera* camera);
    ~Renderer();

    void CreateCommandPools();

    void CreateRenderPass();

    void CreateCameraDescriptorSetLayout();
    void CreateModelDescriptorSetLayout();
    void CreateTimeDescriptorSetLayout();
    void CreateComputeDescriptorSetLayout();
	void CreateTexDescriptorSetLayout();

    void CreateDescriptorPool();

    void CreateCameraDescriptorSet();
    void CreateModelDescriptorSets();
	void CreateTerrainDescriptorSets();
    void CreateTimeDescriptorSet();
    void CreateComputeDescriptorSets();
	void CreateTexDescriptorSet();

    void CreateGraphicsPipeline();
	void CreateTerrainPipeline();
    void CreateComputePipeline();

    void CreateFrameResources();
    void DestroyFrameResources();
    void RecreateFrameResources();

    void RecordCommandBuffers();
    void RecordComputeCommandBuffer();

    void Frame();

private:
    Device* device;
    VkDevice logicalDevice;
    SwapChain* swapChain;
    Scene* scene;
    Camera* camera;

    VkCommandPool graphicsCommandPool;
    VkCommandPool computeCommandPool;

    VkRenderPass renderPass;

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorSetLayout modelDescriptorSetLayout;
    VkDescriptorSetLayout timeDescriptorSetLayout;
	VkDescriptorSetLayout terrainComputeDescriptorSetLayout;
	VkDescriptorSetLayout texDescriptorSetLayout;
    
    VkDescriptorPool descriptorPool;

    VkDescriptorSet cameraDescriptorSet;
    std::vector<VkDescriptorSet> modelDescriptorSets;
	std::vector<VkDescriptorSet> terrainDescriptorSets;
    VkDescriptorSet timeDescriptorSet;
	std::vector<VkDescriptorSet> terrainComputeDescriptorSets;
	VkDescriptorSet texDescriptorSet;

    VkPipelineLayout graphicsPipelineLayout;
	VkPipelineLayout terrainPipelineLayout;
    VkPipelineLayout computePipelineLayout;
	// newly added
	VkPipelineLayout deferredPipelineLayout;

    VkPipeline graphicsPipeline;
	VkPipeline terrainPipeline;
    VkPipeline computePipeline;
	// newly added
	VkPipeline deferredPipeline;

    std::vector<VkImageView> imageViews;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    std::vector<VkFramebuffer> framebuffers;

	// sampler
	VkSampler Sampler;

	// Grass texture
	VkImage grassImage;
	VkDeviceMemory grassImageMemory;
	VkImageView grassImageView;

	// skybox texture
	VkImage skyboxImage;
	VkDeviceMemory skyboxImageMemory;
	VkImageView skyboxImageView;

    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandBuffer computeCommandBuffer;
};

#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Scene.h"
#include "Camera.h"

class VisibilityRenderer {
public:
    VisibilityRenderer() = delete;
    VisibilityRenderer(Device* device, SwapChain* swapChain, Scene* scene, Camera* camera);
    ~VisibilityRenderer();

    void CreateCommandPools();

    void CreateDeferredRenderPass();
    void CreateRenderPass();

    void CreateCameraDescriptorSetLayout();
    void CreateModelDescriptorSetLayout();
    void CreateTimeDescriptorSetLayout();
    void CreateComputeDescriptorSetLayout();

    void CreateDescriptorPool();

    void CreateCameraDescriptorSet();
    void CreateModelDescriptorSets();
	void CreateTerrainDescriptorSets();
    void CreateTimeDescriptorSet();
    void CreateComputeDescriptorSets();

    void CreateGraphicsPipeline();
	void CreateTerrainPipeline();
    void CreateComputePipeline();

    void CreateFrameResources();
    void DestroyFrameResources();
    void RecreateFrameResources();

    void RecordCommandBuffers();
    void RecordComputeCommandBuffer();
    void RecordDeferredCommandBuffer();

    void Frame();

private:
    Device* device;
    VkDevice logicalDevice;
    SwapChain* swapChain;
    Scene* scene;
    Camera* camera;

    VkCommandPool graphicsCommandPool;
    VkCommandPool computeCommandPool;

    VkRenderPass deferredRenderPass;
    VkRenderPass renderPass;

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorSetLayout modelDescriptorSetLayout;
    VkDescriptorSetLayout timeDescriptorSetLayout;
	VkDescriptorSetLayout terrainComputeDescriptorSetLayout;

    VkDescriptorPool descriptorPool;

    VkDescriptorSet cameraDescriptorSet;
    std::vector<VkDescriptorSet> modelDescriptorSets;
	std::vector<VkDescriptorSet> terrainDescriptorSets;
    VkDescriptorSet timeDescriptorSet;
	std::vector<VkDescriptorSet> terrainComputeDescriptorSets;

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

    VkImage deferredVisibilityImage;
    VkDeviceMemory deferredVisibilityImageMemory;
    VkImageView deferredVisibilityImageView;

    VkImage deferredDepthImage;
    VkDeviceMemory deferredDepthImageMemory;
    VkImageView deferredDepthImageView;

    VkFramebuffer deferredFramebuffer;
    VkSampler deferredSampler;

    VkSemaphore deferredSemaphore;

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
    VkCommandBuffer deferredCommandBuffer;
};

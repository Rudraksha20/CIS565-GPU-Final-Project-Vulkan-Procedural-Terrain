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
	void CreateDeferredRenderPass(); // replaces CreateRenderPass and CreateFrameResource

    void CreateCameraDescriptorSetLayout();
    void CreateModelDescriptorSetLayout();
    void CreateTimeDescriptorSetLayout();
    void CreateComputeDescriptorSetLayout();

    void CreateDescriptorPool();

    void CreateCameraDescriptorSet();
    void CreateModelDescriptorSets();
    void CreateGrassDescriptorSets();
    void CreateTimeDescriptorSet();
    void CreateComputeDescriptorSets();

    void CreateGraphicsPipeline();
    void CreateGrassPipeline();
    void CreateComputePipeline();

    void CreateFrameResources();
    void DestroyFrameResources();
    void RecreateFrameResources();

    void RecordCommandBuffers();
    void RecordComputeCommandBuffer();
	void buildDeferredCommandBuffer(); // newly added

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
	VkRenderPass deferredRenderPass; // newly added

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorSetLayout modelDescriptorSetLayout;
    VkDescriptorSetLayout timeDescriptorSetLayout;
    VkDescriptorSetLayout grassComputeDescriptorSetLayout;
    
    VkDescriptorPool descriptorPool;

    VkDescriptorSet cameraDescriptorSet;
    std::vector<VkDescriptorSet> modelDescriptorSets;
    std::vector<VkDescriptorSet> grassDescriptorSets;
    VkDescriptorSet timeDescriptorSet;
    std::vector<VkDescriptorSet> grassComputeDescriptorSets;

    VkPipelineLayout graphicsPipelineLayout;
    VkPipelineLayout grassPipelineLayout;
    VkPipelineLayout computePipelineLayout;
	// newly added
	VkPipelineLayout deferredPipelineLayout;

    VkPipeline graphicsPipeline;
    VkPipeline grassPipeline;
    VkPipeline computePipeline;
	// newly added
	VkPipeline deferredPipeline;

	// deferred pipeline segments, newly added
	VkImage deferredPositionImage;
	VkDeviceMemory deferredPositionMemory;
	VkImageView deferredPositionView;

	VkImage deferredNormalImage;
	VkDeviceMemory deferredNormalMemory;
	VkImageView deferredNormalView;

	VkImage deferredColorImage;
	VkDeviceMemory deferredColorMemory;
	VkImageView deferredColorView;

	VkImage deferredDepthImage;
	VkDeviceMemory deferredDepthMemory;
	VkImageView deferredDepthView;

	VkFramebuffer deferredFrameBuffer;

	// One sampler for the frame buffer color attachments
	VkSampler deferredColorSampler;

	VkSemaphore deferredSemaphore;
	//------

    std::vector<VkImageView> imageViews;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    std::vector<VkFramebuffer> framebuffers;

    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandBuffer computeCommandBuffer;

	VkCommandBuffer deferredCommandBuffer; // newly added
};

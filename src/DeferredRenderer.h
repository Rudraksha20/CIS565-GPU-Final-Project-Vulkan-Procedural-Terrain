#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Scene.h"
#include "Camera.h"

class DeferredRenderer {
public:
    DeferredRenderer() = delete;
    DeferredRenderer(Device* device, SwapChain* swapChain, Scene* scene, Camera* camera);
    ~DeferredRenderer();

    void CreateCommandPools();

    void CreateDeferredRenderPass();
    void CreateRenderPass();

    void CreateCameraDescriptorSetLayout();
    void CreateModelDescriptorSetLayout();
    void CreateTimeDescriptorSetLayout();
    void CreateComputeDescriptorSetLayout();
    void CreateTexDescriptorSetLayout();

    void CreateDescriptorPool();

    void CreateCameraDescriptorSet();
    void CreateModelDescriptorSets();
    void CreateGrassDescriptorSets();
    void CreateTimeDescriptorSet();
    void CreateComputeDescriptorSets();
    void CreateTexDescriptorSet();

    void CreateGraphicsPipeline();
    void CreateGrassPipeline();
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
    VkDescriptorSetLayout grassComputeDescriptorSetLayout;
    VkDescriptorSetLayout texDescriptorSetLayout;

    VkDescriptorPool descriptorPool;

    VkDescriptorSet cameraDescriptorSet;
    std::vector<VkDescriptorSet> modelDescriptorSets;
    std::vector<VkDescriptorSet> grassDescriptorSets;
    VkDescriptorSet timeDescriptorSet;
    std::vector<VkDescriptorSet> grassComputeDescriptorSets;
    VkDescriptorSet texDescriptorSet;

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

    std::vector<VkImageView> imageViews;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    std::vector<VkFramebuffer> framebuffers;

    VkImage deferredAlbedoImage;
    VkDeviceMemory deferredAlbedoImageMemory;
    VkImageView deferredAlbedoImageView;

    VkImage deferredPositionImage;
    VkDeviceMemory deferredPositionImageMemory;
    VkImageView deferredPositionImageView;

    VkImage deferredNormalImage;
    VkDeviceMemory deferredNormalImageMemory;
    VkImageView deferredNormalImageView;

    VkImage deferredDepthImage;
    VkDeviceMemory deferredDepthImageMemory;
    VkImageView deferredDepthImageView;

    VkFramebuffer deferredFramebuffer;
    VkSampler deferredSampler;

	// Grass texture
	VkImage grassImage;
	VkDeviceMemory grassImageMemory;
	VkImageView grassImageView;

	// Snow texture
	VkImage snowImage;
	VkDeviceMemory snowImageMemory;
	VkImageView snowImageView;

    VkSemaphore deferredSemaphore;

    // skybox texture
    VkImage skyboxImage;
    VkDeviceMemory skyboxImageMemory;
    VkImageView skyboxImageView;

    std::vector<VkCommandBuffer> commandBuffers;
    VkCommandBuffer computeCommandBuffer;
    VkCommandBuffer deferredCommandBuffer;
};

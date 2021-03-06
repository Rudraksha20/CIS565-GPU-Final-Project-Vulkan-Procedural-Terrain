#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include "Model.h"

constexpr static unsigned int NUM_TERRAINGRIDS = (1 << 8);
constexpr static float MIN_HEIGHT1 = 1.3f;
constexpr static float MAX_HEIGHT1 = 2.5f;
constexpr static float MIN_WIDTH1 = 0.1f;
constexpr static float MAX_WIDTH1 = 0.14f;
constexpr static float MIN_BEND1 = 7.0f;
constexpr static float MAX_BEND1 = 13.0f;

struct TerrainGrid {
    // Position and direction
    glm::vec4 v0;
    // Bezier point and height
    glm::vec4 v1;
    // Physical model guide and width
    glm::vec4 v2;
    // Up vector and stiffness coefficient
    glm::vec4 up;
    // special vec4 for custom colors: .xyz is RGB, .w is whether to use this color
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(TerrainGrid);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};

        // v0
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(TerrainGrid, v0);

        // v1
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(TerrainGrid, v1);

        // v2
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(TerrainGrid, v2);

        // up
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(TerrainGrid, up);

        // color
        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(TerrainGrid, color);

        return attributeDescriptions;
    }
};

struct TerrainGridDrawIndirect {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

class TerrainGrids : public Model {
private:
    VkBuffer terrainGridsBuffer;
    VkBuffer culledTerrainGridsBuffer;
    VkBuffer numTerrainGridsBuffer;

    VkDeviceMemory terrainGridsBufferMemory;
    VkDeviceMemory culledTerrainGridsBufferMemory;
    VkDeviceMemory numTerrainGridsBufferMemory;

public:
	TerrainGrids(Device* device, VkCommandPool commandPool, float planeDim);
    VkBuffer GetTerrainGridsBuffer() const;
    VkBuffer GetCulledTerrainGridsBuffer() const;
    VkBuffer GetNumTerrainGridsBuffer() const;
    ~TerrainGrids();
};

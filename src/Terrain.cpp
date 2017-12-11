#include <vector>
#include "Terrain.h"
#include "BufferUtils.h"

float generateRandomFloat1() {
    return rand() / (float)RAND_MAX;
}

TerrainGrids::TerrainGrids(Device* device, VkCommandPool commandPool, float planeDim) : Model(device, commandPool, {}, {}) {
    std::vector<TerrainGrid> terrainGrids;
	terrainGrids.reserve(NUM_TERRAINGRIDS);
    //srand(123);
    for (int i = 0; i < NUM_TERRAINGRIDS; i++) {
		TerrainGrid currentTerrainGrid = TerrainGrid();

        glm::vec3 terrainGridUp(0.0f, 1.0f, 0.0f);
		planeDim = 5;

        // Generate positions and direction (v0)
        float x = (float)(i % 16) * planeDim - 8 * planeDim;
        float y = 1.0f;
        float z = (float)(i / 16) * planeDim - 8 * planeDim;
        float direction = generateRandomFloat1() * 2.f * 3.14159265f;
        glm::vec3 terrainGridPosition(x, y, z);
        //currentBlade.v0 = glm::vec4(bladePosition, direction);
        currentTerrainGrid.v0 = glm::vec4(terrainGridPosition, planeDim);

        // Bezier point and height (v1)
        float height = MIN_HEIGHT1 + (generateRandomFloat1() * (MAX_HEIGHT1 - MIN_HEIGHT1));
        currentTerrainGrid.v1 = glm::vec4(terrainGridPosition + terrainGridUp * height, height);

        // Physical model guide and width (v2)
        float width = MIN_WIDTH1 + (generateRandomFloat1() * (MAX_WIDTH1 - MIN_WIDTH1));
        currentTerrainGrid.v2 = glm::vec4(terrainGridPosition + terrainGridUp * height, width);

        // Up vector and stiffness coefficient (up)
        float stiffness = MIN_BEND1 + (generateRandomFloat1() * (MAX_BEND1 - MIN_BEND1));
        currentTerrainGrid.up = glm::vec4(terrainGridUp, stiffness);

        // custom color
        currentTerrainGrid.color = glm::vec4(1.0f);

		terrainGrids.push_back(currentTerrainGrid);
    }

	TerrainGridDrawIndirect indirectDraw;
    indirectDraw.vertexCount = NUM_TERRAINGRIDS;
    indirectDraw.instanceCount = 1;
    indirectDraw.firstVertex = 0;
    indirectDraw.firstInstance = 0;

    BufferUtils::CreateBufferFromData(device, commandPool, terrainGrids.data(), NUM_TERRAINGRIDS * sizeof(TerrainGrid), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, terrainGridsBuffer, terrainGridsBufferMemory);
    BufferUtils::CreateBuffer(device, NUM_TERRAINGRIDS * sizeof(TerrainGrid), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, culledTerrainGridsBuffer, culledTerrainGridsBufferMemory);
    BufferUtils::CreateBufferFromData(device, commandPool, &indirectDraw, sizeof(TerrainGridDrawIndirect), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, numTerrainGridsBuffer, numTerrainGridsBufferMemory);
}

VkBuffer TerrainGrids::GetTerrainGridsBuffer() const {
    return terrainGridsBuffer;
}

VkBuffer TerrainGrids::GetCulledTerrainGridsBuffer() const {
    return culledTerrainGridsBuffer;
}

VkBuffer TerrainGrids::GetNumTerrainGridsBuffer() const {
    return numTerrainGridsBuffer;
}

TerrainGrids::~TerrainGrids() {
    vkDestroyBuffer(device->GetVkDevice(), terrainGridsBuffer, nullptr);
    vkFreeMemory(device->GetVkDevice(), terrainGridsBufferMemory, nullptr);
    vkDestroyBuffer(device->GetVkDevice(), culledTerrainGridsBuffer, nullptr);
    vkFreeMemory(device->GetVkDevice(), culledTerrainGridsBufferMemory, nullptr);
    vkDestroyBuffer(device->GetVkDevice(), numTerrainGridsBuffer, nullptr);
    vkFreeMemory(device->GetVkDevice(), numTerrainGridsBufferMemory, nullptr);
}

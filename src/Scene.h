#pragma once

#include <glm/glm.hpp>
#include <chrono>

#include "Model.h"
#include "Terrain.h"

#define MAX_DELTA_COUNT 2000

using namespace std::chrono;

struct Time {
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
};

class Scene {
private:
    Device* device;
    
    VkBuffer timeBuffer;
    VkDeviceMemory timeBufferMemory;
    Time time;
    
    void* mappedData;

    std::vector<Model*> models;
    std::vector<TerrainGrids*> terrainGrids;

    float deltaAcc; // accumulates deltaTime
    int deltaCount; // counts how many times deltaTime has been accumulated

high_resolution_clock::time_point startTime = high_resolution_clock::now();

public:
    Scene() = delete;
    Scene(Device* device);
    ~Scene();

    const std::vector<Model*>& GetModels() const;
    const std::vector<TerrainGrids*>& GetTerrainGrids() const;
    
    void AddModel(Model* model);
    void AddTerrainGrids(TerrainGrids* terrainGrids);

    VkBuffer GetTimeBuffer() const;

    void UpdateTime();
};

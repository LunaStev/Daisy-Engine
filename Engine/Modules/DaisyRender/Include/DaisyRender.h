#pragma once

#include "Core/Module.h"
#include "Core/Math.h"
#include "DaisyPlatform.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <unordered_map>

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

namespace Daisy {

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoord;
    Vector4 color{1, 1, 1, 1};
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t id = 0;
};

struct Material {
    Vector4 albedo{1, 1, 1, 1};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float emission = 0.0f;
    uint32_t albedoTexture = 0;
    uint32_t normalTexture = 0;
    uint32_t materialTexture = 0;
};

struct RenderObject {
    uint32_t meshId = 0;
    uint32_t materialId = 0;
    Matrix4 transform;
    bool visible = true;
    float lodDistance = 0.0f;
};

struct Camera {
    Vector3 position{0, 0, 0};
    Vector3 target{0, 0, -1};
    Vector3 up{0, 1, 0};
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100000.0f;
    float aspect = 16.0f / 9.0f;
};

struct Light {
    enum Type { Directional, Point, Spot } type;
    Vector3 position{0, 0, 0};
    Vector3 direction{0, -1, 0};
    Vector4 color{1, 1, 1, 1};
    float intensity = 1.0f;
    float range = 10.0f;
    float spotAngle = 45.0f;
};

struct ProceduralSettings {
    int planetDetail = 6;
    int cityDetail = 4;
    int shipDetail = 3;
    float terrainScale = 1000.0f;
    float buildingDensity = 0.5f;
    uint32_t seed = 12345;
};

class DaisyRender : public Module {
public:
    DaisyRender() : Module("DaisyRender") {}
    virtual ~DaisyRender() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    void SetWindow(Window* window) { m_window = window; }
    Window* GetWindow() const { return m_window; }
    
    bool InitializeVulkan();
    void ShutdownVulkan();
    
    uint32_t CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    uint32_t CreateMaterial(const Material& material);
    uint32_t CreateTexture(const void* data, int width, int height, int channels);
    uint32_t CreateRenderObject(uint32_t meshId, uint32_t materialId, const Matrix4& transform);
    
    void DestroyMesh(uint32_t id);
    void DestroyMaterial(uint32_t id);
    void DestroyTexture(uint32_t id);
    void DestroyRenderObject(uint32_t id);
    
    void SetCamera(const Camera& camera) { m_camera = camera; }
    Camera& GetCamera() { return m_camera; }
    
    void AddLight(const Light& light);
    void RemoveLight(size_t index);
    void ClearLights() { m_lights.clear(); }
    
    void SetViewport(int x, int y, int width, int height);
    void SetClearColor(float r, float g, float b, float a = 1.0f);
    
    void BeginFrame();
    void EndFrame();
    void Present();
    
    void SetProceduralSettings(const ProceduralSettings& settings) { m_proceduralSettings = settings; }
    void EnableLOD(bool enable) { m_lodEnabled = enable; }
    void SetLODDistances(float low, float medium, float high);
    
    void EnableInfiniteWorld(bool enable) { m_infiniteWorldEnabled = enable; }
    void SetWorldScale(double scale) { m_worldScale = scale; }
    void SetObserverPosition(const Vector3& position) { m_observerPosition = position; }
    
    uint32_t GenerateProceduralPlanet(const Vector3& position, float radius);
    uint32_t GenerateProceduralCity(const Vector3& position, float size);
    uint32_t GenerateProceduralShip(const Vector3& position, float size);
    
private:
    bool CreateVulkanInstance();
    bool CreateDevice();
    bool CreateSwapchain();
    bool CreateRenderPass();
    bool CreateCommandBuffers();
    void RecreateSwapchain();
    
    Window* m_window = nullptr;
    
    void UpdateLOD();
    void UpdateCulling();
    void RenderFrame();
    void GenerateProceduralContent();
    
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkFramebuffer> m_swapchainFramebuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;
    
    std::unordered_map<uint32_t, std::unique_ptr<Mesh>> m_meshes;
    std::unordered_map<uint32_t, Material> m_materials;
    std::unordered_map<uint32_t, VkImage> m_textures;
    std::unordered_map<uint32_t, RenderObject> m_renderObjects;
    
    std::vector<Light> m_lights;
    Camera m_camera;
    
    uint32_t m_nextMeshId = 1;
    uint32_t m_nextMaterialId = 1;
    uint32_t m_nextTextureId = 1;
    uint32_t m_nextObjectId = 1;
    
    Vector4 m_clearColor{0.1f, 0.1f, 0.2f, 1.0f};
    
    bool m_lodEnabled = true;
    float m_lodDistanceLow = 100.0f;
    float m_lodDistanceMedium = 500.0f;
    float m_lodDistanceHigh = 2000.0f;
    
    bool m_infiniteWorldEnabled = true;
    double m_worldScale = 1e12;
    Vector3 m_observerPosition{0, 0, 0};
    
    ProceduralSettings m_proceduralSettings;
    
    int m_viewportX = 0, m_viewportY = 0;
    int m_viewportWidth = 1920, m_viewportHeight = 1080;
};

}
//// RayTracingSystem.hpp //////////////////////////////////////////////////////
//
//  Author:      Parel Development Team (Arosen Systems / Parel LLC)
//  Date:        12/01/2026
//  Description: Ray tracing systems for realistic lighting and reflections
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../components/RayTracing.hpp"
#include "../components/Transform.hpp"
#include "../components/Render3D.hpp"
#include "../ecs/System.hpp"
#include "Timestep.hpp"

namespace parallax {

    /**
     * @brief Manages ray tracing backend initialization and configuration
     * Initializes the ray tracing API and manages global settings
     */
    class RayTracingInitSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        bool initializeAPI(RayTracingAPI api);
        void shutdownAPI();

        bool m_initialized = false;
        RayTracingAPI m_currentAPI = RayTracingAPI::AUTO;
    };

    /**
     * @brief Builds and updates acceleration structures for ray tracing
     * Creates BVH/TLAS/BLAS structures for fast ray-scene intersection
     */
    class AccelerationStructureBuildSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void buildTopLevelAS();
        void buildBottomLevelAS(entt::entity entity, AccelerationStructureComponent& as);
        void updateDynamicInstances();

        bool checkIfNeedsRebuild(const AccelerationStructureComponent& as,
                                  const TransformComponent& transform);
    };

    /**
     * @brief Updates global illumination probes
     * Computes indirect lighting from GI probes
     */
    class GlobalIlluminationSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void updateProbe(entt::entity entity, GlobalIlluminationProbeComponent& probe);
        void traceGIRays(GlobalIlluminationProbeComponent& probe);
        void blendProbes();

        // Probe irradiance data cache
        struct ProbeData {
            std::vector<glm::vec3> irradiance;  // Per-probe RGB values
            bool isValid = false;
        };
        std::map<entt::entity, ProbeData> m_probeCache;
    };

    /**
     * @brief Updates reflection probes
     * Captures environment maps for realistic reflections
     */
    class ReflectionProbeSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void updateProbe(entt::entity entity, ReflectionProbeComponent& probe);
        void captureEnvironment(ReflectionProbeComponent& probe);
        void applyParallaxCorrection(ReflectionProbeComponent& probe);
    };

    /**
     * @brief Main ray tracing renderer
     * Performs ray traced rendering (shadows, reflections, GI, AO)
     */
    class RayTracingRenderSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        // Ray generation
        void generatePrimaryRays();
        void generateShadowRays(const glm::vec3& hitPoint, const glm::vec3& normal);
        void generateReflectionRays(const glm::vec3& hitPoint, const glm::vec3& direction);

        // Shading
        glm::vec3 shadeSurface(const glm::vec3& hitPoint, const glm::vec3& normal,
                                const RayTracedMaterialComponent& material);
        glm::vec3 computeDirectLighting(const glm::vec3& hitPoint, const glm::vec3& normal);
        glm::vec3 computeIndirectLighting(const glm::vec3& hitPoint, const glm::vec3& normal);

        // Ray tracing passes
        void renderRayTracedShadows();
        void renderRayTracedReflections();
        void renderRayTracedGI();
        void renderRayTracedAO();

        // Denoising
        void applyDenoising();

        // Performance
        void checkPerformance();
        void adaptiveQuality();  // Reduce quality if frame time too high
    };

    /**
     * @brief Denoising system for ray-traced images
     * Applies spatial, temporal, or AI-based denoising
     */
    class DenoisingSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void spatialDenoise();
        void temporalDenoise();
        void aiDenoise(DenoisingComponent::DenoiserType type);

        // Motion vector generation for temporal denoising
        void generateMotionVectors();

        // Frame history for temporal accumulation
        struct FrameHistory {
            std::vector<glm::vec4> colorBuffer;
            std::vector<glm::vec3> normalBuffer;
            std::vector<float> depthBuffer;
            uint32_t frameIndex = 0;
        };
        FrameHistory m_history;
    };

    /**
     * @brief Hybrid rendering system (rasterization + ray tracing)
     * Combines traditional rasterization with selective ray tracing
     */
    class HybridRenderingSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void renderRasterized();        // Base rendering (rasterization)
        void renderRayTracedLayer();    // RT effects on top

        // Classify materials (which need RT?)
        void classifyMaterials();
        std::vector<entt::entity> m_reflectiveMaterials;
        std::vector<entt::entity> m_refractiveMaterials;
        std::vector<entt::entity> m_emissiveMaterials;
    };

    /**
     * @brief VR-optimized ray tracing system
     * Special optimizations for VR (foveated RT, async reprojection)
     */
    class VRRayTracingSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void foveatedRayTracing();      // Variable rate RT based on gaze
        void asyncReprojection();       // Reproject previous frame while RT renders
        void stereoOptimization();      // Optimize for dual-eye rendering

        // Eye tracking integration
        glm::vec2 getGazePoint();
        float getFoveationRadius();
    };

    /**
     * @brief Performance monitoring for ray tracing
     * Tracks RT performance and adjusts quality dynamically
     */
    class RayTracingPerformanceSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void measureFrameTime();
        void updateStatistics();
        void adjustQuality();           // Dynamic quality scaling

        // VR target: 11.1ms per frame (90 FPS)
        const float VR_TARGET_FRAME_TIME_MS = 11.1f;
        const float DESKTOP_TARGET_FRAME_TIME_MS = 16.6f;  // 60 FPS
    };

} // namespace parallax

//// RayTracing.hpp ////////////////////////////////////////////////////////////
//
//  Author:      Parel Development Team (Arosen Systems / Parel LLC)
//  Date:        12/01/2026
//  Description: Ray tracing components for realistic lighting and reflections
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

namespace nexo {

    /**
     * @brief Ray tracing backend API
     */
    enum class RayTracingAPI : uint8_t {
        SOFTWARE,           // CPU-based software ray tracing (fallback)
        DXR,                // DirectX Raytracing (Windows, DX12)
        VULKAN_RT,          // Vulkan Ray Tracing (cross-platform)
        OPTIX,              // NVIDIA OptiX (NVIDIA GPUs only)
        AUTO                // Automatically select best available
    };

    /**
     * @brief Ray tracing quality preset
     */
    enum class RayTracingQuality : uint8_t {
        LOW,                // 1 ray per pixel, no denoising
        MEDIUM,             // 2 rays per pixel, basic denoising
        HIGH,               // 4 rays per pixel, advanced denoising
        ULTRA,              // 8+ rays per pixel, full denoising
        CUSTOM              // User-defined settings
    };

    /**
     * @brief Global ray tracing settings component
     * Attach to a singleton entity to configure ray tracing
     */
    struct RayTracingSettingsComponent {
        bool enabled = true;
        RayTracingAPI api = RayTracingAPI::AUTO;
        RayTracingQuality quality = RayTracingQuality::MEDIUM;

        // Ray budget (lower for VR to maintain 90 FPS)
        uint32_t maxRaysPerPixel = 2;           // For VR: 1-2, Desktop: 4-8
        uint32_t maxBounces = 3;                // Light bounce depth (1-5)
        uint32_t shadowRays = 1;                // Rays per shadow (1-4)

        // Denoising (critical for VR to reduce noise at low ray counts)
        bool enableDenoising = true;
        float denoiserStrength = 0.7f;          // 0.0 - 1.0

        // Performance
        bool adaptiveRayCounts = true;          // Reduce rays in motion
        bool asyncCompute = true;               // Use async compute queue
        float renderScale = 1.0f;               // RT resolution scale (0.5-1.0)

        // Hybrid rendering (combine rasterization + ray tracing)
        bool hybridMode = true;                 // Use RT for specific effects only
        bool rtReflections = true;
        bool rtShadows = true;
        bool rtGlobalIllumination = true;
        bool rtAmbientOcclusion = true;

        // VR-specific
        bool vrOptimizations = true;            // Enable VR-specific optimizations
        bool foveatedRayTracing = false;        // Variable rate ray tracing (requires eye tracking)
    };

    /**
     * @brief Light source for ray tracing
     * Extends existing light components with RT-specific properties
     */
    struct RayTracedLightComponent {
        bool castRayTracedShadows = true;
        bool contributeToGI = true;             // Contribute to global illumination

        // Light properties
        float intensity = 1.0f;
        glm::vec3 color = glm::vec3(1.0f);
        float radius = 0.0f;                    // 0 = point light, >0 = area light

        // Shadows
        float shadowSharpness = 1.0f;           // 0 = soft, 1 = hard
        uint32_t shadowSamples = 1;             // Ray samples for soft shadows

        // Global illumination
        float giIntensity = 1.0f;               // How much this light bounces
        uint32_t giBounces = 2;                 // Max bounces for this light
    };

    /**
     * @brief Material properties for ray tracing
     * Extends Render3D Material with RT-specific properties
     */
    struct RayTracedMaterialComponent {
        // Physically-based properties
        float roughness = 0.5f;                 // 0 = mirror, 1 = diffuse
        float metallic = 0.0f;                  // 0 = dielectric, 1 = metal
        float ior = 1.45f;                      // Index of refraction (glass ~1.5)

        // Transparency
        float transparency = 0.0f;              // 0 = opaque, 1 = fully transparent
        glm::vec3 transmissionColor = glm::vec3(1.0f);

        // Emission
        glm::vec3 emissiveColor = glm::vec3(0.0f);
        float emissiveStrength = 0.0f;

        // Reflections
        bool enableReflections = true;
        float reflectionStrength = 1.0f;
        uint32_t maxReflectionBounces = 3;

        // Refractions (glass, water)
        bool enableRefractions = false;
        float refractionStrength = 1.0f;
        uint32_t maxRefractionBounces = 2;

        // Subsurface scattering (skin, wax, marble)
        bool enableSSS = false;
        glm::vec3 sssColor = glm::vec3(1.0f, 0.8f, 0.8f);
        float sssRadius = 0.1f;

        // Clearcoat (car paint, varnish)
        float clearcoat = 0.0f;                 // 0 = none, 1 = full
        float clearcoatRoughness = 0.1f;
    };

    /**
     * @brief Global illumination probe
     * Pre-baked or dynamic light probe for indirect lighting
     */
    struct GlobalIlluminationProbeComponent {
        enum class ProbeType : uint8_t {
            STATIC,         // Pre-baked, never updates
            DYNAMIC,        // Updates every frame (expensive)
            SEMI_DYNAMIC    // Updates on scene changes only
        };

        ProbeType type = ProbeType::SEMI_DYNAMIC;

        // Probe volume
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 extents = glm::vec3(10.0f);   // Box volume size

        // Resolution
        glm::ivec3 probeGrid = glm::ivec3(8, 8, 8);  // Probe density

        // Quality
        uint32_t raysPerProbe = 256;            // More = better quality
        uint32_t bounces = 2;                   // Indirect light bounces

        // Updates
        bool needsUpdate = true;
        float updateInterval = 0.0f;            // 0 = every frame, >0 = seconds
        float timeSinceUpdate = 0.0f;

        // Blending
        float blendWeight = 1.0f;               // For multiple probes
        float falloffDistance = 5.0f;           // Probe influence distance
    };

    /**
     * @brief Reflection probe for local reflections
     * Captures environment for realistic reflections
     */
    struct ReflectionProbeComponent {
        enum class Resolution : uint32_t {
            LOW = 256,
            MEDIUM = 512,
            HIGH = 1024,
            ULTRA = 2048
        };

        Resolution resolution = Resolution::MEDIUM;

        glm::vec3 position = glm::vec3(0.0f);
        float radius = 10.0f;                   // Influence radius

        bool isDynamic = false;                 // Update every frame?
        bool needsUpdate = true;

        // Parallax correction
        bool useParallaxCorrection = true;
        glm::vec3 boxMin = glm::vec3(-10.0f);
        glm::vec3 boxMax = glm::vec3(10.0f);

        // Blending
        float blendDistance = 2.0f;             // Blend with other probes
        int32_t priority = 0;                   // Higher = takes precedence
    };

    /**
     * @brief Denoiser settings for ray-traced images
     * Uses AI-based denoising (NVIDIA NRD, Intel OIDN, or custom)
     */
    struct DenoisingComponent {
        enum class DenoiserType : uint8_t {
            NONE,
            SPATIAL,            // Blur-based (fast, lower quality)
            TEMPORAL,           // Uses previous frames (better quality)
            AI_NVIDIA_NRD,      // NVIDIA Real-time Denoiser (requires RTX)
            AI_INTEL_OIDN,      // Intel Open Image Denoise (CPU/GPU)
            AI_AMD_FRD          // AMD FidelityFX Denoiser
        };

        DenoiserType type = DenoiserType::TEMPORAL;

        bool enabled = true;
        float strength = 0.8f;                  // 0 = no denoising, 1 = max

        // Temporal settings
        bool useMotionVectors = true;
        float temporalBlend = 0.9f;             // 0 = current frame, 1 = history

        // AI denoiser settings
        bool useAlbedoGuide = true;             // Use albedo buffer for guidance
        bool useNormalGuide = true;             // Use normal buffer

        // Performance
        uint32_t denoisePasses = 1;             // Multiple passes = better quality
    };

    /**
     * @brief Acceleration structure for ray tracing
     * Spatial data structure (BVH, Octree) for fast ray-scene intersection
     */
    struct AccelerationStructureComponent {
        enum class BuildMode : uint8_t {
            STATIC,             // Build once, never rebuild (static geometry)
            DYNAMIC,            // Rebuild every frame (animated objects)
            SEMI_STATIC         // Rebuild on transform change
        };

        BuildMode buildMode = BuildMode::SEMI_STATIC;

        bool needsRebuild = true;
        bool isBuilt = false;

        // Quality vs. speed tradeoff
        enum class BuildQuality : uint8_t {
            FAST,               // Fast build, slower trace
            BALANCED,
            HIGH_QUALITY        // Slow build, fast trace
        };
        BuildQuality quality = BuildQuality::BALANCED;

        // GPU resources (opaque pointers to API-specific structures)
        void* topLevelAS = nullptr;             // TLAS (instance acceleration structure)
        void* bottomLevelAS = nullptr;          // BLAS (geometry acceleration structure)

        // Statistics
        uint32_t triangleCount = 0;
        uint32_t instanceCount = 0;
        float buildTimeMs = 0.0f;
    };

    /**
     * @brief Ray tracing performance monitor
     * Tracks RT performance metrics for optimization
     */
    struct RayTracingPerformanceComponent {
        // Frame timing
        float rayGenTimeMs = 0.0f;              // Ray generation
        float intersectionTimeMs = 0.0f;        // Ray-scene intersection
        float shadingTimeMs = 0.0f;             // Shading/lighting
        float denoiseTimeMs = 0.0f;             // Denoising
        float totalRTTimeMs = 0.0f;             // Total RT time

        // Ray statistics
        uint64_t primaryRays = 0;
        uint64_t shadowRays = 0;
        uint64_t reflectionRays = 0;
        uint64_t refractionRays = 0;
        uint64_t totalRays = 0;

        // Performance metrics
        float raysPerSecond = 0.0f;
        float gigatraysPerSecond = 0.0f;        // Industry metric (GRays/s)

        // Memory usage
        uint64_t accelerationStructureMemoryMB = 0;
        uint64_t textureMemoryMB = 0;

        // Warnings
        bool exceededRayBudget = false;
        bool frameTimeTooHigh = false;          // >11ms for VR
    };

} // namespace nexo

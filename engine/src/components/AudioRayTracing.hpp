//// AudioRayTracing.hpp ///////////////////////////////////////////////////////
//
//  Author:      Parel Development Team (Arosen Systems / Parel LLC)
//  Date:        12/01/2026
//  Description: Audio raytracing components for realistic spatial audio
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace parallax {

    /**
     * @brief Audio raytracing API backend
     */
    enum class AudioRayTracingAPI : uint8_t {
        SOFTWARE,           // CPU-based geometric acoustics
        STEAM_AUDIO,        // Valve Steam Audio (high quality, free)
        PHONON,             // Oculus Phonon (VR-optimized, deprecated but good)
        WWISE_SPATIAL,      // Audiokinetic Wwise Spatial Audio
        FMOD_STUDIO,        // FMOD Studio with geometry
        RESONANCE_AUDIO,    // Google Resonance Audio
        CUSTOM              // Custom implementation
    };

    /**
     * @brief Acoustic material properties
     * Defines how sound interacts with surfaces
     */
    struct AcousticMaterialProperties {
        // Absorption coefficients per frequency band (0 = reflects all, 1 = absorbs all)
        float absorption125Hz = 0.1f;       // Low frequency
        float absorption250Hz = 0.15f;
        float absorption500Hz = 0.2f;       // Mid frequency
        float absorption1000Hz = 0.25f;
        float absorption2000Hz = 0.3f;
        float absorption4000Hz = 0.35f;     // High frequency
        float absorption8000Hz = 0.4f;

        // Scattering coefficient (0 = specular reflection, 1 = diffuse scattering)
        float scattering = 0.5f;

        // Transmission (sound passing through material)
        float transmission125Hz = 0.01f;    // How much sound passes through
        float transmission250Hz = 0.01f;
        float transmission500Hz = 0.005f;
        float transmission1000Hz = 0.002f;
        float transmission2000Hz = 0.001f;
        float transmission4000Hz = 0.0005f;
        float transmission8000Hz = 0.0002f;

        // Presets
        static AcousticMaterialProperties Wood();
        static AcousticMaterialProperties Concrete();
        static AcousticMaterialProperties Glass();
        static AcousticMaterialProperties Carpet();
        static AcousticMaterialProperties Metal();
        static AcousticMaterialProperties Water();
        static AcousticMaterialProperties Grass();
        static AcousticMaterialProperties Brick();
    };

    /**
     * @brief Global audio raytracing settings
     * Attach to singleton entity
     */
    struct AudioRayTracingSettingsComponent {
        bool enabled = true;
        AudioRayTracingAPI api = AudioRayTracingAPI::STEAM_AUDIO;

        // Ray budget (audio rays are cheaper than visual rays)
        uint32_t maxRaysPerSource = 512;        // Rays per sound source (128-2048)
        uint32_t maxBounces = 3;                // Sound reflection bounces (1-5)
        float maxRayDistance = 100.0f;          // Max distance for ray propagation (meters)

        // Quality vs performance
        enum class Quality : uint8_t {
            LOW,        // 128 rays, 1 bounce
            MEDIUM,     // 512 rays, 2 bounces
            HIGH,       // 1024 rays, 3 bounces
            ULTRA       // 2048+ rays, 4+ bounces
        };
        Quality quality = Quality::MEDIUM;

        // Update frequency
        float updateRate = 60.0f;               // Hz (lower = better performance)
        bool updateOnlyWhenMoving = true;       // Skip updates when source/listener static

        // Occlusion & obstruction
        bool enableOcclusion = true;            // Sound blocked by geometry
        bool enableObstruction = true;          // Sound partially blocked (goes around corners)
        float occlusionFrequency = 30.0f;       // Update rate for occlusion (Hz)

        // Reverb & reflections
        bool enableReverb = true;
        bool enableEarlyReflections = true;     // First few reflections (crucial for VR)
        bool enableLateReverb = true;           // Diffuse reverb tail

        // Performance
        bool useMultithreading = true;
        uint32_t maxThreads = 4;                // Audio ray tracing threads
        bool asyncCompute = false;              // Use GPU compute for rays (if available)

        // VR-specific
        bool enableHRTF = true;                 // Head-Related Transfer Function (3D audio)
        bool useBinauralRendering = true;       // Realistic 3D audio for headphones
        float hrtfInterpolation = 1.0f;         // 0 = nearest, 1 = full interpolation
    };

    /**
     * @brief Audio source component with raytracing
     * Attach to entities that emit sound
     */
    struct AudioRayTracedSourceComponent {
        // Audio file
        std::string audioFilePath = "";
        bool isLooping = false;
        bool isPlaying = false;

        // Source properties
        float volume = 1.0f;                    // 0.0 - 1.0
        float pitch = 1.0f;                     // 0.5 - 2.0 (half speed - double speed)
        float minDistance = 1.0f;               // No attenuation within this distance
        float maxDistance = 100.0f;             // Max audible distance
        float rolloffFactor = 1.0f;             // Distance attenuation curve

        // Spatialization
        bool is3D = true;                       // 3D positional or 2D ambient?
        float spatialBlend = 1.0f;              // 0 = 2D, 1 = 3D

        // Directivity (sound cone)
        bool directional = false;
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
        float innerConeAngle = 30.0f;           // Full volume cone (degrees)
        float outerConeAngle = 90.0f;           // Falloff cone (degrees)
        float outerConeVolume = 0.1f;           // Volume outside cone

        // Raytracing-specific
        bool enableRayTracing = true;
        uint32_t rayCount = 512;                // Override global ray count
        bool contributeToReverb = true;
        bool enableOcclusion = true;

        // Doppler effect (VR movement)
        bool enableDoppler = true;
        float dopplerScale = 1.0f;              // Doppler effect strength

        // Priority (for voice limiting)
        int32_t priority = 128;                 // 0 = highest, 255 = lowest
    };

    /**
     * @brief Audio listener component (camera/player)
     * Attach to player entity or VR headset
     */
    struct AudioRayTracedListenerComponent {
        bool isActive = true;

        // HRTF (Head-Related Transfer Function) for VR
        bool useHRTF = true;
        std::string hrtfProfilePath = "";       // Custom HRTF profile (or use default)

        // Room awareness
        float roomSize = 10.0f;                 // Hint for reverb
        float dampening = 0.5f;                 // Wall absorption estimate

        // VR-specific
        glm::vec3 headOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        float interauralDistance = 0.18f;       // Distance between ears (meters)
    };

    /**
     * @brief Acoustic geometry component
     * Attach to entities that affect sound propagation
     */
    struct AcousticGeometryComponent {
        bool affectsSound = true;

        // Material
        AcousticMaterialProperties material;

        // Geometry detail
        enum class GeometryDetail : uint8_t {
            LOW,            // Simplified collision mesh
            MEDIUM,         // Visual mesh decimated
            HIGH,           // Full visual mesh
            CUSTOM          // Custom acoustic mesh
        };
        GeometryDetail detail = GeometryDetail::MEDIUM;

        // Custom mesh (if GeometryDetail::CUSTOM)
        std::string customMeshPath = "";

        // Baking
        bool isBaked = false;                   // Pre-baked acoustic data?
        bool dynamic = false;                   // Updates when moved?
    };

    /**
     * @brief Acoustic zone / reverb zone
     * Defines a region with specific reverb characteristics
     */
    struct AcousticZoneComponent {
        // Zone volume
        enum class ZoneShape : uint8_t {
            BOX,
            SPHERE,
            CONVEX_HULL
        };
        ZoneShape shape = ZoneShape::BOX;

        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 extents = glm::vec3(10.0f);   // For box
        float radius = 10.0f;                   // For sphere

        // Reverb preset
        enum class ReverbPreset : uint8_t {
            NONE,
            SMALL_ROOM,
            MEDIUM_ROOM,
            LARGE_ROOM,
            HALL,
            CATHEDRAL,
            CAVE,
            UNDERWATER,
            OUTDOOR,
            CUSTOM
        };
        ReverbPreset preset = ReverbPreset::MEDIUM_ROOM;

        // Custom reverb parameters (if preset = CUSTOM)
        float reverbTime = 1.5f;                // RT60 (seconds)
        float earlyDelay = 0.02f;               // Early reflections delay (seconds)
        float lateDelay = 0.04f;                // Late reverb delay (seconds)
        float density = 1.0f;                   // Echo density (0-1)
        float diffusion = 1.0f;                 // Reverb diffusion (0-1)
        float wetLevel = 0.3f;                  // Reverb mix (0-1)

        // Priority (for overlapping zones)
        int32_t priority = 0;                   // Higher = takes precedence

        // Blending
        float blendDistance = 2.0f;             // Distance to blend with other zones
    };

    /**
     * @brief Occlusion data for sound source
     * Automatically computed by raytracing system
     */
    struct AudioOcclusionComponent {
        // Occlusion state
        bool isOccluded = false;
        float occlusionFactor = 0.0f;           // 0 = unoccluded, 1 = fully occluded

        // Obstruction (partial blocking)
        float obstructionFactor = 0.0f;         // 0 = direct path, 1 = fully obstructed

        // Transmission (sound through walls)
        float transmissionFactor = 0.0f;        // How much sound passes through

        // Per-frequency occlusion (highs attenuate more than lows)
        float occlusionLowFreq = 0.0f;          // <500 Hz
        float occlusionMidFreq = 0.0f;          // 500-2000 Hz
        float occlusionHighFreq = 0.0f;         // >2000 Hz

        // Direct path
        bool hasDirectPath = true;
        float directPathDistance = 0.0f;

        // Diffraction (sound bending around obstacles)
        bool hasDiffractionPath = false;
        float diffractionPathLength = 0.0f;
        float diffractionAttenuation = 0.0f;

        // Update timing
        float timeSinceLastUpdate = 0.0f;
    };

    /**
     * @brief Reverb / reflection data
     * Computed by audio raytracing
     */
    struct AudioReverbComponent {
        // Early reflections (first 50-80ms)
        struct Reflection {
            glm::vec3 direction;                // Direction to reflection source
            float delay;                        // Time delay (seconds)
            float amplitude;                    // Reflection strength (0-1)
            float distance;                     // Path length (meters)
        };
        std::vector<Reflection> earlyReflections;  // Typically 4-16 reflections

        // Late reverb (diffuse tail)
        float reverbTime = 0.0f;                // RT60 (seconds)
        float reverbLevel = 0.0f;               // Reverb amplitude
        float reverbDelay = 0.0f;               // Reverb onset time

        // Room characteristics (estimated from reflections)
        float estimatedRoomSize = 10.0f;        // Cubic meters
        float estimatedAbsorption = 0.5f;       // Average wall absorption

        // Frequency-dependent reverb
        float lowFreqRT60 = 0.0f;               // <500 Hz reverb time
        float midFreqRT60 = 0.0f;               // 500-2000 Hz
        float highFreqRT60 = 0.0f;              // >2000 Hz

        // Update state
        bool needsUpdate = true;
    };

    /**
     * @brief Audio propagation paths
     * Visualize sound ray paths (debug)
     */
    struct AudioPropagationPathsComponent {
        struct Path {
            std::vector<glm::vec3> points;      // Ray path vertices
            float attenuation;                  // Path total attenuation
            float delay;                        // Time delay
            bool isDirect;                      // Direct or reflected?
        };

        std::vector<Path> paths;                // All active propagation paths

        // Visualization
        bool visualize = false;
        glm::vec3 directPathColor = glm::vec3(0.0f, 1.0f, 0.0f);      // Green
        glm::vec3 reflectionColor = glm::vec3(0.0f, 0.5f, 1.0f);      // Blue
        glm::vec3 occludedColor = glm::vec3(1.0f, 0.0f, 0.0f);        // Red
    };

    /**
     * @brief Audio raytracing performance metrics
     */
    struct AudioRayTracingPerformanceComponent {
        // Timing
        float rayGenTimeMs = 0.0f;              // Ray generation time
        float intersectionTimeMs = 0.0f;        // Geometry intersection time
        float reverbComputeTimeMs = 0.0f;       // Reverb calculation time
        float totalTimeMs = 0.0f;               // Total audio RT time

        // Ray statistics
        uint32_t totalRaysCast = 0;
        uint32_t raysHitGeometry = 0;
        uint32_t raysMissed = 0;
        uint32_t activeSources = 0;

        // Performance
        float updateFrequency = 0.0f;           // Actual update rate (Hz)
        bool exceededBudget = false;            // Exceeded time budget

        // Memory
        uint64_t geometryMemoryMB = 0;
        uint64_t audioBufferMemoryMB = 0;
    };

} // namespace parallax

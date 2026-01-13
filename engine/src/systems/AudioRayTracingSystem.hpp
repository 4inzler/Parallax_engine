//// AudioRayTracingSystem.hpp /////////////////////////////////////////////////
//
//  Author:      Parel Development Team (Arosen Systems / Parel LLC)
//  Date:        12/01/2026
//  Description: Audio raytracing systems for realistic spatial audio
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../components/AudioRayTracing.hpp"
#include "../components/Transform.hpp"
#include "../components/VR.hpp"
#include "../ecs/System.hpp"
#include "Timestep.hpp"

namespace parallax {

    /**
     * @brief Initializes audio raytracing backend
     * Sets up Steam Audio, Phonon, or other acoustic simulation library
     */
    class AudioRayTracingInitSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        bool initializeAPI(AudioRayTracingAPI api);
        void shutdownAPI();
        void loadHRTFDatabase();        // Head-Related Transfer Functions for VR

        bool m_initialized = false;
        AudioRayTracingAPI m_currentAPI = AudioRayTracingAPI::STEAM_AUDIO;
    };

    /**
     * @brief Builds acoustic geometry for sound propagation
     * Creates simplified meshes for audio raytracing
     */
    class AcousticGeometryBuildSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void buildAcousticMesh(entt::entity entity, AcousticGeometryComponent& geometry);
        void assignMaterials();
        void optimizeMesh(AcousticGeometryComponent& geometry);

        // Convert visual mesh to acoustic mesh (simplified)
        void decimateMesh(const std::vector<glm::vec3>& vertices,
                          std::vector<glm::vec3>& outVertices);
    };

    /**
     * @brief Updates audio listener (VR headset or camera)
     * Tracks listener position and orientation for HRTF
     */
    class AudioListenerSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void updateListenerTransform();
        void updateHRTF();              // Update HRTF based on head orientation

        entt::entity m_activeListener = entt::null;
    };

    /**
     * @brief Traces audio rays for sound propagation
     * Shoots rays from sound sources to listener, computes paths
     */
    class AudioPropagationSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        // Ray tracing
        void traceAudioRays(entt::entity sourceEntity, AudioRayTracedSourceComponent& source);
        void traceDirectPath(const glm::vec3& sourcePos, const glm::vec3& listenerPos);
        void traceReflections(const glm::vec3& sourcePos, uint32_t maxBounces);

        // Path computation
        struct AudioPath {
            std::vector<glm::vec3> points;
            float totalDistance = 0.0f;
            float attenuation = 1.0f;
            float delay = 0.0f;
            bool isDirect = true;
            std::vector<AcousticMaterialProperties> surfaceMaterials;
        };
        std::vector<AudioPath> computePaths(const glm::vec3& source, const glm::vec3& listener);

        // Energy decay calculation
        float computePathAttenuation(const AudioPath& path, float frequency);
        float computeAirAbsorption(float distance, float frequency);
        float computeSurfaceAbsorption(const AcousticMaterialProperties& material, float frequency);
    };

    /**
     * @brief Computes occlusion and obstruction
     * Determines if sound is blocked or muffled by geometry
     */
    class AudioOcclusionSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void updateOcclusion(entt::entity sourceEntity);
        void checkDirectPath(const glm::vec3& source, const glm::vec3& listener,
                              AudioOcclusionComponent& occlusion);
        void computeDiffraction(const glm::vec3& source, const glm::vec3& listener,
                                 AudioOcclusionComponent& occlusion);

        // Frequency-dependent occlusion
        void computeFrequencyOcclusion(const std::vector<AcousticMaterialProperties>& materials,
                                        AudioOcclusionComponent& occlusion);
    };

    /**
     * @brief Computes reverb from room acoustics
     * Calculates early reflections and late reverb tail
     */
    class AudioReverbSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void computeReverb(entt::entity sourceEntity, AudioReverbComponent& reverb);
        void computeEarlyReflections(const glm::vec3& source, const glm::vec3& listener,
                                      AudioReverbComponent& reverb);
        void computeLateReverb(const glm::vec3& source, const glm::vec3& listener,
                                AudioReverbComponent& reverb);

        // Room estimation
        void estimateRoomCharacteristics(AudioReverbComponent& reverb);
        float computeRT60(const std::vector<AudioReverbComponent::Reflection>& reflections);

        // Reverb zones
        void applyZoneReverb(entt::entity sourceEntity);
        AcousticZoneComponent* findActiveZone(const glm::vec3& position);
    };

    /**
     * @brief Spatial audio rendering system
     * Applies HRTF, panning, and distance attenuation
     */
    class SpatialAudioRenderSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void renderSource(entt::entity sourceEntity, AudioRayTracedSourceComponent& source);
        void applyHRTF(const glm::vec3& sourceDirection, float* audioBuffer, size_t bufferSize);
        void applyDistanceAttenuation(float distance, AudioRayTracedSourceComponent& source);
        void applyDirectivity(const glm::vec3& toListener, AudioRayTracedSourceComponent& source);
        void applyDoppler(const glm::vec3& sourceVelocity, const glm::vec3& listenerVelocity,
                          AudioRayTracedSourceComponent& source);

        // Mix all sources
        void mixAudioSources();

        // VR binaural rendering
        void renderBinaural(AudioRayTracedSourceComponent& source);
    };

    /**
     * @brief Applies acoustic effects (reverb, occlusion filters)
     * Processes audio with computed reverb and occlusion
     */
    class AcousticEffectsSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void applyOcclusionFilter(entt::entity sourceEntity);
        void applyReverbEffect(entt::entity sourceEntity);
        void applyEarlyReflections(entt::entity sourceEntity);

        // DSP filters
        void lowPassFilter(float* buffer, size_t size, float cutoffFreq);
        void convolveWithImpulseResponse(float* buffer, size_t size,
                                          const std::vector<float>& impulseResponse);

        // Frequency-dependent filtering for occlusion
        void applyFrequencyOcclusion(float* buffer, size_t size,
                                       const AudioOcclusionComponent& occlusion);
    };

    /**
     * @brief VR-optimized spatial audio
     * Special handling for VR headset tracking and binaural audio
     */
    class VRAudioSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void updateHMDOrientation();
        void updateControllerAudio();   // Haptic feedback + audio sync
        void renderBinauralAudio();
        void applyHeadTracking();       // Update HRTF based on head rotation

        // Ambisonics for VR (optional, advanced)
        void renderAmbisonics();
        void decodeAmbisonicsToHRTF();
    };

    /**
     * @brief Visualizes audio propagation paths (debug)
     * Renders sound rays and reflections for debugging
     */
    class AudioVisualizationSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void drawPropagationPaths();
        void drawOcclusionRays();
        void drawReflectionPaths();
        void drawReverbZones();

        // Debug UI
        void renderDebugUI();
    };

    /**
     * @brief Performance monitoring for audio raytracing
     * Tracks audio RT performance and adjusts quality
     */
    class AudioRayTracingPerformanceSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        void measureUpdateTime();
        void updateStatistics();
        void adjustQuality();           // Reduce ray count if too slow

        // Target: <2ms per frame for audio (leave time for visual RT)
        const float TARGET_AUDIO_TIME_MS = 2.0f;
    };

} // namespace parallax

#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uViewProjection;
uniform mat4 uMatModel;

out vec3 vFragPos;
out vec2 vTexCoord;
out vec3 vNormal;
out mat3 vTBN;

void main()
{
    vec4 worldPos = uMatModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vTexCoord = aTexCoord;

    // Normal matrix for transforming normals
    mat3 normalMatrix = mat3(transpose(inverse(uMatModel)));
    vNormal = normalize(normalMatrix * aNormal);

    // Construct TBN matrix for normal mapping
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = vNormal;
    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vTBN = mat3(T, B, N);

    gl_Position = uViewProjection * vec4(vFragPos, 1.0);
}

#type fragment
#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define PI 3.14159265359

// Light definitions
struct DirectionalLight {
    vec3 direction;
    vec4 color;
};

struct PointLight {
    vec3 position;
    vec4 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec4 color;
    float cutOff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;
};

in vec3 vFragPos;
in vec2 vTexCoord;
in vec3 vNormal;
in mat3 vTBN;

uniform sampler2D uTexture[32];

uniform vec3 uCamPos;

uniform vec3 uAmbientLight;
uniform DirectionalLight uDirLight;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform int uNumPointLights;
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];
uniform int uNumSpotLights;

struct Material {
    vec4 albedoColor;
    int albedoTexIndex;
    vec4 specularColor;
    int specularTexIndex;
    vec3 emissiveColor;
    int emissiveTexIndex;
    float roughness;
    int roughnessTexIndex;
    float metallic;
    int metallicTexIndex;
    float opacity;
    int opacityTexIndex;
    int normalTexIndex;  // Normal map
    float normalStrength;
};
uniform Material uMaterial;

uniform int uEntityId;

// PBR Functions

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

// Geometry Function (Smith's Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Enhanced Fresnel with roughness
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalcPBRLight(vec3 L, vec3 radiance, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcDirLightPBR(DirectionalLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(-light.direction);
    vec3 radiance = light.color.rgb * light.color.a;

    return CalcPBRLight(L, radiance, N, V, albedo, metallic, roughness, F0);
}

vec3 CalcPointLightPBR(PointLight light, vec3 fragPos, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 radiance = light.color.rgb * light.color.a * attenuation;

    return CalcPBRLight(L, radiance, N, V, albedo, metallic, roughness, F0);
}

vec3 CalcSpotLightPBR(SpotLight light, vec3 fragPos, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    vec3 radiance = light.color.rgb * light.color.a * attenuation * intensity;

    return CalcPBRLight(L, radiance, N, V, albedo, metallic, roughness, F0);
}

void main()
{
    // Sample textures
    vec4 albedoSample = texture(uTexture[uMaterial.albedoTexIndex], vTexCoord);
    if (albedoSample.a < 0.1)
        discard;

    vec3 albedo = pow(uMaterial.albedoColor.rgb * albedoSample.rgb, vec3(2.2)); // Convert to linear space

    float metallic = uMaterial.metallic * texture(uTexture[uMaterial.metallicTexIndex], vTexCoord).r;
    float roughness = uMaterial.roughness * texture(uTexture[uMaterial.roughnessTexIndex], vTexCoord).r;
    roughness = clamp(roughness, 0.04, 1.0); // Prevent division by zero

    // Normal mapping
    vec3 normal;
    if (uMaterial.normalTexIndex > 0) {
        normal = texture(uTexture[uMaterial.normalTexIndex], vTexCoord).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(vTBN * normal);
        // Blend with vertex normal based on strength
        normal = normalize(mix(vNormal, normal, uMaterial.normalStrength));
    } else {
        normal = normalize(vNormal);
    }

    vec3 V = normalize(uCamPos - vFragPos);

    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Lighting calculation
    vec3 Lo = vec3(0.0);

    // Directional light
    Lo += CalcDirLightPBR(uDirLight, normal, V, albedo, metallic, roughness, F0);

    // Point lights
    for (int i = 0; i < uNumPointLights; i++) {
        Lo += CalcPointLightPBR(uPointLights[i], vFragPos, normal, V, albedo, metallic, roughness, F0);
    }

    // Spot lights
    for (int i = 0; i < uNumSpotLights; i++) {
        Lo += CalcSpotLightPBR(uSpotLights[i], vFragPos, normal, V, albedo, metallic, roughness, F0);
    }

    // Ambient lighting (simplified IBL)
    vec3 F = fresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 ambient = (kD * albedo + kS * uMaterial.specularColor.rgb) * uAmbientLight;

    // Emissive
    vec3 emissive = uMaterial.emissiveColor * texture(uTexture[uMaterial.emissiveTexIndex], vTexCoord).rgb;

    vec3 color = ambient + Lo + emissive;

    // HDR tonemapping (Reinhard)
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, albedoSample.a * uMaterial.opacity);
    EntityID = uEntityId;
}

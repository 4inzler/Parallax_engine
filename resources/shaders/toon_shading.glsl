#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uViewProjection;
uniform mat4 uMatModel;

out vec3 vFragPos;
out vec2 vTexCoord;
out vec3 vNormal;

void main()
{
    vec4 worldPos = uMatModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vTexCoord = aTexCoord;
    vNormal = mat3(transpose(inverse(uMatModel))) * aNormal;
    gl_Position = uViewProjection * vec4(vFragPos, 1.0);
}

#type fragment
#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

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
};
uniform Material uMaterial;

uniform int uEntityId;

// Toon shading parameters
const int numBands = 4;
const float specularThreshold = 0.9;
const float rimThreshold = 0.3;
const float rimIntensity = 0.5;
const float outlineThickness = 0.1;

// Quantize a value into discrete bands
float toonify(float value, int bands)
{
    return floor(value * float(bands)) / float(bands);
}

vec3 CalcToonDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);

    // Toon diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float toonDiff = toonify(diff, numBands);

    // Toon specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    float toonSpec = step(specularThreshold, spec) * 1.0;

    vec3 diffuse = light.color.rgb * toonDiff * albedo;
    vec3 specular = light.color.rgb * toonSpec * uMaterial.specularColor.rgb;

    return diffuse + specular;
}

vec3 CalcToonPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Toon diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float toonDiff = toonify(diff, numBands);

    // Toon specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    float toonSpec = step(specularThreshold, spec) * 1.0;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float toonAttenuation = toonify(attenuation, numBands);

    vec3 diffuse = light.color.rgb * toonDiff * albedo * toonAttenuation;
    vec3 specular = light.color.rgb * toonSpec * uMaterial.specularColor.rgb * toonAttenuation;

    return diffuse + specular;
}

vec3 CalcToonSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Toon diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float toonDiff = toonify(diff, numBands);

    // Toon specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    float toonSpec = step(specularThreshold, spec) * 1.0;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    float toonIntensity = toonify(intensity * attenuation, numBands);

    vec3 diffuse = light.color.rgb * toonDiff * albedo * toonIntensity;
    vec3 specular = light.color.rgb * toonSpec * uMaterial.specularColor.rgb * toonIntensity;

    return diffuse + specular;
}

void main()
{
    vec4 albedoSample = texture(uTexture[uMaterial.albedoTexIndex], vTexCoord);
    if (albedoSample.a < 0.1)
        discard;

    vec3 albedo = uMaterial.albedoColor.rgb * albedoSample.rgb;
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(uCamPos - vFragPos);

    // Ambient (toonified)
    vec3 ambient = uAmbientLight * albedo;
    ambient = toonify(length(ambient), numBands) * normalize(ambient);

    // Lighting
    vec3 result = ambient;
    result += CalcToonDirLight(uDirLight, norm, viewDir, albedo);

    for (int i = 0; i < uNumPointLights; i++) {
        result += CalcToonPointLight(uPointLights[i], vFragPos, norm, viewDir, albedo);
    }

    for (int i = 0; i < uNumSpotLights; i++) {
        result += CalcToonSpotLight(uSpotLights[i], vFragPos, norm, viewDir, albedo);
    }

    // Rim lighting for toon effect
    float rimDot = 1.0 - max(dot(viewDir, norm), 0.0);
    float rimAmount = step(1.0 - rimThreshold, rimDot);
    vec3 rim = vec3(rimAmount * rimIntensity);

    result += rim;

    // Emissive
    result += uMaterial.emissiveColor * texture(uTexture[uMaterial.emissiveTexIndex], vTexCoord).rgb;

    FragColor = vec4(result, albedoSample.a * uMaterial.opacity);
    EntityID = uEntityId;
}

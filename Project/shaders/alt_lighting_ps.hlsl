
Texture2D diffuse : register(t0);
SamplerState samState : register(s0);

struct OutputVertex
{
    float4 position : SV_POSITION; // System value
    float3 worldPosition : WORLDPOSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float3 cameraposition : CAMPOSITION;
};

struct Light
{
    float4 position, lightDirection;
    float4 ambient, diffuse, specular;
    float _constant, _linear, _exponent;
    unsigned int lightType;
    float cosineInnerCone, cosineOuterCone;
    float lightRadius;
    int lightOn;
};

cbuffer Lights
{
    Light light[3];
};

float4 computeDirectionalLightColor(Light dlight, OutputVertex input)
{
    // Standard color 
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Set material factors hard coded
    float ambientStrength = 0.1;
    int specularShininess = 32;
    
    // Ambient color
    float3 ambientColor = ambientStrength * dlight.ambient.rgb;
    
    // Diffuse color
    float3 normal = normalize(input.normal);
    //float3 lightDirection = normalize(dlight.position.xyz - input.posxyz);
    float3 lightDirection = normalize(-dlight.lightDirection.xyz);
    float diffuseFactor = saturate(dot(normal, lightDirection));
    float3 diffuseColor = saturate(diffuseFactor * dlight.diffuse.rgb);
    
    // Specular color
    float3 cameraDirection = normalize(input.cameraposition - input.worldPosition);
    float3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularIntensity = pow(saturate(dot(cameraDirection, reflectionDirection)), specularShininess);
    float3 specularColor = saturate(specularIntensity * dlight.specular.rgb);
    
    // Combine colors
    color = float4(saturate((ambientColor + diffuseColor + specularColor) * color.rgb), 1);
    
    //return color;
    //return float4(input.normal, 1);
    return float4(specularColor, 1);
    //return float4(saturate(specularColor + ambientColor), 1);
    //return diffuse.Sample(samState, input.tex);
}

float4 computePointLightColor(Light plight, OutputVertex input)
{
    // Set material factors hard coded
    float ambientStrength = 0.1;
    int specularShininess = 32;
    
    // Standard color 
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Ambient light
    float3 ambientColor = saturate(ambientStrength * plight.ambient.rgb);
   
    // Diffuse color
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(plight.position.xyz - input.worldPosition);
    float diffuseFactor = saturate(dot(normal, lightDirection));
    float3 diffuseColor = saturate(diffuseFactor * plight.diffuse);
    
    // Specular color
    float3 cameraDirection = normalize(input.cameraposition - input.worldPosition);
    float3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularIntensity = pow(saturate(dot(normal, reflectionDirection)), specularShininess);
    float3 specularColor = saturate(specularIntensity * plight.specular.rgb);
    
    // Attenuation
    float distance = length(plight.position.xyz - input.worldPosition);
    float attenuation = 1.0 / (plight._constant + plight._linear * distance + plight._exponent * pow(distance, 2));
    
    diffuseColor = saturate(attenuation * diffuseColor);
    specularColor = saturate(attenuation * specularColor);
    
    // Combine colors
    color = float4(saturate((ambientColor + diffuseColor + specularColor) * color.rgb), 1);
    //color = float4(saturate(specularColor), 1);
    return color;
}

float4 computeSpotLightColor(Light slight, OutputVertex input)
{
    // Note: This bitch doesn't have ambient light - It be like that sometimes
    
    // Set material factors hard coded
    int specularShininess = 32;
    float diffuseIntensity = 0.1;
    
    // Standard color 
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Light Directions and what not
    float3 pixelToLightDirection = normalize(slight.position.xyz - input.worldPosition);
    float3 spotlightDirection = normalize(slight.lightDirection.xyz);
    
    // Cone crap 
    float coneDirection = dot(-pixelToLightDirection, spotlightDirection);
    float spotIntensity = smoothstep(slight.cosineOuterCone, slight.cosineInnerCone, coneDirection);
    
    // Diffuse color
    float3 normal = normalize(input.normal);
    float3 diffuseFactor = saturate(dot(normal, pixelToLightDirection));
    float3 diffuseColor = saturate(diffuseFactor * slight.diffuse.rgb);
    
    // Specular color
    float3 cameraDirection = normalize(input.cameraposition - input.worldPosition);
    float3 reflectionDirection = normalize(reflect(-pixelToLightDirection, normal));
    float specularIntensity = pow(saturate(dot(normal, reflectionDirection)), specularShininess);
    float3 specularColor = saturate(specularIntensity * slight.specular.rgb);
    
    // Attenuation
    float distance = length(slight.position.xyz - input.worldPosition);
    float attenuation = 1.0 / (slight._constant + slight._linear * distance + slight._exponent * pow(distance, 2));
    
    // Adjust for attenuation / spot intensity
    diffuseColor = saturate(diffuseColor * spotIntensity * attenuation);
    //specularColor = saturate(specularColor * spotIntensity * attenuation);
    specularColor = saturate(specularColor * spotIntensity);
    
    // Mix colors
    color = saturate(float4((diffuseColor + specularColor) * color.rgb, 1));
    color = saturate(float4(specularColor, 1));
    return color;
}

float4 main(OutputVertex input) : SV_TARGET
{
    // Loop through all light sources and get the light color and shit
    return computeDirectionalLightColor(light[0], input);
    //return computePointLightColor(light[1], input);
    //return computeSpotLightColor(light[2], input);
    //return float4(1, 0, 0, 1);

}
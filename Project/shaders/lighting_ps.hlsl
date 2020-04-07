
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
    unsigned int lightType;
    float ambientIntensity, diffuseIntensity, specularIntensity;
    float cosineInnerCone, cosineOuterCone;
    float lightRadius;
    int lightOn;
};

cbuffer Lights
{
    Light light[16];
};

float4 computeDirectionalLight(Light dlight, OutputVertex input)
{
    // Hard coded values
    float specularShininess = 100;
    
    // Texture color
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Ambient color
    float3 ambientColor = saturate(color.rgb * dlight.ambientIntensity);
    
    // Diffuse color
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(dlight.lightDirection.xyz);
    float diffuseFactor = saturate(dot(normal, -lightDirection));
    float3 diffuseColor = saturate(diffuseFactor * dlight.diffuseIntensity * dlight.diffuse.rgb);
    
    // Specular color
    float3 viewDirection = normalize(input.cameraposition - input.worldPosition);
    float3 halfVector = normalize(-lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normal, halfVector), 0), specularShininess);
    float3 specularColor = saturate(specularFactor * dlight.specularIntensity * dlight.specular.rgb);
    
    // Combine phong components
    color = saturate(float4((ambientColor + diffuseColor + specularColor) * color.rgb, color.a));
    //color = saturate(float4(specularColor + ambientColor, 1));
    //color = float4(specularColor, 1);
    
    return color; 
}

float4 computePointLight(Light plight, OutputVertex input)
{
    // Hard coded values
    float specularShininess = 50;
    
    // Texture color
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Ambient color
    float3 ambientColor = saturate(color.rgb * plight.ambientIntensity);
    
    // Diffuse color
    float3 normal = normalize(input.normal);
    float3 lightDirection = (plight.position.xyz - input.worldPosition);
    float diffuseFactor = saturate(dot(normal, lightDirection));
    float3 diffuseColor = saturate(diffuseFactor * plight.diffuseIntensity * plight.diffuse);
    
    // Specular color
    // The view direction needs to be added to the positive light direction here
    float3 viewDirection = normalize(input.cameraposition - input.worldPosition);
    float3 halfVector = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normal, halfVector), 0), specularShininess);
    float3 specularColor = saturate(specularFactor * plight.specularIntensity * plight.specular.rgb);
    
    // Attenuation
    float distance = length(plight.position.xyz - input.worldPosition);
    float attenuation = 1.0 - saturate(distance/ plight.lightRadius);
    attenuation *= attenuation;
    
    // Apply attenuation
    diffuseColor = saturate(diffuseColor * attenuation);
    specularColor = saturate(specularColor * attenuation);
    
    // Combine phong components
    color = saturate(float4((ambientColor + diffuseColor + specularColor) * color.rgb, color.a));
    //color = saturate(float4(specularColor + ambientColor, 1));
    return color;
}

float4 computeSpotLight(Light slight, OutputVertex input)
{
    // Spotlights don't have an ambient component
    
    // Hard coded values
    float specularShininess = 50;
    
    // Texture color
    float4 color = diffuse.Sample(samState, input.tex);
    
    // Diffuse color
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(slight.position.xyz - input.worldPosition);
    
    float diffuseFactor = saturate(dot(lightDirection, normal));
    float3 diffuseColor = saturate(diffuseFactor * slight.diffuseIntensity * slight.diffuse.rgb);
    
    // Specular color
    // The view direction needs to be added to the positive light direction here
    float3 viewDirection = normalize(input.cameraposition - input.worldPosition);
    float3 halfVector = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normal, halfVector), 0), specularShininess);
    float3 specularColor = saturate(specularFactor * slight.specularIntensity * slight.specular.rgb);
    
    // Cone attenuation and spotlight clamping
    float surfaceratio = saturate(dot(-lightDirection, slight.lightDirection.xyz));
    float spotFactor = (surfaceratio > slight.cosineOuterCone) ? 1 : 0;
    
    float distance = length(slight.position.xyz - input.worldPosition);
    float attenuation = 1.0 - saturate((slight.cosineInnerCone - surfaceratio) / (slight.cosineInnerCone - slight.cosineOuterCone));
    
    // Color clamping and attenuation
    diffuseColor = saturate(diffuseColor * spotFactor * attenuation);
    specularColor = saturate(specularColor * spotFactor * attenuation);
    
    // Combine phone components
    //diffuseColor *= .2;
    color = saturate(float4((diffuseColor + specularColor) * color.rgb, color.a));
    
    return color;
}

float4 main(OutputVertex input) : SV_TARGET
{   
    float4 color = float4(0, 0, 0, 0);
    
    float4 tempcolor;
    for (int i = 0; i < 3; i++)
    {
        //if (light[i].lightType == 0)
        //{
        //    tempcolor = computeDirectionalLight(light[i], input);
        //}
        switch (light[i].lightType)
        {
            case 0:
                tempcolor = computeDirectionalLight(light[i], input);
                break;
            case 1:
                tempcolor = computePointLight(light[i], input);
                break;
            case 2:
                tempcolor = computeSpotLight(light[i], input);
                break;
            default:
                tempcolor = float4(0, 0, 0, 0);
                break;
        }
        
        color = saturate(color + tempcolor);
        tempcolor = float4(0, 0, 0, 0);
    }

    return color;
}
/* Note:
 * Normalizing the normal can be removed if the normal is normalized in the vertex shader, first
 */
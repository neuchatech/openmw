uniform float exponentialFogDensity;
uniform bool heightFogEnabled;
uniform float heightFogDensity;
uniform float heightFogFalloff;
uniform float heightFogOffset;
uniform vec3 cameraPos;
uniform mat4 invViewMatrix;
uniform float skyBlendingStart;
uniform vec2 screenRes;
uniform float near;
uniform float far;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

vec4 applyFogAtDist(vec4 color, float euclideanDist, float linearDist, float farVal)
{
#if @radialFog
    float dist = euclideanDist;
#else
    float dist = abs(linearDist);
#endif

    float fogValue = 0.0;

#if @exponentialFog
    // Density-based exponential fog with slider offset
    fogValue = 1.0 - exp(-max(0.0, dist - fogStart) * exponentialFogDensity * 0.0001);
#else
    float fogScale = 1.0 / max(0.001, fogEnd - fogStart);
    fogValue = clamp((dist - fogStart) * fogScale, 0.0, 1.0);
#endif

    vec3 effectiveFogColor = fogColor.xyz;
#if @skyBlending
    effectiveFogColor = sampleSkyColor(gl_FragCoord.xy / max(vec2(1.0), screenRes));
#endif

#ifdef ADDITIVE_BLENDING
    color.xyz *= 1.0 - fogValue;
#else
    color.xyz = mix(color.xyz, effectiveFogColor, fogValue);
#endif

#if @skyBlending
    float skyFadeRange = max(0.001, farVal - skyBlendingStart);
    float fadeValue = clamp((farVal - dist) / skyFadeRange, 0.0, 1.0);
    fadeValue *= fadeValue;
#ifdef ADDITIVE_BLENDING
    color.xyz *= fadeValue;
#else
    color.xyz = mix(sampleSkyColor(gl_FragCoord.xy / max(vec2(1.0), screenRes)), color.xyz, fadeValue);
#endif
#endif

    return color;
}

vec4 applyFogAtPos(vec4 color, vec3 pos, float farVal)
{
    float euclideanDist = length(pos);
    float linearDist = pos.z;

#if @radialFog
    float dist = euclideanDist;
#else
    float dist = abs(linearDist);
#endif

    float fogValue = 0.0;

#if @exponentialFog
    // Density-based exponential fog with slider offset
    fogValue = 1.0 - exp(-max(0.0, dist - fogStart) * exponentialFogDensity * 0.0001);
#else
    float fogScale = 1.0 / max(0.001, fogEnd - fogStart);
    fogValue = clamp((dist - fogStart) * fogScale, 0.0, 1.0);
#endif

    if (heightFogEnabled)
    {
        vec3 worldViewDir = (invViewMatrix * vec4(normalize(pos), 0.0)).xyz;
        float h0 = cameraPos.z - heightFogOffset;
        float vz = worldViewDir.z;

        float falloff = heightFogFalloff * 0.001;
        float density = heightFogDensity * 0.0001;

        float heightFogAmount = 0.0;
        float effectiveHeightDist = max(0.0, dist - fogStart);
        if (abs(vz) > 0.0001)
        {
            float falloffVz = falloff * vz;
            heightFogAmount = (density * exp(-falloff * h0) / falloffVz) * (1.0 - exp(-falloffVz * effectiveHeightDist));
        }
        else
        {
            heightFogAmount = density * exp(-falloff * h0) * effectiveHeightDist;
        }
        fogValue = max(fogValue, clamp(heightFogAmount, 0.0, 1.0));
    }

    vec3 effectiveFogColor = fogColor.xyz;
#if @skyBlending
    effectiveFogColor = sampleSkyColor(gl_FragCoord.xy / max(vec2(1.0), screenRes));
#endif

#ifdef ADDITIVE_BLENDING
    color.xyz *= 1.0 - fogValue;
#else
    color.xyz = mix(color.xyz, effectiveFogColor, fogValue);
#endif

#if @skyBlending
    float skyFadeRange = max(0.001, farVal - skyBlendingStart);
    float fadeValue = clamp((farVal - dist) / skyFadeRange, 0.0, 1.0);
    fadeValue *= fadeValue;
#ifdef ADDITIVE_BLENDING
    color.xyz *= fadeValue;
#else
    color.xyz = mix(sampleSkyColor(gl_FragCoord.xy / max(vec2(1.0), screenRes)), color.xyz, fadeValue);
#endif
#endif

    return color;
}


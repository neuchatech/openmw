#ifndef OPENMW_COMPONENTS_SETTINGS_CATEGORIES_SHADOWS_H
#define OPENMW_COMPONENTS_SETTINGS_CATEGORIES_SHADOWS_H

#include <components/settings/sanitizerimpl.hpp>
#include <components/settings/settingvalue.hpp>

#include <osg/Math>
#include <osg/Vec2f>
#include <osg/Vec3f>

#include <cstdint>
#include <string>
#include <string_view>

namespace Settings
{
    struct ShadowsCategory : WithIndex
    {
        using WithIndex::WithIndex;

        SettingValue<bool> mEnableShadows{ mIndex, "Shadows", "enable shadows" };
        SettingValue<std::string> mShadowMappingMethod{ mIndex, "Shadows", "shadow mapping method",
            makeEnumSanitizerString({ "legacy vdsm", "stable csm" }) };
        SettingValue<int> mNumberOfShadowMaps{ mIndex, "Shadows", "number of shadow maps",
            makeClampSanitizerInt(1, 8) };
        SettingValue<float> mMaximumShadowMapDistance{ mIndex, "Shadows", "maximum shadow map distance" };
        SettingValue<float> mShadowFadeStart{ mIndex, "Shadows", "shadow fade start", makeClampSanitizerFloat(0, 1) };
        SettingValue<float> mSplitPointUniformLogarithmicRatio{ mIndex, "Shadows",
            "split point uniform logarithmic ratio", makeClampSanitizerFloat(0, 1) };
        SettingValue<float> mSplitPointBias{ mIndex, "Shadows", "split point bias" };
        SettingValue<bool> mEnableDebugHud{ mIndex, "Shadows", "enable debug hud" };
        SettingValue<bool> mEnableDebugOverlay{ mIndex, "Shadows", "enable debug overlay" };
        SettingValue<std::string> mComputeSceneBounds{ mIndex, "Shadows", "compute scene bounds",
            makeEnumSanitizerString({ "primitives", "bounds", "none" }) };
        SettingValue<int> mShadowMapResolution{ mIndex, "Shadows", "shadow map resolution" };
        SettingValue<float> mMinimumLispsmNearFarRatio{ mIndex, "Shadows", "minimum lispsm near far ratio",
            makeMaxStrictSanitizerFloat(0) };
        SettingValue<float> mPolygonOffsetFactor{ mIndex, "Shadows", "polygon offset factor" };
        SettingValue<float> mPolygonOffsetUnits{ mIndex, "Shadows", "polygon offset units" };
        SettingValue<float> mNormalOffsetDistance{ mIndex, "Shadows", "normal offset distance" };
        SettingValue<bool> mUseFrontFaceCulling{ mIndex, "Shadows", "use front face culling" };
        SettingValue<int> mStableCsmCascades{ mIndex, "Shadows", "stable csm cascades",
            makeClampSanitizerInt(1, 8) };
        SettingValue<int> mStableCsmResolution{ mIndex, "Shadows", "stable csm resolution",
            makeClampSanitizerInt(1, 32768) };
        SettingValue<float> mStableCsmDistance{ mIndex, "Shadows", "stable csm distance",
            makeMaxStrictSanitizerFloat(0) };
        SettingValue<float> mStableCsmSplitLambda{ mIndex, "Shadows", "stable csm split lambda",
            makeClampSanitizerFloat(0, 1) };
        SettingValue<bool> mStableCsmTexelSnapping{ mIndex, "Shadows", "stable csm texel snapping" };
        SettingValue<int> mStableCsmUpdateIntervalMilliseconds{ mIndex, "Shadows",
            "stable csm update interval ms", makeClampSanitizerInt(16, 1000) };
        SettingValue<float> mStableCsmSunUpdateAngleThreshold{ mIndex, "Shadows",
            "stable csm sun update angle threshold", makeMaxSanitizerFloat(0) };
        SettingValue<bool> mActorShadows{ mIndex, "Shadows", "actor shadows" };
        SettingValue<bool> mPlayerShadows{ mIndex, "Shadows", "player shadows" };
        SettingValue<bool> mTerrainShadows{ mIndex, "Shadows", "terrain shadows" };
        SettingValue<bool> mObjectShadows{ mIndex, "Shadows", "object shadows" };
        SettingValue<bool> mStaticObjectShadows{ mIndex, "Shadows", "static object shadows" };
        SettingValue<bool> mSmallObjectShadows{ mIndex, "Shadows", "small object shadows" };
        SettingValue<float> mShadowCasterMinRadius{ mIndex, "Shadows", "shadow caster min radius",
            makeMaxSanitizerFloat(0) };
        SettingValue<float> mShadowCasterMaxDistance{ mIndex, "Shadows", "shadow caster max distance",
            makeMaxSanitizerFloat(0) };
        SettingValue<bool> mShadowSmallFeatureCulling{ mIndex, "Shadows", "shadow small feature culling" };
        SettingValue<float> mShadowSmallFeatureCullingPixelSize{ mIndex, "Shadows",
            "shadow small feature culling pixel size", makeMaxSanitizerFloat(0) };
        SettingValue<bool> mEnableShadowCascadeStats{ mIndex, "Shadows", "enable shadow cascade stats" };
        SettingValue<bool> mEnableAmbientOcclusion{ mIndex, "Shadows", "enable ambient occlusion" };
        SettingValue<std::string> mAoMethod{ mIndex, "Shadows", "ao method",
            makeEnumSanitizerString({ "contact", "none" }) };
        SettingValue<float> mAoRadius{ mIndex, "Shadows", "ao radius", makeMaxSanitizerFloat(0) };
        SettingValue<float> mAoStrength{ mIndex, "Shadows", "ao strength", makeMaxSanitizerFloat(0) };
        SettingValue<int> mAoSampleCount{ mIndex, "Shadows", "ao sample count",
            makeClampSanitizerInt(1, 32) };
        SettingValue<float> mAoFadeDistance{ mIndex, "Shadows", "ao fade distance", makeMaxSanitizerFloat(0) };
        SettingValue<bool> mEnableIndoorShadows{ mIndex, "Shadows", "enable indoor shadows" };
    };
}

#endif

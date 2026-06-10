#include "stableshadowtechnique.hpp"

#include <osg/FrameStamp>
#include <osg/Timer>
#include <osg/Uniform>
#include <osgShadow/ShadowedScene>

#include <components/debug/debuglog.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace
{
    constexpr double sMinSplitNear = 1.0;

    class StableCameraCullCallback : public osg::NodeCallback
    {
    public:
        explicit StableCameraCullCallback(SceneUtil::StableShadowTechnique* technique)
            : mTechnique(technique)
        {
        }

        void operator()(osg::Node* node, osg::NodeVisitor* nv) override
        {
            osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(nv);

            cv->pushStateSet(mTechnique->getOrCreateShadowsBinStateSet());
            if (mTechnique->getShadowedScene())
                mTechnique->getShadowedScene()->osg::Group::traverse(*nv);
            cv->popStateSet();
        }

    private:
        SceneUtil::StableShadowTechnique* mTechnique;
    };

    double computeSplitDistance(double nearDistance, double farDistance, unsigned int split, unsigned int splitCount,
        double splitLambda)
    {
        if (split == 0)
            return nearDistance;
        if (split == splitCount)
            return farDistance;

        const double ratio = static_cast<double>(split) / static_cast<double>(splitCount);
        const double positiveNear = std::max(nearDistance, sMinSplitNear);
        const double positiveFar = std::max(farDistance, positiveNear + sMinSplitNear);
        const double logarithmic = positiveNear * std::pow(positiveFar / positiveNear, ratio);
        const double uniform = nearDistance + (farDistance - nearDistance) * ratio;
        return splitLambda * logarithmic + (1.0 - splitLambda) * uniform;
    }

    osg::Vec3d lerp(const osg::Vec3d& a, const osg::Vec3d& b, double t)
    {
        return a + (b - a) * t;
    }

    std::array<osg::Vec3d, 8> getCascadeCorners(
        const SceneUtil::MWShadowTechnique::Frustum& frustum, double nearDistance, double farDistance)
    {
        const double frustumDepth = std::max((frustum.centerFarPlane - frustum.centerNearPlane).length(), 1.0);
        const double frustumNear = (frustum.centerNearPlane - frustum.eye).length();
        const double nearT = std::clamp((nearDistance - frustumNear) / frustumDepth, 0.0, 1.0);
        const double farT = std::clamp((farDistance - frustumNear) / frustumDepth, 0.0, 1.0);

        return {
            lerp(frustum.corners[0], frustum.corners[3], nearT),
            lerp(frustum.corners[1], frustum.corners[2], nearT),
            lerp(frustum.corners[4], frustum.corners[7], nearT),
            lerp(frustum.corners[5], frustum.corners[6], nearT),
            lerp(frustum.corners[0], frustum.corners[3], farT),
            lerp(frustum.corners[1], frustum.corners[2], farT),
            lerp(frustum.corners[4], frustum.corners[7], farT),
            lerp(frustum.corners[5], frustum.corners[6], farT),
        };
    }

    osg::Vec3d average(const std::array<osg::Vec3d, 8>& points)
    {
        osg::Vec3d result;
        for (const osg::Vec3d& point : points)
            result += point;
        return result / static_cast<double>(points.size());
    }

    double radius(const std::array<osg::Vec3d, 8>& points, const osg::Vec3d& center)
    {
        double result = 1.0;
        for (const osg::Vec3d& point : points)
            result = std::max(result, (point - center).length());
        return result;
    }

    bool makeLightBasis(const osg::Vec3d& lightDir, osg::Vec3d& side, osg::Vec3d& up)
    {
        osg::Vec3d dir = lightDir;
        if (dir.length2() == 0.0)
            return false;
        dir.normalize();

        osg::Vec3d upCandidate(0.0, 0.0, 1.0);
        if (std::abs(dir * upCandidate) > 0.95)
            upCandidate.set(0.0, 1.0, 0.0);

        side = upCandidate ^ dir;
        if (side.length2() == 0.0)
            return false;
        side.normalize();

        up = dir ^ side;
        if (up.length2() == 0.0)
            return false;
        up.normalize();
        return true;
    }

    osg::Vec3d snapCenterToTexels(
        osg::Vec3d center, const osg::Vec3d& side, const osg::Vec3d& up, double diameter, int resolution)
    {
        if (resolution <= 0 || diameter <= 0.0)
            return center;

        const double worldUnitsPerTexel = diameter / static_cast<double>(resolution);
        const double lightX = center * side;
        const double lightY = center * up;
        const double snappedX = std::floor(lightX / worldUnitsPerTexel) * worldUnitsPerTexel;
        const double snappedY = std::floor(lightY / worldUnitsPerTexel) * worldUnitsPerTexel;
        center += side * (snappedX - lightX);
        center += up * (snappedY - lightY);
        return center;
    }

    double angleDegrees(osg::Vec3d a, osg::Vec3d b)
    {
        if (a.length2() == 0.0 || b.length2() == 0.0)
            return 180.0;

        a.normalize();
        b.normalize();
        return std::acos(std::clamp(a * b, -1.0, 1.0)) * 180.0 / osg::PI;
    }

    double centerRefreshThreshold(double diameter, int resolution)
    {
        if (diameter <= 0.0 || resolution <= 0)
            return 0.0;
        return diameter / static_cast<double>(resolution) * 0.5;
    }

    bool differs(double lhs, double rhs)
    {
        return std::abs(lhs - rhs) > std::max(0.001, std::max(std::abs(lhs), std::abs(rhs)) * 0.0001);
    }

    void setFloatUniform(SceneUtil::MWShadowTechnique::Uniforms& uniforms, const std::string& name, float value)
    {
        osg::ref_ptr<osg::Uniform> target;
        for (const osg::ref_ptr<osg::Uniform>& uniform : uniforms)
        {
            if (uniform->getName() == name)
            {
                target = uniform;
                break;
            }
        }

        if (!target)
        {
            target = new osg::Uniform(name.c_str(), value);
            uniforms.push_back(target);
        }
        else
            target->set(value);
    }
}

namespace SceneUtil
{
    class StableShadowTechnique::StableViewDependentData : public MWShadowTechnique::ViewDependentData
    {
    public:
        struct Cascade
        {
            osg::Matrixd mViewMatrix;
            osg::Matrixd mProjectionMatrix;
            osg::Vec3d mCenter;
            double mDiameter = 0.0;
            double mNear = 0.0;
            double mFar = 0.0;
            bool mValid = false;
        };

        explicit StableViewDependentData(MWShadowTechnique* technique)
            : ViewDependentData(technique)
        {
        }

        std::vector<Cascade> mCascades;
        osg::Vec3d mLightDir;
        bool mLightDirValid = false;
        bool mRenderedOnce = false;
        double mLastRefreshTime = 0.0;
        unsigned int mNextCascadeRefreshIndex = 0;
        unsigned int mLastCasterMask = 0;
        unsigned int mLastCascadeCount = 0;
        int mLastResolution = 0;
        float mLastDistance = 0.0f;
        float mLastSplitLambda = 0.0f;
        bool mLastTexelSnapping = false;
    };

    StableShadowTechnique::StableShadowTechnique()
        : MWShadowTechnique()
    {
    }

    StableShadowTechnique::StableShadowTechnique(const StableShadowTechnique& rhs, const osg::CopyOp& copyop)
        : MWShadowTechnique(rhs, copyop)
        , mSettings(rhs.mSettings)
    {
    }

    StableShadowTechnique::~StableShadowTechnique() = default;

    void StableShadowTechnique::setStableSettings(const Settings& settings)
    {
        mSettings = settings;
    }

    MWShadowTechnique::ViewDependentData* StableShadowTechnique::createViewDependentData(osgUtil::CullVisitor* /*cv*/)
    {
        return new StableViewDependentData(this);
    }

    void StableShadowTechnique::assignShadowStateSettings(
        osgUtil::CullVisitor& cv, osg::Camera* camera, unsigned int sm_i, Uniforms& uniforms)
    {
        MWShadowTechnique::assignShadowStateSettings(cv, camera, sm_i, uniforms);

        StableViewDependentData* stableVdd = dynamic_cast<StableViewDependentData*>(getViewDependentData(&cv));
        if (!stableVdd || sm_i >= stableVdd->mCascades.size())
            return;

        const StableViewDependentData::Cascade& cascade = stableVdd->mCascades[sm_i];
        setFloatUniform(uniforms, "shadowCascadeNear" + std::to_string(sm_i), static_cast<float>(cascade.mNear));
        setFloatUniform(uniforms, "shadowCascadeFar" + std::to_string(sm_i), static_cast<float>(cascade.mFar));
    }

    void StableShadowTechnique::cull(osgUtil::CullVisitor& cv)
    {
        if (!_enableShadows)
        {
            MWShadowTechnique::cull(cv);
            return;
        }

        if (!_shadowCastingStateSet)
        {
            _shadowedScene->osg::Group::traverse(cv);
            return;
        }

        ViewDependentData* vdd = getViewDependentData(&cv);
        if (!vdd)
        {
            _shadowedScene->osg::Group::traverse(cv);
            return;
        }

        StableViewDependentData* stableVdd = dynamic_cast<StableViewDependentData*>(vdd);
        if (!stableVdd)
        {
            _shadowedScene->osg::Group::traverse(cv);
            return;
        }

        Uniforms& vddUniforms = vdd->_uniforms[cv.getTraversalNumber() % 2];
        osgShadow::ShadowSettings* settings = getShadowedScene()->getShadowSettings();

        const osg::CullSettings::ComputeNearFarMode cachedNearFarMode = cv.getComputeNearFarMode();
        osg::RefMatrix& viewProjectionMatrix = *cv.getProjectionMatrix();
        const bool orthographicViewFrustum = viewProjectionMatrix(0, 3) == 0.0 && viewProjectionMatrix(1, 3) == 0.0
            && viewProjectionMatrix(2, 3) == 0.0;

        double minZNear = 0.0;
        double maxZFar = mSettings.mDistance;
        if (cachedNearFarMode == osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
        {
            double left, right, top, bottom;
            if (orthographicViewFrustum)
                viewProjectionMatrix.getOrtho(left, right, bottom, top, minZNear, maxZFar);
            else
                viewProjectionMatrix.getFrustum(left, right, bottom, top, minZNear, maxZFar);
        }

        if (settings->getComputeNearFarModeOverride() != osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
            cv.setComputeNearFarMode(settings->getComputeNearFarModeOverride());

        auto* shadowReceiverStateSet = vdd->getStateSet(cv.getTraversalNumber());
        shadowReceiverStateSet->clear();
        cv.pushStateSet(shadowReceiverStateSet);
        cullShadowReceivingScene(&cv);
        cv.popStateSet();

        if (cv.getComputeNearFarMode() != osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR)
            cv.computeNearPlane();

        maxZFar = std::min<double>(mSettings.mDistance, maxZFar);
        if (minZNear > maxZFar)
            minZNear = maxZFar * settings->getMinimumShadowMapNearFarRatio();
        cv.setNearFarRatio(minZNear / maxZFar);

        Frustum frustum(&cv, minZNear, maxZFar);
        if (_customFrustumCallback)
        {
            osgUtil::CullVisitor* sharedFrustumHint = nullptr;
            _customClipSpace.init();
            _customFrustumCallback->operator()(cv, _customClipSpace, sharedFrustumHint);
            frustum.setCustomClipSpace(_customClipSpace);
            if (sharedFrustumHint)
            {
                std::lock_guard<std::mutex> lock(_viewDependentDataMapMutex);
                auto it = _viewDependentDataMap.find(sharedFrustumHint);
                if (it != _viewDependentDataMap.end())
                {
                    copyShadowMap(cv, vdd, it->second);
                    cv.setComputeNearFarMode(cachedNearFarMode);
                    return;
                }
            }
        }

        frustum.init();
        cv.setComputeNearFarMode(cachedNearFarMode);

        selectActiveLights(&cv, vdd);

        unsigned int textureUnit = settings->getBaseShadowTextureUnit();
        unsigned int numValidShadows = 0;

        ShadowDataList& sdl = vdd->getShadowDataList();
        ShadowDataList previousSdl;
        previousSdl.swap(sdl);

        const unsigned int numShadowMapsPerLight = settings->getNumShadowMapsPerLight();
        const unsigned int cascadeCount = std::min<unsigned int>(
            numShadowMapsPerLight, std::max(1, mSettings.mCascadeCount));
        stableVdd->mCascades.resize(cascadeCount);

        const unsigned int casterMask = settings->getCastsShadowTraversalMask();
        const bool configChanged = !stableVdd->mRenderedOnce || stableVdd->mLastCasterMask != casterMask
            || stableVdd->mLastCascadeCount != cascadeCount || stableVdd->mLastResolution != mSettings.mResolution
            || stableVdd->mLastDistance != mSettings.mDistance
            || stableVdd->mLastSplitLambda != mSettings.mSplitLambda
            || stableVdd->mLastTexelSnapping != mSettings.mTexelSnapping;
        const osg::FrameStamp* frameStamp = cv.getFrameStamp();
        const double currentTime = frameStamp ? frameStamp->getReferenceTime()
                                              : static_cast<double>(cv.getTraversalNumber()) / 60.0;
        const double updateIntervalSeconds = std::max(0.0, mSettings.mUpdateIntervalMilliseconds / 1000.0);
        const bool intervalElapsed = !stableVdd->mRenderedOnce
            || currentTime - stableVdd->mLastRefreshTime >= updateIntervalSeconds;
        bool refreshedAnyCascade = false;

        LightDataList& lights = vdd->getLightDataList();
        for (LightDataList::iterator lightIt = lights.begin(); lightIt != lights.end(); ++lightIt)
        {
            LightData& light = **lightIt;
            if (!light.directionalLight)
                continue;

            osg::Vec3d lightDir = light.lightDir;
            if (lightDir.length2() == 0.0)
                continue;
            lightDir.normalize();

            const double sunAngleChange
                = stableVdd->mLightDirValid ? angleDegrees(lightDir, stableVdd->mLightDir) : 180.0;
            const bool sunMoved = !stableVdd->mLightDirValid || sunAngleChange > 0.0001;
            const bool sunJumped = !stableVdd->mLightDirValid || sunAngleChange > mSettings.mSunUpdateAngleThreshold;

            const bool refreshSunThisFrame = configChanged || (intervalElapsed && sunMoved);
            const osg::Vec3d effectiveLightDir
                = refreshSunThisFrame || !stableVdd->mLightDirValid ? lightDir : stableVdd->mLightDir;

            osg::Vec3d lightSide;
            osg::Vec3d lightUp;
            if (!makeLightBasis(effectiveLightDir, lightSide, lightUp))
                continue;

            struct PreparedCascade
            {
                osg::Matrixd mViewMatrix;
                osg::Matrixd mProjectionMatrix;
                osg::Vec3d mCenter;
                double mDiameter = 0.0;
                double mNear = 0.0;
                double mFar = 0.0;
                double mRadius = 0.0;
                bool mRefreshRequested = false;
            };

            std::vector<PreparedCascade> preparedCascades(cascadeCount);
            for (unsigned int cascade = 0; cascade < cascadeCount; ++cascade)
            {
                StableViewDependentData::Cascade& cache = stableVdd->mCascades[cascade];
                PreparedCascade& prepared = preparedCascades[cascade];

                prepared.mNear = computeSplitDistance(
                    minZNear, maxZFar, cascade, cascadeCount, mSettings.mSplitLambda);
                prepared.mFar = computeSplitDistance(
                    minZNear, maxZFar, cascade + 1, cascadeCount, mSettings.mSplitLambda);
                const std::array<osg::Vec3d, 8> corners = getCascadeCorners(frustum, prepared.mNear, prepared.mFar);

                prepared.mCenter = average(corners);
                prepared.mRadius = std::ceil(radius(corners, prepared.mCenter) * 16.0) / 16.0;
                prepared.mDiameter = prepared.mRadius * 2.0;

                if (mSettings.mTexelSnapping)
                    prepared.mCenter = snapCenterToTexels(
                        prepared.mCenter, lightSide, lightUp, prepared.mDiameter, mSettings.mResolution);

                const double depthRange = std::max<double>(mSettings.mDistance, prepared.mRadius * 4.0);

                prepared.mViewMatrix.makeLookAt(
                    prepared.mCenter - effectiveLightDir * depthRange, prepared.mCenter, lightUp);

                prepared.mProjectionMatrix.makeOrtho(
                    -prepared.mRadius, prepared.mRadius, -prepared.mRadius, prepared.mRadius, 0.0, depthRange * 2.0);

                const double centerThreshold = centerRefreshThreshold(prepared.mDiameter, mSettings.mResolution);
                const bool centerChanged
                    = !cache.mValid || (cache.mCenter - prepared.mCenter).length2() > centerThreshold * centerThreshold;
                const bool cascadeShapeChanged = !cache.mValid || differs(cache.mDiameter, prepared.mDiameter)
                    || differs(cache.mNear, prepared.mNear) || differs(cache.mFar, prepared.mFar);
                prepared.mRefreshRequested = !cache.mValid || configChanged
                    || (intervalElapsed && (sunMoved || centerChanged || cascadeShapeChanged));
            }

            const bool refreshAllRequestedCascades
                = configChanged || !stableVdd->mRenderedOnce || (intervalElapsed && sunJumped);
            unsigned int selectedCascade = cascadeCount;
            if (!refreshAllRequestedCascades && intervalElapsed && cascadeCount > 0)
            {
                const unsigned int startCascade = stableVdd->mNextCascadeRefreshIndex % cascadeCount;
                for (unsigned int offset = 0; offset < cascadeCount; ++offset)
                {
                    const unsigned int candidate = (startCascade + offset) % cascadeCount;
                    if (preparedCascades[candidate].mRefreshRequested)
                    {
                        selectedCascade = candidate;
                        break;
                    }
                }
            }

            for (unsigned int cascade = 0; cascade < cascadeCount; ++cascade)
            {
                osg::ref_ptr<ShadowData> shadowData;
                if (previousSdl.empty())
                    shadowData = new ShadowData(vdd);
                else
                {
                    shadowData = previousSdl.front();
                    previousSdl.erase(previousSdl.begin());
                }

                osg::ref_ptr<osg::Camera> camera = shadowData->_camera;
                applyShadowCameraCullingSettings(*camera);

                StableViewDependentData::Cascade& cache = stableVdd->mCascades[cascade];
                const PreparedCascade& prepared = preparedCascades[cascade];
                const bool refreshCascade = prepared.mRefreshRequested
                    && (refreshAllRequestedCascades || !cache.mValid || cascade == selectedCascade);

                if (refreshCascade)
                {
                    cache.mViewMatrix = prepared.mViewMatrix;
                    cache.mProjectionMatrix = prepared.mProjectionMatrix;
                    cache.mCenter = prepared.mCenter;
                    cache.mDiameter = prepared.mDiameter;
                    cache.mNear = prepared.mNear;
                    cache.mFar = prepared.mFar;
                    cache.mValid = true;
                }

                camera->setViewMatrix(cache.mViewMatrix);
                camera->setProjectionMatrix(cache.mProjectionMatrix);
                if (!dynamic_cast<StableCameraCullCallback*>(camera->getCullCallback()))
                    camera->setCullCallback(new StableCameraCullCallback(this));

                double cascadeCullMs = 0.0;
                if (refreshCascade)
                {
                    cv.pushStateSet(_shadowCastingStateSet.get());

                    const osg::Timer_t cascadeCullStart = osg::Timer::instance()->tick();
                    cullShadowCastingScene(&cv, camera.get());
                    cascadeCullMs
                        = osg::Timer::instance()->delta_m(cascadeCullStart, osg::Timer::instance()->tick());

                    cv.popStateSet();
                    if (!refreshAllRequestedCascades)
                        stableVdd->mNextCascadeRefreshIndex = (cascade + 1) % cascadeCount;
                    refreshedAnyCascade = true;
                }

                assignShadowStateSettings(cv, camera, cascade, vddUniforms);

                light.textureUnits.push_back(textureUnit);
                shadowData->_textureUnit = textureUnit;
                shadowData->_sm_i = cascade;
                sdl.push_back(shadowData);

                if (_debugHud)
                    _debugHud->draw(shadowData->_texture, cascade,
                        camera->getViewMatrix() * camera->getProjectionMatrix(), cv);

                if (_cascadeStatsEnabled && (refreshCascade || cv.getTraversalNumber() % 120 == 0))
                {
                    const osg::Vec2s textureSize = settings->getTextureSize();
                    Log(Debug::Info) << "Shadow cascade method=stable-csm cascade=" << cascade
                                     << " refreshed=" << refreshCascade
                                     << " mask=" << settings->getCastsShadowTraversalMask()
                                     << " cull_ms=" << cascadeCullMs << " resolution=" << textureSize.x() << "x"
                                     << textureSize.y() << " distance=" << mSettings.mDistance
                                     << " near=" << prepared.mNear << " far=" << prepared.mFar
                                     << " radius=" << prepared.mRadius;
                }

                ++textureUnit;
                ++numValidShadows;
            }

            if (refreshedAnyCascade)
            {
                stableVdd->mLightDir = effectiveLightDir;
                stableVdd->mLightDirValid = true;
            }
        }

        if (refreshedAnyCascade)
        {
            stableVdd->mRenderedOnce = true;
            stableVdd->mLastRefreshTime = currentTime;
            stableVdd->mLastCasterMask = casterMask;
            stableVdd->mLastCascadeCount = cascadeCount;
            stableVdd->mLastResolution = mSettings.mResolution;
            stableVdd->mLastDistance = mSettings.mDistance;
            stableVdd->mLastSplitLambda = mSettings.mSplitLambda;
            stableVdd->mLastTexelSnapping = mSettings.mTexelSnapping;
        }

        vdd->setNumValidShadows(numValidShadows);

        if (_cascadeStatsEnabled && (refreshedAnyCascade || cv.getTraversalNumber() % 120 == 0))
            Log(Debug::Info) << "Shadow cascades method=stable-csm valid=" << numValidShadows
                             << " requested=" << cascadeCount;

        if (numValidShadows > 0)
            prepareStateSetForRenderingShadow(*vdd, cv.getTraversalNumber());
    }
}

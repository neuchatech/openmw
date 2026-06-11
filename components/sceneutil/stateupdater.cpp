#include "stateupdater.hpp"

#include <osg/Fog>
#include <osg/PolygonMode>

#include "depth.hpp"
#include "rtt.hpp"
#include "statesetupdater.hpp"

#include <components/resource/scenemanager.hpp>
#include <components/settings/values.hpp>
#include <components/stereo/multiview.hpp>
#include <components/stereo/stereomanager.hpp>

namespace SceneUtil
{
    PerViewUniformStateUpdater::PerViewUniformStateUpdater(Resource::SceneManager* sceneManager, int opaqueTextureUnit)
        : mSceneManager(sceneManager)
        , mOpaqueTextureUnit(opaqueTextureUnit)
    {
    }

    void PerViewUniformStateUpdater::setDefaults(osg::StateSet* stateset)
    {
        stateset->addUniform(new osg::Uniform("projectionMatrix", osg::Matrixf{}));
        if (mSkyRTT)
            stateset->addUniform(new osg::Uniform("sky", mSkyTextureUnit));
    }

    void PerViewUniformStateUpdater::apply(osg::StateSet* stateset, osg::NodeVisitor* nv)
    {
        stateset->getUniform("projectionMatrix")->set(mProjectionMatrix);
        if (mSkyRTT && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
        {
            osg::Texture* skyTexture = mSkyRTT->getColorTexture(static_cast<osgUtil::CullVisitor*>(nv));
            stateset->setTextureAttribute(
                mSkyTextureUnit, skyTexture, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        }

        if (mOpaqueTextureUnit > 0)
            stateset->setTextureAttribute(mOpaqueTextureUnit,
                mSceneManager->getOpaqueDepthTex(nv->getTraversalNumber()), osg::StateAttribute::ON);
    }

    void PerViewUniformStateUpdater::applyLeft(osg::StateSet* stateset, osgUtil::CullVisitor* nv)
    {
        stateset->getUniform("projectionMatrix")->set(getEyeProjectionMatrix(0));
    }

    void PerViewUniformStateUpdater::applyRight(osg::StateSet* stateset, osgUtil::CullVisitor* nv)
    {
        stateset->getUniform("projectionMatrix")->set(getEyeProjectionMatrix(1));
    }

    void PerViewUniformStateUpdater::setProjectionMatrix(const osg::Matrixf& projectionMatrix)
    {
        mProjectionMatrix = projectionMatrix;
    }

    const osg::Matrixf& PerViewUniformStateUpdater::getProjectionMatrix() const
    {
        return mProjectionMatrix;
    }

    void PerViewUniformStateUpdater::enableSkyRTT(int skyTextureUnit, RTTNode* skyRTT)
    {
        mSkyTextureUnit = skyTextureUnit;
        mSkyRTT = skyRTT;
    }

    osg::Matrixf PerViewUniformStateUpdater::getEyeProjectionMatrix(int view)
    {
        return Stereo::Manager::instance().computeEyeProjection(view, AutoDepth::isReversed());
    }

    SharedUniformStateUpdater::SharedUniformStateUpdater(float skyBlendingStartCoef)
        : mSkyBlendingStartCoef(skyBlendingStartCoef)
    {
    }

    void SharedUniformStateUpdater::setDefaults(osg::StateSet* stateset)
    {
        stateset->addUniform(new osg::Uniform("near", 0.f));
        stateset->addUniform(new osg::Uniform("far", 0.f));
        stateset->addUniform(new osg::Uniform("skyBlendingStart", 0.f));
        stateset->addUniform(new osg::Uniform("screenRes", osg::Vec2f{}));
        stateset->addUniform(new osg::Uniform("isReflection", false));
        stateset->addUniform(new osg::Uniform("windSpeed", 0.0f));
        stateset->addUniform(new osg::Uniform("playerPos", osg::Vec3f(0.f, 0.f, 0.f)));
        stateset->addUniform(new osg::Uniform("cameraPos", osg::Vec3f(0.f, 0.f, 0.f)));
        stateset->addUniform(new osg::Uniform("invViewMatrix", osg::Matrixf{}));
        stateset->addUniform(new osg::Uniform("useTreeAnim", false));

        stateset->addUniform(new osg::Uniform("exponentialFogDensity", 0.f));
        stateset->addUniform(new osg::Uniform("heightFogEnabled", false));
        stateset->addUniform(new osg::Uniform("heightFogDensity", 0.f));
        stateset->addUniform(new osg::Uniform("heightFogFalloff", 0.f));
        stateset->addUniform(new osg::Uniform("heightFogOffset", 0.f));

        stateset->addUniform(new osg::Uniform("fogStart", 0.f));
        stateset->addUniform(new osg::Uniform("fogEnd", 0.f));
        stateset->addUniform(new osg::Uniform("fogColor", osg::Vec4f(0.f, 0.f, 0.f, 0.f)));
    }

    void SharedUniformStateUpdater::apply(osg::StateSet* stateset, osg::NodeVisitor* nv)
    {
        if (osg::Uniform* nearU = stateset->getUniform("near"))
            nearU->set(mNear);
        if (osg::Uniform* farU = stateset->getUniform("far"))
            farU->set(mFar);
        if (osg::Uniform* sBStartU = stateset->getUniform("skyBlendingStart"))
            sBStartU->set(mFar * mSkyBlendingStartCoef);
        if (osg::Uniform* sResU = stateset->getUniform("screenRes"))
            sResU->set(mScreenRes);
        if (osg::Uniform* wSpeedU = stateset->getUniform("windSpeed"))
            wSpeedU->set(mWindSpeed);
        if (osg::Uniform* pPosU = stateset->getUniform("playerPos"))
            pPosU->set(mPlayerPos);
        if (osg::Uniform* cPosU = stateset->getUniform("cameraPos"))
            cPosU->set(mCameraPos);
        if (osg::Uniform* iVMatU = stateset->getUniform("invViewMatrix"))
            iVMatU->set(mInvViewMatrix);

        if (osg::Uniform* expFogU = stateset->getUniform("exponentialFogDensity"))
            expFogU->set(Settings::fog().mExponentialFogDensity);
        if (osg::Uniform* hFogEU = stateset->getUniform("heightFogEnabled"))
            hFogEU->set(Settings::fog().mHeightFogEnabled);
        if (osg::Uniform* hFogDU = stateset->getUniform("heightFogDensity"))
            hFogDU->set(Settings::fog().mHeightFogDensity);
        if (osg::Uniform* hFogFU = stateset->getUniform("heightFogFalloff"))
            hFogFU->set(Settings::fog().mHeightFogFalloff);
        if (osg::Uniform* hFogOU = stateset->getUniform("heightFogOffset"))
            hFogOU->set(Settings::fog().mHeightFogOffset);

        if (osg::Uniform* fSU = stateset->getUniform("fogStart"))
            fSU->set(mFogStart);
        if (osg::Uniform* fEU = stateset->getUniform("fogEnd"))
            fEU->set(mFogEnd);
        if (osg::Uniform* fCU = stateset->getUniform("fogColor"))
            fCU->set(mFogColor);
    }

    void SharedUniformStateUpdater::setNear(float near)
    {
        mNear = near;
    }

    void SharedUniformStateUpdater::setFar(float far)
    {
        mFar = far;
    }

    void SharedUniformStateUpdater::setScreenRes(float width, float height)
    {
        mScreenRes = osg::Vec2f(width, height);
    }

    void SharedUniformStateUpdater::setWindSpeed(float windSpeed)
    {
        mWindSpeed = windSpeed;
    }

    void SharedUniformStateUpdater::setPlayerPos(osg::Vec3f playerPos)
    {
        mPlayerPos = playerPos;
    }

    void SharedUniformStateUpdater::setCameraPos(osg::Vec3f cameraPos)
    {
        mCameraPos = cameraPos;
    }

    void SharedUniformStateUpdater::setInvViewMatrix(const osg::Matrixf& matrix)
    {
        mInvViewMatrix = matrix;
    }

    void SharedUniformStateUpdater::setFogStart(float start)
    {
        mFogStart = start;
    }

    void SharedUniformStateUpdater::setFogEnd(float end)
    {
        mFogEnd = end;
    }

    void SharedUniformStateUpdater::setFogColor(const osg::Vec4f& color)
    {
        mFogColor = color;
    }

    void StateUpdater::setDefaults(osg::StateSet* stateset)
    {
        osg::Fog* fog = new osg::Fog;
        fog->setMode(osg::Fog::LINEAR);
        stateset->setAttributeAndModes(fog, osg::StateAttribute::ON);
        if (mWireframe)
        {
            osg::PolygonMode* polygonmode = new osg::PolygonMode;
            polygonmode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
            stateset->setAttributeAndModes(polygonmode, osg::StateAttribute::ON);
        }
        else
            stateset->removeAttribute(osg::StateAttribute::POLYGONMODE);
    }

    void StateUpdater::apply(osg::StateSet* stateset, osg::NodeVisitor*)
    {
        configureSunAmbientOverride(mAmbientColor, stateset);
        osg::Fog* fog = static_cast<osg::Fog*>(stateset->getAttribute(osg::StateAttribute::FOG));
        fog->setColor(mFogColor);
        fog->setStart(mFogStart);
        fog->setEnd(mFogEnd);
    }

    void StateUpdater::setAmbientColor(const osg::Vec4f& col)
    {
        mAmbientColor = col;
    }

    void StateUpdater::setFogColor(const osg::Vec4f& col)
    {
        mFogColor = col;
    }

    void StateUpdater::setFogStart(float start)
    {
        mFogStart = start;
    }

    void StateUpdater::setFogEnd(float end)
    {
        mFogEnd = end;
    }

    void StateUpdater::setWireframe(bool wireframe)
    {
        if (mWireframe != wireframe)
        {
            mWireframe = wireframe;
            reset();
        }
    }

    bool StateUpdater::getWireframe() const
    {
        return mWireframe;
    }

}

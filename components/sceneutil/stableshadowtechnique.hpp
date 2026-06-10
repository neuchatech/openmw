#ifndef COMPONENTS_SCENEUTIL_STABLESHADOWTECHNIQUE_H
#define COMPONENTS_SCENEUTIL_STABLESHADOWTECHNIQUE_H

#include "mwshadowtechnique.hpp"

namespace SceneUtil
{
    class StableShadowTechnique : public MWShadowTechnique
    {
    public:
        struct Settings
        {
            int mCascadeCount = 3;
            int mResolution = 2048;
            float mDistance = 8192.f;
            float mSplitLambda = 0.65f;
            bool mTexelSnapping = true;
            int mUpdateIntervalMilliseconds = 100;
            float mSunUpdateAngleThreshold = 0.25f;
        };

        StableShadowTechnique();
        StableShadowTechnique(const StableShadowTechnique& rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

        META_Object(SceneUtil, StableShadowTechnique)

        void cull(osgUtil::CullVisitor& cv) override;

        void setStableSettings(const Settings& settings);

        void assignShadowStateSettings(
            osgUtil::CullVisitor& cv, osg::Camera* camera, unsigned int sm_i, Uniforms& uniforms) override;

    protected:
        virtual ~StableShadowTechnique();

    private:
        class StableViewDependentData;

        ViewDependentData* createViewDependentData(osgUtil::CullVisitor* cv) override;

        Settings mSettings;
    };
}

#endif

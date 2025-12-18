# Rendering Loop & Shadow Synchronization Improvements

During the investigation of 1-frame shadow lag (which was eventually resolved via monitor settings), several architectural issues and potential optimizations were identified in the OpenMW rendering pipeline. This document outlines these findings for future technical debt resolution.

## Technical Debt & Synchronization Issues

### 1. Mathematical 1-Frame Sync Risk in Shadows
In `MWShadowTechnique::cull`, the `shadowSpaceMatrix` is computed using `cv.getModelViewMatrix()`.
- **The Issue**: There is a risk that the shadow cameras are processed before the main camera's view matrix has been fully updated for the current frame by the `UpdateRenderCameraCallback`.
- **Potential Fix**: Explicitly pass the current frame's camera matrices from `RenderingManager` to `MWShadowTechnique` to ensure total synchronization regardless of OSG traversal order.

### 2. The "First Person Hack" (Camera-Animation Dependency)
`MWRender::Camera::updateCamera` contains a hack to manually recalculate the camera position because it depends on neck animations which are updated *during* the cull traversal.
- **The Issue**: Circular dependency where the camera needs animation data, but animations haven't been "baked" yet when the camera update starts.
- **Potential Fix**: Decouple the camera from the OSG animation nodes. Move animation updates to a dedicated "pre-cull" update phase so that the camera and shadows can consume final world-space positions.

### 3. Redundant Matrix Inversions
`SceneUtil::MWShadowTechnique::assignShadowStateSettings` performs a matrix inversion every frame:
```cpp
osg::Matrix inverseViewMatrix = osg::Matrix::inverse(*cv.getModelViewMatrix());
```
- **The Issue**: `RenderingManager::update` already calculates this inverse and provides it as the `invViewMatrix` uniform.
- **Potential Fix**: Reuse the pre-calculated `mInvViewMatrix` from `RenderingManager` or `SharedUniformStateUpdater`.

### 4. Shadow Near-Clip Precision (Issue #6072)
A `// TODO` in `renderingmanager.cpp` notes that `mNearClip` is bounded because small values break OSG shadow calculations on the CPU.
- **The Issue**: This limits depth precision and causes near-camera clipping.
- **Potential Fix**: Debug the light-space frustum bounds calculation in `MWShadowTechnique` to handle high dynamic ranges in near/far ratios without precision loss.

## Performance Optimizations

### 5. Efficient Uniform Updates
`RenderingManager::update` pushes several uniforms (e.g., `screenRes`, `windSpeed`) to the `SharedUniformStateUpdater` every frame.
- **Improvement**: Implement a "dirty" flag or a comparison check to only update these uniforms when their source values actually change (especially for static values like resolution).

### 6. Shader Property Mapping
The shadow mapping pipeline uses `traversalNumber % 2` to double-buffer uniforms.
- **Improvement**: While necessary for multi-threading, the complexity of managing `_uniforms[2]` in `MWShadowTechnique` could be simplified by abstracting the state-switching into a more robust "FrameState" object used across the whole renderer.

## Summary of Targeted Files
- [renderingmanager.cpp](file:///d:/data/code/openmw/apps/openmw/mwrender/renderingmanager.cpp) - Main update loop and uniform dispatch.
- [camera.cpp](file:///d:/data/code/openmw/apps/openmw/mwrender/camera.cpp) - Camera matrix logic and the FirstPerson hack.
- [mwshadowtechnique.cpp](file:///d:/data/code/openmw/components/sceneutil/mwshadowtechnique.cpp) - Shadow matrix derivation and uniform management.

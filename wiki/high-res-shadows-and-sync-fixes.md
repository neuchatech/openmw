# High-Resolution Shadows & Rendering Sync Fixes

Summary of changes made to improve shadow fidelity and resolve rendering latency issues.

## 1. High-Resolution Shadows
The shadow mapping system has been tuned for higher fidelity, especially useful for distant land setups.

- **Shadow Map Resolution**: Increased from 1024 to **8192** in `settings-default.cfg`. This significantly reduces pixelation and flickering on shadow edges.
- **Shadow Cascades**: increased `number of shadow maps` from 3 to **4**. This provides a better distribution of resolution across the viewing distance.
- **Shadow Distance**: increased `maximum shadow map distance` to **16384** (double the default) to maintain shadow visibility at higher viewing distances.

## 2. Rendering Synchronization (Fog Lag Fix)
A fundamental synchronization issue in `RenderingManager` was identified where camera updates occurred after uniform state updates.

- **Update Order**: Moved `mCamera->update` to the very beginning of `RenderingManager::update`.
- **Immediate Matrix Computation**: Modified `Camera::update` to immediately compute and store the `mViewMatrix`. 
- **Result**: Uniforms such as `invViewMatrix`, `cameraPos`, and fog parameters now use the actual camera position for the current frame, eliminating the 1-frame "catch-up" effect previously visible in the fog.

## 3. Ongoing Investigation: Shadow "Ghosting" / Lighting Pops
While the fog update is now real-time, shadows still exhibit a minor 1-frame artifact during rapid camera rotation.

- **Current Status**: The camera update order fix has improved the situation, but the shadow mapping pipeline (`MWShadowTechnique`) has its own internal state management that may still be lagging.
- **Next Steps**: Investigating the synchronization between the main camera's cull traversal and the shadow camera's settings derivation in `MWShadowTechnique::cull`.

---
*Status: It was just the monito with a too stong pediction setting. No issue after all, but code smells remain.*

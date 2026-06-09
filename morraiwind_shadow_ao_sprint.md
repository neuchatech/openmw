# Morraiwind Shadow and AO Sprint

## Purpose

Exterior performance in dense areas, especially Tamriel Rebuilt cities such as Old Ebonheart, is currently dominated by shadow rendering. The existing OpenMW shadow path is based on OSG `ViewDependentShadowMap` logic with cascaded shadow maps and view-dependent fitting. In practice it is expensive, visually unstable from some camera angles, and particularly bad when `object shadows` includes paged static world geometry.

This sprint replaces the current outdoor shadow strategy with a modern, stable cascaded shadow map path and adds a lightweight ambient occlusion pass to keep small objects grounded without requiring every static asset to cast real-time shadows.

## Current Findings

Measured Old Ebonheart performance shows the bottleneck is render/cull/GPU work, not Lua, AI, physics, or fog.

Representative smoke-test results:

| Variant | Average FPS | Average Frame Time | Render | Cull | GPU |
| --- | ---: | ---: | ---: | ---: | ---: |
| All shadows, 4096 | ~36.5 | ~29.1 ms | ~23.3 ms | ~14.9 ms | ~24.4 ms |
| 2x2048, distance 8192 | ~39.0 | ~25.9 ms | ~20.3 ms | ~12.8 ms | ~21.4 ms |
| 2x1024, distance 4096 | ~44.0 | ~23.0 ms | ~17.3 ms | ~11.3 ms | ~18.4 ms |
| 2x2048, no object shadows | ~55.3 | ~18.5 ms | ~12.9 ms | ~8.7 ms | ~14.6 ms |
| No shadows | ~57.8 | ~17.7 ms | ~11.9 ms | ~8.1 ms | ~14.3 ms |

Important code paths:

- `apps/openmw/mwrender/renderingmanager.cpp`: `object shadows` currently enables both `Mask_Object` and `Mask_Static`.
- `apps/openmw/mwrender/objectpaging.cpp`: paged exterior object chunks are assigned `Mask_Static`.
- `components/sceneutil/mwshadowtechnique.cpp`: current shadow cameras are rebuilt during cull traversal and are view-dependent.
- `components/sceneutil/mwshadowtechnique.cpp`: shadow cameras disable small-feature culling.
- `components/fx` and `.omwfx`: post-processing already exposes depth, normals, world position, sun data, and resolution helpers, making AO feasible without a renderer rewrite.

## Goals

1. Replace the fragile outdoor shadow behavior with a stable CSM implementation.
2. Keep exterior shadows visually stable while rotating the camera.
3. Reduce shadow cull/draw cost in dense exterior cities.
4. Provide clear settings for caster categories and quality/performance tradeoffs.
5. Add AO/contact AO to ground small objects and contact areas.
6. Preserve the current shadow system as a fallback until the new path is proven.
7. Keep the work scoped to OpenGL/OpenSceneGraph; do not attempt a DLSS or renderer-backend migration in this sprint.

## Non-Goals

- Do not implement DLSS, FSR 2, XeSS, frame generation, or Vulkan/DX backends.
- Do not attempt virtual shadow maps.
- Do not implement full real-time GI.
- Do not remove the existing shadow code until the new path is tested across vanilla and Tamriel Rebuilt scenes.
- Do not make static buildings all cast high-resolution real-time shadows by default if doing so prevents stable 60 FPS.

## Proposed User Settings

Add settings under `[Shadows]`:

```ini
# old/current path or new stable path
shadow mapping method = stable csm

# New stable CSM quality controls
stable csm cascades = 3
stable csm resolution = 2048
stable csm distance = 8192
stable csm split lambda = 0.65
stable csm texel snapping = true
stable csm update interval = 1
stable csm sun update angle threshold = 0.25

# Caster category controls
actor shadows = true
player shadows = true
terrain shadows = true
object shadows = true
static object shadows = false
small object shadows = false

# Caster filtering
shadow caster min radius = 32.0
shadow caster max distance = 8192
shadow small feature culling = true
shadow small feature culling pixel size = 4.0

# AO
enable ambient occlusion = true
ao method = contact
ao radius = 90.0
ao strength = 0.65
ao sample count = 8
ao fade distance = 4096
```

Compatibility notes:

- Preserve existing setting names where possible.
- Keep `object shadows` behavior understandable for users.
- Add `static object shadows` so dense paged world geometry can be controlled separately from smaller objects.
- Defaults should favor stable performance in dense exterior modded cities.

## Stable CSM Architecture

The new path should be implemented beside the existing `MWShadowTechnique`, not as a risky in-place rewrite.

Suggested classes:

- `SceneUtil::StableShadowTechnique`
- `SceneUtil::StableShadowCascade`
- `SceneUtil::ShadowCasterFilter`

Integration:

- `SceneUtil::ShadowManager` chooses the technique from `shadow mapping method`.
- Existing shader defines and shadow texture uniforms should be reused where practical.
- If shader-side changes are needed, keep old and new defines side by side until migration is complete.

### Cascade Construction

For each frame requiring a shadow update:

1. Compute cascade split depths from camera near to `stable csm distance`.
2. Use a practical split scheme:
   - uniform/log blend controlled by `stable csm split lambda`
   - default around `0.65`
3. For each cascade, compute the camera frustum slice corners in world space.
4. Fit a bounding sphere or stable AABB to the slice.
5. Build a directional-light view matrix from sun direction.
6. Fit an orthographic projection around the stable cascade bounds.
7. Snap the projection center to shadow texel increments when `stable csm texel snapping = true`.
8. Render shadow casters into one depth texture per cascade.

Important: cascade extents should not resize aggressively based on the exact visible caster bounds. Stable CSM should trade a little resolution for predictable, non-swimming shadows.

### Texel Snapping

Texel snapping is required. Without it, shadows will shimmer while the camera moves.

Implementation sketch:

```cpp
const float worldUnitsPerTexel = cascadeDiameter / shadowMapResolution;
lightSpaceCenter.x() = std::floor(lightSpaceCenter.x() / worldUnitsPerTexel) * worldUnitsPerTexel;
lightSpaceCenter.y() = std::floor(lightSpaceCenter.y() / worldUnitsPerTexel) * worldUnitsPerTexel;
```

Use the snapped light-space center to rebuild the cascade orthographic projection.

### Refresh Policy

Stable CSM can support throttling better than the current view-dependent system, but the default should remain every frame until quality is verified.

Refresh shadows when any of these happen:

- `stable csm update interval` elapsed.
- Camera moved far enough to shift snapped cascade center.
- Sun direction changed more than `stable csm sun update angle threshold`.
- Cell/worldspace changed.
- Shadow-related settings changed.
- Caster masks changed.
- Debug overlay/HUD requires fresh maps.

Do not throttle actor-only near shadows initially unless visual tests show it is acceptable.

### Caster Filtering

The current `object shadows` cost comes from including both `Mask_Object` and `Mask_Static`.

New caster categories:

- Actors: `Mask_Actor`
- Player: `Mask_Player`
- Terrain: `Mask_Terrain`
- Dynamic/small objects: `Mask_Object`
- Paged/static world geometry: `Mask_Static`

Default plan:

- Actors: enabled
- Player: enabled
- Terrain: enabled
- Objects: enabled
- Static objects: disabled
- Small object shadows: disabled or radius-filtered

Add a filter for static/object casters:

- reject small casters by bounding radius
- reject far object casters by distance
- allow large static casters only in near cascades if `static object shadows = true`

This requires care because OSG traversal masks are coarse. If per-object rejection is not easy inside the existing traversal, start with mask-level controls, then add a shadow-specific cull callback or node visitor.

## Ambient Occlusion / Contact AO

AO should be implemented as an `.omwfx` post-process first, not as a core renderer pass.

The post-processing API already provides:

- depth sampler
- normals sampler when `pass_normals = true`
- `omw_GetLinearDepth`
- `omw_GetWorldPosFromUV`
- `omw_GetNormals`
- fog and camera data

Recommended first AO pass:

- small-radius contact AO
- 6 to 8 taps
- depth and normal rejection
- distance fade
- fog-aware fade
- optional cheap blur if noise is visible

The AO target is not cinematic SSAO. It is a grounding layer for:

- clutter
- rocks
- steps
- actor feet
- crates/barrels
- building-ground contact lines near the camera

AO should not be expected to replace large building shadows.

## Implementation Plan

### Phase 1: Instrumentation

- Add optional CPU/GPU timing around each shadow cascade.
- Report:
  - cascade index
  - caster mask
  - cull time
  - draw/GPU time where available
  - number of valid cascades
  - resolution and distance
- Extend existing smoke-test workflow to capture these values.

Acceptance:

- Old Ebonheart run shows per-cascade cost.
- We can compare old shadows, stable CSM, and AO profiles.

### Phase 2: Caster Mask Split

- Add `static object shadows`.
- Change outdoor shadow mask assembly so `object shadows` no longer automatically includes `Mask_Static`.
- Keep existing behavior available with `static object shadows = true`.

Acceptance:

- `object shadows = true`, `static object shadows = false` works.
- Old Ebonheart FPS is close to the previous no-object-shadow profile.
- No missing actor/player/terrain shadows.

### Phase 3: Stable CSM Prototype

- Add `StableShadowTechnique`.
- Implement 2 to 4 cascades.
- Use orthographic directional-light projections.
- Implement texel snapping.
- Reuse existing shadow textures/uniform layout if possible.
- Keep old `MWShadowTechnique` selectable.

Acceptance:

- Shadows remain stable while slowly rotating and strafing.
- No severe cascade swimming.
- Old Ebonheart performance is no worse than the current tuned 2x2048 profile, before further filtering.

### Phase 4: Caster Filtering and Quality Controls

- Add radius/distance filters for object/static casters.
- Add small-feature culling controls for shadow cameras.
- Limit static casters to near cascades if enabled.

Acceptance:

- Dense city static shadows can be enabled experimentally without catastrophic FPS loss.
- Default profile remains near 60 FPS target in Old Ebonheart.

### Phase 5: AO Prototype

- Add `contactao.omwfx`.
- Add settings and shader uniforms.
- Enable normals for the technique.
- Tune for subtle grounding, not heavy dirt.

Acceptance:

- AO adds visible grounding to small objects.
- AO cost is low enough to keep the default exterior profile near target FPS.
- AO fades gracefully with fog/distance.

### Phase 6: Visual and Perf Pass

Test scenes:

- Old Ebonheart, Tamriel Rebuilt
- Vanilla Balmora exterior
- Seyda Neen swamp/forest
- Ashlands open terrain
- Interior with actors
- Dawn/dusk with changing sun angle
- Rain/fog weather

Metrics:

- average FPS
- p95 frame time
- render/cull/GPU time
- cascade timings
- visible shimmer
- cascade transition artifacts
- actor/object grounding

Target:

- Dense exterior target: stable 60 FPS where possible on the current test machine.
- Interior target: do not regress current high FPS behavior.

## Risks

- Existing shaders may assume the current shadow-space matrices and cascade behavior.
- OSG render-stage plumbing may make a clean new technique harder than expected.
- Static caster filtering may need a custom visitor instead of traversal masks.
- AO normal buffer may increase cost when post-processing is otherwise disabled.
- Shadow throttling can create stale dynamic shadows if applied too broadly.

## Fallback Strategy

Keep current shadows behind:

```ini
shadow mapping method = legacy vdsm
```

If stable CSM is not ready for V1, ship:

- split static/object shadow settings
- tuned shadow defaults
- contact AO
- documented performance profile

## Definition of Done

- New stable CSM path can be enabled from settings.
- Legacy path remains available.
- Default Morraiwind shadow profile performs materially better in Old Ebonheart.
- Camera rotation no longer causes severe shadow resolution collapse.
- Small objects feel grounded with AO even when static object shadows are disabled.
- Smoke-test results are documented with before/after numbers.
- README or release notes mention the new shadow/AO options.

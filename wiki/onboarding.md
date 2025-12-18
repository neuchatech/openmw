# OpenMW Development Onboarding

Welcome! This document provides the essential information for contributors working on the current phase of the project.

## 1. Project Context
We are currently in a **Combat & Rendering Polish** phase. The primary focus is moving combat away from RNG-heavy mechanics towards a more dynamic, stamina-driven system, while also ensuring the rendering pipeline is synchronized and high-performance.

### Key Active Documents
- [Codebase Structure](codebase_structure.md) - Map of features to files.
- [Combat Rework Design](combat_rework_design.md) - Vision for the new combat system.
- [Render Loop Refactor Tasks](render-loop-refactor-tasks.md) - Ongoing rendering optimizations and cleanup.

## 2. Recent Accomplishments

### Combat & Mechanics (Sprint 6 Highlights)
- **Stamina Shield**: Physical damage now drains Fatigue before Health in `creaturestats.cpp`.
- **High-Cost Actions**: Swinging weapons now drains significant stamina (5x multiplier) in `combat.cpp`.
- **Dynamic Regen**: Faster fatigue regeneration (5x for player, 2.5x for AI) with health-gating and a 1-second delay upon depletion.

### Rendering & Sync
- **Camera-Uniform Sync**: Eliminated fog lag by moving `mCamera->update` to the top of the rendering loop.
- **High-Res Shadows**: Updated default settings to support 8192 shadow maps with 4 cascades for distant land setups.

## 3. High-Priority Next Tasks
1. **Heavy Attacks**: Implement the "high-risk, high-reward" charge system (1s charge for 4x damage).
2. **Hit Detection**: Solidify the transition from "Dice Roll" hit chance to physical collision detection.
3. **Render Refactor**: Address the "First Person Hack" in `camera.cpp` and fix the redundant matrix inversions in `MWShadowTechnique`.
4. **Stamina UI**: Improve the visibility of the "Stamina Shield" on the HUD.

## 4. Development Workflow

### Build Environment
We use **Visual Studio 2022** with **Ninja** and **CMake**.

**Restoring Environment (Git Bash):**
```bash
./CI/before_script.msvc.sh -k -p Win64 -v 2022 -N
```

**Compiling (Powershell):**
```powershell
. ./ActivateMSVC.ps1
.\fastcompile.bat
.\releasecompile.bat
```

### Essential Files to Watch
- `apps/openmw/mwrender/renderingmanager.cpp`: Main render loop entry.
- `apps/openmw/mwmechanics/combat.cpp`: Combat logic and accuracy.
- `apps/openmw/mwmechanics/creaturestats.cpp`: Health/Fatigue/Magicka management.
- `apps/openmw/mwmechanics/actors.cpp`: Actor updates and regeneration.

## 5. Tips for Agents
- Always check `MWBase::Environment::get()` to access global managers.
- Use `MWWorld::Ptr` for referencing any game object; never use raw pointers.
- When modifying combat, test both first-person and third-person modes to ensure sync.

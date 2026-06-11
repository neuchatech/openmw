Morraiwind
==========

Morraiwind is a personal gameplay and rendering fork of
[OpenMW](https://www.openmw.org), the open-source engine for playing
Morrowind by Bethesda Softworks.

You still need to own Morrowind. This repository contains engine changes only;
it does not include Bethesda game data.

V1 Focus
--------

* Version: 0.52.0
* License: GPLv3 (see [LICENSE](https://gitlab.com/OpenMW/openmw/-/raw/master/LICENSE) for more information)
* Website: https://www.openmw.org
* IRC: #openmw on irc.libera.chat
* Discord: https://discord.gg/bWuqq2e

This fork aims to make Morrowind feel wider, sharper, and more physical while
keeping the OpenMW foundation intact.

### Graphics and World Rendering

* Much higher default viewing distance.
* Distant terrain enabled by default.
* Reworked fog defaults for long-distance play.
* Radial, exponential, and height fog enabled for a softer horizon.
* Distant land fog tuned for large exterior views.
* Sky blending and camera-uniform sync improvements to reduce fog lag.
* Higher shadow quality defaults, including longer shadow distance and more
  cascades.

### Combat Rework

* Physical contact matters more: hits that connect can deal damage instead of
  being discarded by a pure dice-roll miss.
* Old hit chance is reused as damage scaling, so skill still matters.
* Combat damage scaling can be toggled in settings.
* Floating damage feedback is available.
* Fatigue is much more important:
  * attacks cost more stamina,
  * fatigue regeneration is delayed by damage instead of every action,
  * player, NPC, and creature regeneration can be tuned separately,
  * physical damage is absorbed by fatigue first through a stamina-shield
    model.
* Creature/NPC balance knobs were added for more dangerous fights.
* Balance settings are exposed in the in-game settings UI.

### V1 Stability Fixes

The current V1 patch fixes the two major blockers found during testing:

* Local map exploration updates no longer render newly updated regions black.
  The map and character-preview render-to-texture passes now override the full
  new fog shader uniform set instead of relying on legacy fog state.
* NPC/creature encounter crashes from attacks and projectiles are guarded more
  robustly. Empty or non-actor targets no longer flow into hit chance, Lua hit
  events, caster cell lookups, or projectile recovery logic.

See [morraiwind_bug_review_report.html](morraiwind_bug_review_report.html) for
the detailed investigation and implementation notes.

Build On Windows
----------------

This checkout is set up for Visual Studio 2022, CMake, and Ninja.

From PowerShell or Command Prompt:

```powershell
cd D:\data\code\openmw
.\releasecompile.bat
```

For a quicker development build:

```powershell
.\fastcompile.bat
```

If the build environment needs to be restored:

```powershell
.\restore_env.bat
```

Run The Game
------------

Release build:

```powershell
.\MSVC2022_64_Ninja\Release\openmw-launcher.exe
```

Or run the engine directly:

```powershell
.\MSVC2022_64_Ninja\Release\openmw.exe
```

Debug build, useful for crash investigation but much slower:

```powershell
.\MSVC2022_64_Ninja\Debug\openmw.exe
```

If OpenMW needs to locate or import the Morrowind game files again:

```powershell
.\MSVC2022_64_Ninja\Release\openmw-wizard.exe
```

Upstream OpenMW
---------------

Morraiwind is based on OpenMW 0.51.0-era development sources.

OpenMW is an open-source open-world RPG engine that supports Morrowind,
Tribunal, and Bloodmoon. It also includes OpenMW-CS, a replacement for
Bethesda's Construction Set.

Useful upstream links:

* Website: https://www.openmw.org
* Source: https://gitlab.com/OpenMW/openmw
* Documentation: https://openmw.readthedocs.io/
* License: GPLv3, see [LICENSE](LICENSE)

Font Licenses
-------------

* DejaVuLGCSansMono.ttf: custom, see
  [files/data/fonts/DejaVuFontLicense.txt](files/data/fonts/DejaVuFontLicense.txt)
* DemonicLetters.ttf: SIL Open Font License, see
  [files/data/fonts/DemonicLettersFontLicense.txt](files/data/fonts/DemonicLettersFontLicense.txt)
* MysticCards.ttf: SIL Open Font License, see
  [files/data/fonts/MysticCardsFontLicense.txt](files/data/fonts/MysticCardsFontLicense.txt)

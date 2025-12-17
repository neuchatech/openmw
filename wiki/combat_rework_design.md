# Combat Rework Design Document

## 1. Vision
To make OpenMW combat more dynamic, fair, and skill-based, moving away from RNG-heavy mechanics ("dice rolls") towards a system where stamina management and timing are key.

## 2. Core Mechanics

### 2.1. Stamina as a Shield ("The Buffer System")
Inspired by *Kingdom Come: Deliverance*, stamina will act as a primary defensive layer.
-   **Mechanism**: When an actor takes damage, it is deducted from **Stamina** (Fatigue) first.
-   **Break Point**: If Stamina reaches 0 (or a low threshold), subsequent attacks deal direct **Health** damage.
-   **Exceptions**: 
    -   Critical hits or specific "piercing" attacks might bypass some stamina.
    -   Option: 50/50 split? (Needs testing).

### 2.2. Heavy Attacks
Heavy attacks should be high-risk, high-reward moves.
-   **Charge Time**: Increased significantly (e.g., from ~0.2s to ~1.0s).
-   **Damage**: Massively increased (e.g., Base Dmg * 3 or * 4). A charged hit should feel devastating.
-   **Stamina Cost**: High cost to execute.
-   **function**: The primary way to break a turtle's stamina guard.

### 2.3. Stamina Economy
The flow of combat should cycle between aggression and defense.
-   **Attacks Cost More**: Swinging a weapon should drain stamina noticeably.
-   **Regeneration**: Stamina should regenerate much faster when not attacking/sprinting, allowing for quick "resets" if the player backs off.
-   **Capacity**: Actors should be able to perform a limited chain of attacks (e.g., 5-10 light attacks, 2-3 heavy attacks) before becoming exhausted ("Gassed").

### 2.4. Hit Detection vs RNG
-   Reduce the influence of the "Hit Chance" formula.
-   If the weapon physically connects (collision), it should register as a hit (checked against Stamina/Block).
-   High "Agility/Sanctuary" might still dodge, but the default should be solid hits.

## 3. Implementation Details (Brainstorming)
-   **HUD**: Stamina bar needs to be highly visible (maybe a secondary bar for enemies?).
-   **Feedback**: Visual/Audio cues when an enemy's stamina is broken (stagger, sound effect).
-   **Stats**:
    -   **Strength**: Increases Stamina Damage.
    -   **Endurance**: Increases Max Stamina and Block efficiency.
    -   **Speed**: Increases Attack Speed (maybe regen rate?).

## 4. Open Questions
-   How does Armor Factor calculation interact with Stamina damage?
-   Do we remove the "Dice Roll" misses entirely, or just clamp them to a high minimum (e.g., 90% chance)?
-   How do spells interact with this system? (Shock damage drains magicka, maybe Frost drains stamina?)

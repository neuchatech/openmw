# LLM Integration Roadmap for OpenMW

Transforming OpenMW into a dynamic, generative world using Large Language Models (LLMs).

## Core Concepts

### 1. Generative NPC Dialogue (The "Living Oracle")
*   **Vision**: Replace static dialogue trees with real-time responses via the Gemini 1.5 Flash API.
*   **Mechanism**: A C++ REST client that sends a prompt containing the player's message and a "World Context" (NPC personality, current cell, weather, player history).
*   **Context API**: A system to serialize game state into text for the LLM.

### 2. The Generative Game Master (The "Architect")
*   **Vision**: An LLM that monitors player actions and dynamically injects new content.
*   **Features**:
    *   **Dynamic Quests**: GM writes a quest based on the player's current location and level.
    *   **Runtime Content Injection**: GM spawns NPCs, items, and dialogue using an extended Lua API.
    *   **World Reactions**: NPCs in different towns might start talking about a quest the GM just "created" elsewhere.

### 3. Divine Intervention (The "Agentic Player")
*   **Vision**: A "Command Console" where the player describes a desired change ("Make it rain fire", "Give me a sword that glows blue"), and an agentic LLM implements it using the Lua API or console commands in real-time.
*   **Architecture**: Detailed design for security and sandboxing is documented in [divine_intervention_architecture.md](C:\Users\oncho\.gemini\antigravity\brain\2b9e5fe9-4fc9-4e71-9af4-c77ae86c896a\divine_intervention_architecture.md).

---

## Technical Contributions & Enhancements (Antigravity Ideas)

### A. RAG-Engine (Lore Anchoring)
To prevent NPCs from "hallucinating" facts (e.g., talking about cars in Vvardenfell), we will integrate a **Retrieval-Augmented Generation** system.
*   The engine will scrape the `ESM` records for Books and existing Dialogue.
*   Relevant lore snippets are injected into the prompt based on the topic.

### B. Procedural Voice-Acting (TTS Pipeline)
*   Integrate a lightweight TTS engine (like Piper or Silero) to turn the LLM's text output into audio files that play through the `SoundManager`.

### C. Combat Taunts & Morale
*   Combat AI doesn't just use behavior trees; it asks the LLM for a "Tactical Posture".
*   NPCs can shout context-aware taunts ("You swing that toothpick like a scribe!") or plead for mercy based on their personality.

### D. LLM-Driven Dynamic Scheduling
*   **Vision**: Replace static AI packages with LLM-decided goals.
*   **Mechanism**: A "Daily Planner" LLM call for key NPCs. Instead of just "go to the tavern at 8 PM", the LLM might decide: *"Today Fargoth is feeling adventurous and will try to sneak into the lighthouse to see the view."*

---

## Roadmap: Sprint 11 - The Connection

### Goal: Establish the Bridge
- [ ] **Infrastructure**: Implement a C++ `RESTClient` using `libcurl` to communicate with the Gemini API.
- [ ] **Settings**: Add `ai_api_key` to the settings system (hidden).
- [ ] **Lua Hooks**: Create Lua bindings so a mod can call `AI.requestResponse(npc_ptr, message)`.
- [ ] **Prompt Engineering**: Develop the "Context Scraper" to build descriptive prompts like:
    *"You are Fargoth, a Bosmer living in Seyda Neen. You are currently in the Seyda Neen docks. It is raining. The player is wearing expensive silk robes..."*

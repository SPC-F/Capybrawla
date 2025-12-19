# Capybrawla
**Repository for the game *Capybrawla*, built using the Capycore engine** 🎮


## 📂 Game File Structure

```
/src
│
├── main.cpp            # Kick the game off 🚀
│
├── game/               # Core game setup & configuration
│ ├── game.cpp          # Scene registration & first scene initialization
│ └── game_config.h     # Global settings ⚙️
│
├── assets/             # Asset management
│ ├── assets.cpp        # Helper to load assets
│ └── asset_config.h    # Texture names/paths 🎨
│
├── scenes/             # Game scenes 🌎
│ ├── main_menu.cpp
│ ├── swamp.cpp
│ └── ...               
│
├── prefabs/            # Reusable game objects
│ ├── ...               # Player, enemies, environment
│ └── weapon/           # If more prefabs are closely aligned, form a directory
│ └── ...               
│
├── behaviors/          # Gameplay logic 🛠️
│ ├── player/           # Player-related behaviors
│ │ └── ...
│ │
│ ├── ai/               # Enemy AI behaviors 🤖
│ │ └── ...
│ │
│ ├── weapon/           # Weapon behaviors ⚔️
│ │ └── ...
│ │
│ └── common/           # Shared behaviors 🩺
│ └── ...
│
├── util/               # Utility & helper classes 🔧
│ ├── timer.h           # Custom chrono timer ⏱️
│ └── json.h            # JSON (Nholmann) 📄
```

---

### 📝 Additional notes
- Keep **behaviors** logic-only; avoid mixing with prefabs.  
- Prefabs should define how an object is built and configured, not behavior.

---
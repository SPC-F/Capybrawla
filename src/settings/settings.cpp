#include <game/settings/settings.h>

#include <variant>
#include <random>
#include <engine/core/rendering/renderingService.h>
#include <engine/storage/simple_storage.h>
#include <engine/core/engine.h>

namespace settings {
    constexpr auto vsync_settings_key = "settings.vsync";
    constexpr auto username_key = "username";

    // 200 capitalized gamer-tag-style adjectives / modifiers
    static const std::vector<std::string> adjectives = {
        "Ashen","Static","Neon","Raw","Hollow","Cryptic","Blurred","Offset","Latent","Grim",
        "Faded","Noisy","Wired","Broken","Dusty","Cold","Sharp","Drifty","Twisted","Voided",
        "Fractured","Muted","Scarred","Eerie","Jaded","Toxic","Glitchy","Dark","Soft","Coarse",
        "Vivid","Sour","Flat","Deep","Shallow","Lofi","Hires","Cracked","Bent","Odd",
        "Feral","Urban","Primal","Rogue","Numb","Chaotic","Volatile","Shaky","Smoky","Icy",
        "Dry","Wet","Staticx","Grimdark","Warped","Compressed","Analog","Digital","Binary","Hexed",
        "Pixel","Synthetic","Organic","Neural","Kinetic","Thermal","Sonic","Silent","Loud","Rapid",
        "Slow","Lazy","Hyper","Turbo","Micro","Macro","Nano","Mega","Ultra","Proto","Iron",
        "Beta","Alpha","Delta","Omega","Prime","Null","Empty","Hazy","Blunt","Vague","Bat",
        "Clean","Dirty","Rusted","Polished","Rough","Smooth","Slick","Greasy","Sharpish","Softish",
        "Local","Remote","Global","Inner","Outer","Upper","Lower","Sideways","Inverted","Twilight",
        "Midnight","Dawn","Dusky","Solar","Lunar","Stellar","Cosmic","Orbital","Deepsea","Grounded",
        "Floating","Falling","Rising","Tilted","Skewed","Misaligned","Centered","Split","Merged","Stacked",
        "Buffered","Cached","Streamed","Queued","Looped","Forked","Threaded","Sandboxed","Virtual","Baremetal",
        "Cursed","Blessed","Tainted","Pure","Wild","Tame","Loose","Tight","Snapped","Stitched",
        "Stitchedx","Jittery","Flicker","Ghosted","Burned","Frozen","Thawed","Charged","Drained","Loaded",
        "Armed","Idle","Active","Passive","Hostile","Neutral","Friendly","Unknown","Hidden","Visible"
    };

    static const std::vector<std::string> nouns = {
        "Vector","Cipher","Signal","Kernel","Module","Process","Thread","Buffer","Stack","Heap",
        "Node","Sector","Zone","Realm","Core","Shell","Frame","Unit","Proxy","Avatar","Man",
        "Entity","Object","Agent","Operator","Hunter","Rogue","Scout","Sniper","Merc","Sentinel",
        "Ghost","Wraith","Specter","Shade","Lurker","Stalker","Watcher","Drifter","Nomad","Outlaw",
        "Relic","Artifact","Glyph","Sigil","Rune","Emblem","Crest","Mark","Scar","Trace",
        "Echo","Pulse","Wave","Noise","Static","Flux","Surge","Spark","Flare","Ember",
        "Void","Rift","Anomaly","Fracture","Shard","Fragment","Remnant","Residue","Husk","Vessel",
        "Form","Construct","Machine","Engine","Reactor","Circuit","Neuron","Synapse","Matrix","Grid",
        "Array","Index","Offset","Payload","Packet","Stream","Channel","Signalx","Vectorx","Null",
        "Zero","One","Prime","Root","Branch","Fork","Loop","Cycle","Clock","Timer",
        "Daemon","Bot","Drone","Mech","Android","Cyborg","Unitx","Framex","Shellx","Proxyx",
        "Archive","Record","Log","Traceback","Snapshot","Checkpoint","Save","Load","Spawn","Respawn",
        "Inventory","Drop","Loot","Stash","Cache","Vault","Chest","Key","Lock","Gate",
        "Portal","Door","Path","Route","Map","Radar","Scanner","Beacon","Marker","Ping",
        "Zonek","Sector7","Level","Stage","Instance","Session","Match","Round","Queue","Lobby",
        "Arena","Pit","Field","Range","Scope","Sight","Barrel","Trigger","Mag","Clip",
        "Blade","Edge","Point","Hammer","Anvil","Forge","Weapon","Tool","Device","Gadget",
        "Thing","Objectx","Formx","Shape","Mass","Weight","Density","Volume","Space","Time"
    };

    void apply(const settings &settings) {
        toggle_vsync(settings.vsync);
        register_username();
    }

    void apply_current_settings() {
        apply(get_settings());
    }

    void toggle_vsync(const bool enabled) {
        RenderingService& rendering_service = Engine::instance().services->try_get_service<RenderingService>().value();
        rendering_service.vsync(enabled);
        SimpleStorage::instance().set_value(vsync_settings_key, enabled);
        SimpleStorage::instance().save();
    }

    void register_username() {
        SimpleStorage& storage = SimpleStorage::instance();
        std::string username = storage.get_value_or_default<std::string>(username_key, "");

        if (username.empty()) {
            static std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<> adj_dist(0, adjectives.size() - 1);
            std::uniform_int_distribution<> noun_dist(0, nouns.size() - 1);
            
            username = adjectives[adj_dist(gen)] + nouns[noun_dist(gen)];
            storage.set_value(username_key, username);
            storage.save();
        }
    }

    settings get_settings() {
        SimpleStorage& storage = SimpleStorage::instance();
        const bool vsync = storage.get_value_or_default<int>(vsync_settings_key, true);

        return {
            .vsync = vsync
        };
    }
}

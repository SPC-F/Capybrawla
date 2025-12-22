#include <game/settings/settings.h>

#include <variant>
#include <engine/core/rendering/renderingService.h>
#include <engine/storage/simple_storage.h>
#include <engine/core/engine.h>

namespace settings {
    constexpr auto vsync_settings_key = "settings.vsync";

    void apply(const settings &settings) {
        toggle_vsync(settings.vsync);
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

    settings get_settings() {
        SimpleStorage& storage = SimpleStorage::instance();
        const bool vsync = storage.get_value_or_default<int>(vsync_settings_key, true);

        return {
            .vsync = vsync
        };
    }
}

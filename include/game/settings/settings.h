#pragma once

namespace settings {
    struct settings {
        bool vsync = true;
    };

    void apply(const settings& settings);
    void apply_current_settings();
    void toggle_vsync(bool enabled);
    void register_username();
    settings get_settings();

}

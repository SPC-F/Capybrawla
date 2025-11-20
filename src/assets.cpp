#include "assets.h"

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/core/rendering/renderingService.h>

void Assets::load_resources(std::vector<LoadResource>& resources) {
    Engine& engine = Engine::instance();
    auto& asset_service = engine.services->get_service<AssetService>().get();
    
    for (const auto& resource : resources) {
        asset_service.load_from_resource(
            resource.file,
            resource.name,
            resource.rows,
            resource.columns
        );
    }

    // asset_service.load_from_resource("swamp_background.png", "swamp_bg", 1, 1);
    // asset_service.load_from_resource("swamp_compact.png", "swamp_tiles", 9, 12);
}

void Assets::register_textures(std::vector<LoadTexture>& textures) {
    Engine& engine = Engine::instance();
    auto& asset_service = engine.services->get_service<AssetService>().get();

    for (const auto& texture : textures) {
        if (texture.asset_name.empty() || texture.texture_name.empty()) {
            throw std::runtime_error("Texture asset name and texture name cannot be empty");
        }

        asset_service.register_texture(
            texture.asset_name,
            texture.texture_name,
            static_cast<size_t>(texture.index)
        );
    }

    // asset_service.register_texture("swamp_tiles", "grass_single", 24);

    // asset_service.register_texture("swamp_tiles", "grass_multi_top_left", 1);
    // asset_service.register_texture("swamp_tiles", "grass_multi_top", 2);
    // asset_service.register_texture("swamp_tiles", "grass_multi_top_right", 3);
    // asset_service.register_texture("swamp_tiles", "grass_multi_middle_left", 13);
    // asset_service.register_texture("swamp_tiles", "grass_multi_middle", 14);
    // asset_service.register_texture("swamp_tiles", "grass_multi_middle_right", 15);
    // asset_service.register_texture("swamp_tiles", "grass_multi_bottom_left", 25);
    // asset_service.register_texture("swamp_tiles", "grass_multi_bottom", 26);
    // asset_service.register_texture("swamp_tiles", "grass_multi_bottom_right", 27);
}
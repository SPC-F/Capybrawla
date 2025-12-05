#include <game/assets.h>

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
}
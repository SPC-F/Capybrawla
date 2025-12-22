#include <game/assets.h>

#include <engine/core/engine.h>
#include <engine/core/rendering/assetService.h>
#include <engine/audio/audio_service.h>
#include <engine/core/rendering/renderingService.h>

AssetService& get_asset_service() {
  const Engine &engine = Engine::instance();
  return engine.services->get_service<AssetService>().get();
}

AudioService& get_audio_service() {
  const Engine &engine = Engine::instance();
  return engine.services->get_service<AudioService>().get();
}

void Assets::load_resources(const std::vector<LoadResourceData> &resources) {
  auto &asset_service = get_asset_service();
  for (const auto &resource : resources) {
    asset_service.load_from_resource(resource.file, resource.name,
                                     resource.rows, resource.columns);
  }
}

void Assets::register_textures(const std::vector<LoadTexture> &textures) {
  auto &asset_service = get_asset_service();
  for (const auto &texture : textures) {
    if (texture.asset_name.empty() || texture.texture_name.empty()) {
      throw std::runtime_error(
          "Texture asset name and texture name cannot be empty");
    }

    asset_service.register_texture(texture.asset_name, texture.texture_name,
                                   static_cast<size_t>(texture.index));
  }
}

void Assets::register_sprite_sheets(const std::vector<LoadAnimation> &animations) {
  auto &asset_service = get_asset_service();

  for (const auto &animation : animations) {
    if (animation.resource_name.empty() || animation.animation_name.empty()) {
      throw std::runtime_error(
          "Animation resource name and animation name cannot be empty");
    }

    asset_service.create_spritesheet_for(
        animation.resource_name, animation.animation_name,
        static_cast<size_t>(animation.start_frame),
        static_cast<size_t>(animation.frame_count));
  }
}

void Assets::register_audio(const std::vector<LoadAudio> &audios) {
  auto &audio_service = get_audio_service();

  for (const auto &audio : audios) {
    if (audio.path.empty() || audio.name.empty()) {
      throw std::runtime_error(
          "Audio resource path and name cannot be empty");
    }

    audio_service.register_sound(
        audio.path,
        audio.name,
        audio.type);
  }
}
#pragma once
#include <cmath>
#include <engine/public/behavior.h>

#include <lib/timer.h>

#include <engine/public/scene.h>
#include <engine/public/ui/ui_text.h>
#include <engine/public/util/layers.h>

class RoundTimer final : public Behavior
{
public:
    using timer_elapsed_cb_t = std::function<void()>;

    RoundTimer() : text_(std::nullopt)
    {
    }

    void on_start() override
    {
        create_timer_label();
    }

    void on_update(float dt) override
    {
        text_.value().get().text(timer_.formatted_remaining_time());

        if (timer_.remaining_time() <= 10)
        {
            pulse_time_ += dt;

            // Pulse effect
            const float scale_factor = 96.0f + 48.0f * std::abs(std::sin(pulse_time_));
            text_.value().get().font_size(scale_factor);
            text_.value().get().color({255, 0, 0, 255});
        }
        else
        {
            text_.value().get().font_size(96.0f);
        }
    }

    void start_timer(const timer_elapsed_cb_t& timer_elapsed_cb)
    {
        timer_elapsed_cb_ = timer_elapsed_cb;
        timer_.start();
    }

private:
    const int round_duration_seconds_ = 70;
    lib::Timer timer_{round_duration_seconds_};
    timer_elapsed_cb_t timer_elapsed_cb_;
    std::optional<std::reference_wrapper<UIText>> text_;
    float pulse_time_ = 0.0f;

    void create_timer_label()
    {
        auto& scene = game_object().scene();
        text_ = scene.add_game_object<UIText>(
            scene,
            timer_.formatted_remaining_time(),
            "ByteBounce",
            "resources/fonts/bytebounce/ByteBounce.ttf",
            500,
            100,
            Point{0.5f, 0.5f},
            Point{0.5f, 0.5f}
        );
        text_.value().get().transform().position({1920 / 2.0f - 500 / 2.0f, 0.0f, 0.0f}); // Horizontal center
        text_.value().get().color({255, 255, 255, 255});
        text_.value().get().font_size(96);
        text_.value().get().layer(Layers::UI);
    }
};

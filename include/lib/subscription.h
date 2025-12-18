#pragma once

#include <functional>
#include <string>
#include <utility>

namespace lib {

    template <typename... Args>
    class Signal {
    private:
        std::string id_;
        std::function<void(Args...)> fn_;
    public:
        Signal(std::string id, std::function<void(Args...)> fn): id_(std::move(id)), fn_(fn) {}
        [[nodiscard]] const std::string& id() const { return id_; }
        void envoke(Args... args) const {
            fn_(args...);
        }
    };

    class Subscription {
    private:
        std::function<void()> unsubscribe_;
    public:
        Subscription() noexcept = default;

        explicit Subscription(std::function<void()> unsubscribe);

        Subscription(Subscription&& other) noexcept;

        Subscription& operator=(Subscription&& other) noexcept;

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        ~Subscription();

        void reset();

        explicit operator bool() const noexcept;
    };
}

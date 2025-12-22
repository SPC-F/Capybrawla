#include <utility>
#include <lib/subscription.h>

namespace lib {
    Subscription::Subscription(std::function<void()> unsubscribe, std::weak_ptr<void> emitter_validity_token)
          : unsubscribe_(std::move(unsubscribe)), emitter_validity_token_(std::move(emitter_validity_token)) {}

    Subscription::Subscription(Subscription&& other) noexcept
          : unsubscribe_(std::exchange(other.unsubscribe_, {})) {}

    Subscription& Subscription::operator=(Subscription&& other) noexcept {
        if (this != &other) {
            reset();
            unsubscribe_ = std::exchange(other.unsubscribe_, {});
            emitter_validity_token_ = std::move(other.emitter_validity_token_);
        }
        return *this;
    }

    Subscription::~Subscription()  {
        reset();
    }

    bool Subscription::expired() const {
        return emitter_validity_token_.expired();
    }

    void Subscription::reset() {
        if (unsubscribe_ && !emitter_validity_token_.expired()) {
            unsubscribe_();
            unsubscribe_ = {};
            emitter_validity_token_.reset();
        }
    }

    Subscription::operator bool() const noexcept {
        return !emitter_validity_token_.expired() && static_cast<bool>(unsubscribe_);
    }
}

#include <utility>
#include <lib/subscription.h>

namespace lib {
    Subscription::Subscription(std::function<void()> unsubscribe)
          : unsubscribe_(std::move(unsubscribe)) {}
    Subscription::Subscription(Subscription&& other) noexcept
          : unsubscribe_(std::exchange(other.unsubscribe_, {})) {}

    Subscription& Subscription::operator=(Subscription&& other) noexcept {
        if (this != &other) {
            reset();
            unsubscribe_ = std::exchange(other.unsubscribe_, {});
        }
        return *this;
    }

    Subscription::~Subscription()  {
        reset();
    }

    void Subscription::reset() {
        if (unsubscribe_) {
            unsubscribe_();
            unsubscribe_ = {};
        }
    }

    Subscription::operator bool() const noexcept {
        return static_cast<bool>(unsubscribe_);
    }
}

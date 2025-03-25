#ifndef __RACE_DEFER_H__
#define __RACE_DEFER_H__

#include <utility>  // std::move

template <class F>
class Defer {
public:
    explicit Defer(const F &_callback) : callback(_callback) {}
    explicit Defer(F &&_callback) : callback(std::move(_callback)) {}
    ~Defer() {
        callback();
    }

    Defer(const Defer &) = delete;
    Defer(Defer &&) = delete;
    Defer &operator=(const Defer &) = delete;
    Defer &operator=(Defer &&) = delete;

private:
    F callback;
};

#endif

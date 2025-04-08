#ifndef TA2_TWOSIX_USER_MODEL_MARKOV_MODEL_H
#define TA2_TWOSIX_USER_MODEL_MARKOV_MODEL_H

#include <boost/numeric/ublas/matrix.hpp>
#include <random>

class MarkovModel {
public:
    enum class UserAction {
        FETCH,
        POST,
        WAIT,
    };

    UserAction getNextUserAction();

protected:
    int currentState{0};

    virtual double getRandom();

private:
    std::random_device rd;
    std::mt19937 gen{rd()};
};

#endif  // __TA2_TWOSIX_USER_MODEL_MARKOV_MODEL_H__

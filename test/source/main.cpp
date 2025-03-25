#include "RaceLog.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
    RaceLog::setLogLevelStdout(RaceLog::LL_DEBUG);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#ifndef __TA2_TWOSIX_TRANSPORT_MOCK_LINK_H__
#define __TA2_TWOSIX_TRANSPORT_MOCK_LINK_H__

#include <gmock/gmock.h>

#include "Link.h"

class MockLink : public Link {
public:
    using Link::Link;

    MOCK_METHOD(ComponentStatus, enqueueContent,
                (uint64_t actionId, const std::vector<uint8_t> &content), (override));
    MOCK_METHOD(ComponentStatus, dequeueContent, (uint64_t actionId), (override));
    MOCK_METHOD(ComponentStatus, fetch, (std::vector<RaceHandle> handles), (override));
    MOCK_METHOD(ComponentStatus, post, (std::vector<RaceHandle> handles, uint64_t actionId),
                (override));
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, shutdown, (), (override));
};

#endif  // __TA2_TWOSIX_TRANSPORT_MOCK_LINK_H__
#include <gtest/gtest.h>
#include <race/mocks/MockTransportSdk.h>

#include "LinkMap.h"

class TestLinkMap : public ::testing::Test {
public:
    MockTransportSdk sdk;
    LinkMap map;

    std::shared_ptr<Link> createLink(const std::string &linkId) {
        return std::make_shared<Link>(linkId, LinkAddress(), LinkProperties(), &sdk);
    }
};

TEST_F(TestLinkMap, size) {
    map.add(createLink("LinkID_1"));
    ASSERT_EQ(1, map.size());
    map.add(createLink("LinkID_2"));
    ASSERT_EQ(2, map.size());
    map.clear();
    ASSERT_EQ(0, map.size());
}

TEST_F(TestLinkMap, get) {
    ASSERT_THROW(map.get("LinkID_3"), std::out_of_range);
    map.add(createLink("LinkID_3"));
    ASSERT_NE(nullptr, map.get("LinkID_3"));
    ASSERT_THROW(map.get("LinkID_4"), std::out_of_range);
}

TEST_F(TestLinkMap, remove) {
    ASSERT_EQ(nullptr, map.remove("LinkID_5"));
    map.add(createLink("LinkID_5"));
    ASSERT_NE(nullptr, map.remove("LinkID_5"));
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "JsonTypes.h"
#include "LinkUserModel.h"

class LinkUserModelToTest : public LinkUserModel {
public:
    using LinkUserModel::LinkUserModel;
    MOCK_METHOD(MarkovModel::UserAction, getNextUserAction, (), (override));
};

class TestLinkUserModel : public ::testing::Test {
public:
    std::atomic<uint64_t> nextActionId{0};
    LinkUserModelToTest model{"LinkID", nextActionId};
    ActionTimeline timeline;

    void verifyAction(size_t index, Timestamp expectedTimestamp, uint64_t expectedActionId,
                      ActionType expectedAction) {
        auto &action = timeline.at(index);
        EXPECT_NEAR(expectedTimestamp, action.timestamp, 0.001) << "for action at index " << index;
        EXPECT_EQ(expectedActionId, action.actionId) << "for action at index " << index;
        ActionJson actionJson = nlohmann::json::parse(action.json);
        EXPECT_EQ("LinkID", actionJson.linkId) << "for action at index " << index;
        EXPECT_EQ(expectedAction, actionJson.type) << "for action at index " << index;
    }
};

TEST_F(TestLinkUserModel, should_generate_single_timeline) {
    EXPECT_CALL(model, getNextUserAction())
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))  // 1000
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))   // 1001
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1002
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1003
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT));  // 1004

    timeline = model.getTimeline(1000.0, 1004.0);
    ASSERT_EQ(4, timeline.size());
    verifyAction(0, 1000.0, 1, ACTION_FETCH);
    verifyAction(1, 1001.0, 2, ACTION_FETCH);
    verifyAction(2, 1001.0, 3, ACTION_POST);
    verifyAction(3, 1003.0, 4, ACTION_POST);
}

TEST_F(TestLinkUserModel, should_generate_non_overlapping_timelines_without_any_cached_actions) {
    EXPECT_CALL(model, getNextUserAction())
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))  // 1000
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))   // 1001
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1002
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1003
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT));  // 1004

    timeline = model.getTimeline(1000.0, 1002.0);
    ASSERT_EQ(3, timeline.size());
    verifyAction(0, 1000.0, 1, ACTION_FETCH);
    verifyAction(1, 1001.0, 2, ACTION_FETCH);
    verifyAction(2, 1001.0, 3, ACTION_POST);

    timeline = model.getTimeline(1002.0, 1004.0);
    ASSERT_EQ(1, timeline.size());
    verifyAction(0, 1003.0, 4, ACTION_POST);
}

TEST_F(TestLinkUserModel, should_generate_overlapping_timelines_with_cached_actions) {
    EXPECT_CALL(model, getNextUserAction())
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))  // 1000
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))   // 1001
        .WillOnce(::testing::Return(MarkovModel::UserAction::FETCH))
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1002
        // 2nd time it's called, this wait will apply to time=1001 again
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))  // 1002
        .WillOnce(::testing::Return(MarkovModel::UserAction::POST))
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT))   // 1003
        .WillOnce(::testing::Return(MarkovModel::UserAction::WAIT));  // 1004

    timeline = model.getTimeline(1000.0, 1002.0);
    ASSERT_EQ(3, timeline.size());
    verifyAction(0, 1000.0, 1, ACTION_FETCH);
    verifyAction(1, 1001.0, 2, ACTION_FETCH);
    verifyAction(2, 1001.0, 3, ACTION_POST);

    timeline = model.getTimeline(1001.0, 1004.0);
    ASSERT_EQ(3, timeline.size());
    verifyAction(0, 1001.0, 2, ACTION_FETCH);
    verifyAction(1, 1001.0, 3, ACTION_POST);
    verifyAction(2, 1002.0, 4, ACTION_POST);
}
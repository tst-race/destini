#ifndef __RACE_CHANNEL_ROLE_H_
#define __RACE_CHANNEL_ROLE_H_

#include <string>
#include <vector>

enum LinkSide {
    LS_UNDEF = 0,
    LS_CREATOR = 1,
    LS_LOADER = 2,
    LS_BOTH = 3,
};

struct ChannelRole {
    ChannelRole() : linkSide(LS_UNDEF) {}

    std::string roleName;
    std::vector<std::string> mechanicalTags;
    std::vector<std::string> behavioralTags;
    LinkSide linkSide;
};

inline bool operator==(const ChannelRole &a, const ChannelRole &b) {
    return a.roleName == b.roleName && a.mechanicalTags == b.mechanicalTags &&
           a.behavioralTags == b.behavioralTags && a.linkSide == b.linkSide;
}
inline bool operator!=(const ChannelRole &a, const ChannelRole &b) {
    return !(a == b);
}

/**
 * @brief Convert a LinkSide value to a human readable string.
 *
 * @param LinkSide The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string linkSideToString(LinkSide linkSide);
LinkSide linkSideFromString(const std::string &linkSideString);

std::ostream &operator<<(std::ostream &out, LinkSide linkSide);

/**
 * @brief Convert a ChannelRole value to a human readable string. This function is strictly for
 * logging and debugging. The output formatting may change without notice. Do NOT use this for any
 * logical comparisons, etc. The functionality of your plugin should in no way rely on the output of
 * this function.
 *
 * @param ChannelRole The value to convert to a string.
 * @return std::string The string representation of the provided value.
 */
std::string channelRoleToString(const ChannelRole &channelRole);

#endif

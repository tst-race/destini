#include "LinkAddress.h"

//
// First pass:  we'll encode the user within the linkaddress hashtag.
//

void to_json(nlohmann::json &destJson, const LinkAddress &srcLinkAddress) {
    destJson = nlohmann::json{
        // clang-format off
        {"hashtag", srcLinkAddress.hashtag},
        {"hostname", srcLinkAddress.hostname},
        {"port", srcLinkAddress.port},
        {"maxTries", srcLinkAddress.maxTries},
        {"timestamp", srcLinkAddress.timestamp},
        {"user", srcLinkAddress.user}
        // clang-format on
    };
}

void from_json(const nlohmann::json &srcJson, LinkAddress &destLinkAddress) {
    // Required
    srcJson.at("hashtag").get_to(destLinkAddress.hashtag);
    // Optional
    destLinkAddress.hostname = srcJson.value("hostname", destLinkAddress.hostname);
    destLinkAddress.port = srcJson.value("port", destLinkAddress.port);
    destLinkAddress.maxTries = srcJson.value("maxTries", destLinkAddress.maxTries);
    destLinkAddress.timestamp = srcJson.value("timestamp", destLinkAddress.timestamp);
    destLinkAddress.user = srcJson.value("user", destLinkAddress.user);
}

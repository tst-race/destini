#ifndef __RACE_PLUGIN_EXPORTS_H_
#define __RACE_PLUGIN_EXPORTS_H_

#include "RaceExport.h"

struct RaceVersionInfo {
    int major, minor, compatibility;
};

constexpr bool operator==(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major == b.major && a.minor == b.minor && a.compatibility == b.compatibility;
}
constexpr bool operator!=(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major != b.major || a.minor != b.minor || a.compatibility != b.compatibility;
}
constexpr bool operator<(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major != b.major ?
               a.major < b.major :
               a.minor != b.minor ? a.minor < b.minor : a.compatibility < b.compatibility;
}
constexpr bool operator<=(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major != b.major ?
               a.major < b.major :
               a.minor != b.minor ? a.minor < b.minor : a.compatibility <= b.compatibility;
}
constexpr bool operator>(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major != b.major ?
               a.major > b.major :
               a.minor != b.minor ? a.minor > b.minor : a.compatibility > b.compatibility;
}
constexpr bool operator>=(const RaceVersionInfo &a, const RaceVersionInfo &b) {
    return a.major != b.major ?
               a.major > b.major :
               a.minor != b.minor ? a.minor > b.minor : a.compatibility >= b.compatibility;
}

#define RACE_VERSION (RaceVersionInfo{2, 4, 0})

extern "C" EXPORT const RaceVersionInfo raceVersion;
extern "C" EXPORT const char *const racePluginId;
extern "C" EXPORT const char *const racePluginDescription;

#endif

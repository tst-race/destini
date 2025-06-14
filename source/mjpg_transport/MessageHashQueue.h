#ifndef TA2_TWOSIX_TRANSPORT_MESSAGE_HASH_QUEUE_H
#define TA2_TWOSIX_TRANSPORT_MESSAGE_HASH_QUEUE_H

#include <cinttypes>
#include <deque>
#include <string>
#include <vector>

class MessageHashQueue {
public:
    std::size_t addMessage(const std::string &message);
    void removeHash(std::size_t hash);
    bool findAndRemoveMessage(const std::string &message);

private:
    static const std::deque<std::size_t>::size_type max{1024};
    static std::size_t hash(const std::string &message);

    std::deque<std::size_t> queue;
};

#endif  // __TA2_TWOSIX_TRANSPORT_MESSAGE_HASH_QUEUE_H__

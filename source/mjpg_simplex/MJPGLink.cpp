#include "Link.h"

#include <ITransportSdk.h>
#include <base64.h>

#include <chrono>
#include <nlohmann/json.hpp>

#include "PersistentStorageHelpers.h"
// #include "curlwrap.h"

#include "mjpg_transport.h"

#include <log.h>

#define TRACE_SRI_METHOD(...) TRACE_METHOD_BASE(PluginTA2SRI_MJPG_, ##__VA_ARGS__)
#define TRACE_SRI_FUNCTION(...) TRACE_FUNCTION_BASE(PluginTA2SRI_MJPG_, ##__VA_ARGS__)

static const size_t ACTION_QUEUE_MAX_CAPACITY = 10;

// extern std::string getCurrentTags2(long);

namespace std {
static std::ostream &operator<<(std::ostream &out, const std::vector<RaceHandle> &handles) {
    return out << nlohmann::json(handles).dump();
}
}  // namespace std

std::string replace_all(std::string s,  uint8_t c1, uint8_t c2) {
  uint32_t l = s.length();
 
  // loop to traverse in the string
  for (uint32_t i = 0; i < l; i++) 
    {
      // check for c1 and replace
      if ( s[i] == char(c1) )
	s[i] = char(c2);
 
      // check for c2 and replace
      else if (s[i] == char(c2) )
	s[i] = char(c1);
    }
  return s;
}
 
//Link::Link(LinkID linkId, LinkAddress address, LinkProperties properties, ITransportSdk *sdk, const DynamicWords& dw) :
Link::Link(LinkID linkId, LinkAddress address, LinkProperties properties, ITransportSdk *sdk) :
  //    dynamic_words(dw),
    sdk(sdk),
    linkId(std::move(linkId)),
    address(std::move(address)),
    properties(std::move(properties))
{
    logDebug("MJPGLink: in Link::Link");
    this->properties.linkAddress = nlohmann::json(this->address).dump();
}

Link::~Link() {
    TRACE_SRI_METHOD(linkId);
    shutdown();
}

LinkID Link::getId() const {
    return linkId;
}

const LinkProperties &Link::getProperties() const {
    return properties;
}

ComponentStatus Link::enqueueContent(uint64_t actionId, const std::vector<uint8_t> &content) {
    TRACE_SRI_METHOD(linkId, actionId);
    logDebug("MJPGLink: in enqueueContent");
    {
        std::lock_guard<std::mutex> lock(mutex);
        contentQueue[actionId] = content;
    }
    logDebug("MJPGLink: returning from enqueueContent");
    return COMPONENT_OK;
}

ComponentStatus Link::dequeueContent(uint64_t actionId) {
    TRACE_SRI_METHOD(linkId, actionId);
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto iter = contentQueue.find(actionId);
        if (iter != contentQueue.end()) {
            contentQueue.erase(iter);
        }
    }
    return COMPONENT_OK;
}

ComponentStatus Link::fetch(std::vector<RaceHandle> handles) {
    TRACE_SRI_METHOD(linkId, handles);
    logDebug("MJPGLink: In fetch");


    if (isShutdown) {
        logError(logPrefix + "link has been shutdown: " + linkId);
        return COMPONENT_ERROR;
    }

    std::lock_guard<std::mutex> lock(mutex);

    if (actionQueue.size() >= ACTION_QUEUE_MAX_CAPACITY) {
        logError(logPrefix + "action queue full for link: " + linkId);
        return COMPONENT_ERROR;
    }

    actionQueue.push_back({false, std::move(handles), 0});
    conditionVariable.notify_one();
    return COMPONENT_OK;
}

ComponentStatus Link::post(std::vector<RaceHandle> handles, uint64_t actionId) {
    TRACE_SRI_METHOD(linkId, handles, actionId);

    logDebug("MJPGLink: In post");
    if (isShutdown) {
        logError(logPrefix + "link has been shutdown: " + linkId);
        return COMPONENT_ERROR;
    }

    std::lock_guard<std::mutex> lock(mutex);

    logDebug("MJPGLink: In post checking action queue capacity");
    if (actionQueue.size() >= ACTION_QUEUE_MAX_CAPACITY) {
        logError(logPrefix + "action queue full for link: " + linkId);
        return COMPONENT_ERROR;
    }

    logDebug("MJPGLink: In post checking queue");
    if (contentQueue.find(actionId) == contentQueue.end()) {
        // TODO: what's the correct log level. We want it to be an error(?) for performer encodings,
        // but this is expected for our own ta2.
        logInfo(logPrefix + "no enqueued content for given action ID: " + std::to_string(actionId));
        updatePackageStatus(handles, PACKAGE_FAILED_GENERIC);
        return COMPONENT_OK;
    }
    
    logDebug("MJPGLink: In post queue pushback");
    actionQueue.push_back({true, std::move(handles), actionId});
    logDebug("MJPGLink: In post notify_one");
    conditionVariable.notify_one();
    return COMPONENT_OK;
}

void Link::start() {
  std::string video;
    TRACE_SRI_METHOD(linkId);
    // address.port will be the port on which the STREAMER is sending
    // video.

    ChannelProperties p = sdk->getChannelProperties();
    LinkDirection dir = p.linkDirection;
    if (dir == LD_CREATOR_TO_LOADER)
      logDebug(logPrefix + " Link::start() LD_CREATOR_TO_LOADER");
    else if (dir == LD_LOADER_TO_CREATOR)
      logDebug(logPrefix + " Link::start() LD_LOADER_TO_CREATOR");
    else if (dir == LD_UNDEF)
      logDebug(logPrefix + " Link::start() LD_UNDEF");
    else if (dir == LD_BIDI)
      logDebug(logPrefix + " Link::start() LD_BIDI");
    else logDebug(logPrefix + " Link::start() Unhandled link direction?");
      
    // address.hostname should be an IP:PORT combo that will be used
    // to LISTEN to a remote stream.
  
    // Should rethink this - what makes sense as an address for MJPG?
    streamer_port = std::to_string(address.port);
    client_url = "http://" + address.hostname + "/streamer";

    if (address.port == 8080) {
      video = "/root/veil/videos/husky.mjpg";
    } else {
      video = "/root/veil/videos/osaka-full.mjpg";
    }
    logDebug(logPrefix + "     Server is streaming video " + video);
    
#if 1
    // Proper logic for simplex operation:
    if (dir == LD_CREATOR_TO_LOADER) {
      logDebug(logPrefix + " Link::start() starting streamer on port " + streamer_port);
      streamer_pid = startMJPGServer( strdup(video.c_str()), strdup(streamer_port.c_str()) );
      client_pid = -1;
      logDebug(logPrefix + " Link::start() LD_CREATOR_TO_LOADER, startMJPGServer returns " + std::to_string(streamer_pid));
    } else {
      streamer_pid = -1;
      logDebug(logPrefix + " Link::start() starting client with url " + client_url);
      client_pid = startMJPGClient( strdup(client_url.c_str()) );
      logDebug(logPrefix + " Link::start() startMJPGClient returns " + std::to_string(client_pid));
    }
#else
    streamer_pid = startMJPGServer( strdup(video.c_str()), strdup(streamer_port.c_str()) );
    client_pid = startMJPGClient( strdup(client_url.c_str()) );
#endif    
    logDebug(logPrefix + " Link::start() => Here is where we would start a client-server pair.");
    logDebug(logPrefix + "                  Seems like we might need matching pairs for each link!.");
    logDebug(logPrefix + " Link::start invokes runActionThread");

    thread = std::thread(&Link::runActionThread, this);
}

void Link::shutdown() {
    TRACE_SRI_METHOD(linkId);
    // Possible stream kill point.
    // Don't shut it down yet.  Do this manually for demo purposes:
    // int rc = stopMJPGServer(streamer_pid);

    //    logDebug(logPrefix + " Link::shutdown() => stopMJPGServer returns." + std::to_string(rc));
    logDebug(logPrefix + " Link::shutdown() but did not call stopMJPGServer.");
    isShutdown = true;
    conditionVariable.notify_one();
    if (thread.joinable()) {
        thread.join();
    }
}

void Link::runActionThread() {
    TRACE_SRI_METHOD(linkId);
    logPrefix += linkId + ": ";
    logDebug(logPrefix + " starting runActionThread");

    int latest = getInitialIndex();

    while (not isShutdown) {
        std::unique_lock<std::mutex> lock(mutex);
        conditionVariable.wait(lock, [this] { return isShutdown or not actionQueue.empty(); });
	logDebug(logPrefix + " runActionThread in while loop");
	  
        if (isShutdown) {
            logDebug(logPrefix + "shutting down");
            break;
        }

        auto action = actionQueue.front();
        actionQueue.pop_front();

        if (action.post) {
	  logDebug(logPrefix + " runActionThread calling postOnActionThread");
	  postOnActionThread(action.handles, action.actionId);
        } else {
            latest = fetchOnActionThread(latest);
        }
    }
    logDebug(logPrefix + " shutting down runActionThread");

}

int Link::getInitialIndex() {
    TRACE_SRI_METHOD(linkId);
    logPrefix += linkId + ": ";

    // TODO check link hints for timestamp
    double timestamp = psh::readValue(sdk, prependIdentifier("lastTimestamp"), -1.0);
    if (timestamp > 0) {
        logDebug(logPrefix + "using last recorded timestamp: " + std::to_string(timestamp));
    } else if (address.timestamp <= 0) {
        std::chrono::duration<double> sinceEpoch =
            std::chrono::high_resolution_clock::now().time_since_epoch();
        timestamp = sinceEpoch.count();
        logDebug(logPrefix + "using now for timestamp: " + std::to_string(timestamp));
    } else {
        timestamp = address.timestamp;
        logDebug(logPrefix + "using address timestamp: " + std::to_string(timestamp));
    }

    return getIndexFromTimestamp(timestamp);
}


/* This is trying to get new content from the stream, but the
   exceptions listed here are probably irrelevant (e.g., we're not
   curling here). */
int Link::fetchOnActionThread(int latestIndex) {
    TRACE_SRI_METHOD(linkId, latestIndex);
    logPrefix += linkId + ": ";

    try {
      /* The guts: get new "posts", i.e., streamed steg conent: */
        auto [posts, newLatestIndex, serverTimestamp] = getNewPosts(latestIndex);

        int numPosts = static_cast<int>(posts.size());
        if (numPosts < newLatestIndex - latestIndex) {
            logError(logPrefix + "expected " + std::to_string(newLatestIndex - latestIndex) +
                     " posts, but only got " + std::to_string(numPosts) + ". " +
                     std::to_string(newLatestIndex - latestIndex - numPosts) +
                     " posts may have been lost.");
        }

	// Message has already been retrieved and placed on the
	// "incoming" queue.  I think.  -CC
        for (const auto &post : posts) {
            if (postedMessageHashes.findAndRemoveMessage(post)) {
                logDebug(logPrefix + "received post from self, ignoring");
            } else {
                logDebug(logPrefix + "received encrypted package");
		std::string post2 = replace_all(post, uint8_t('='), uint8_t('+'));
		std::vector<uint8_t> message = base64::decode(post2);
                sdk->onReceive(linkId, {linkId, "*/*", false, {}}, message);
            }
        }

        if (numPosts > 0) {
            psh::saveValue(sdk, prependIdentifier("lastTimestamp"), serverTimestamp);
        }

        fetchAttempts = 0;

        return newLatestIndex;
	//    } catch (curl_exception &error) {
	//        logError(logPrefix + "curl exception: " + std::string(error.what()));
    } catch (nlohmann::json::exception &error) {
        logError(logPrefix + "json exception: " + std::string(error.what()));
    } catch (std::exception &error) {
        logError(logPrefix + "std exception: " + std::string(error.what()));
    }

    ++fetchAttempts;
    if (fetchAttempts >= address.maxTries) {
        logError(logPrefix + "Retry limit reached. Giving up.");
        sdk->updateState(COMPONENT_STATE_FAILED);
    }

    return latestIndex;
}

/**
 * @brief callback function required by libcurl-dev.
 * See documentation in link below:
 * https://curl.haxx.se/libcurl/c/libcurl-tutorial.html
 */
#if 0
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    (static_cast<std::string *>(userp))->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}
#endif

int Link::getIndexFromTimestamp(double secondsSinceEpoch) {
    TRACE_SRI_METHOD(linkId, secondsSinceEpoch);
    logPrefix += linkId + ": ";
    logDebug(logPrefix + "in Link:getIndexFromTimestamp");

    logDebug(logPrefix + "in Link:getIndexFromTimestamp, avoiding curl and returning 0.");
    int index = 0;
    return index;
}

// ================================================================================
//
// Legacy misnomer: Retrieves a message from the "whiteboard" and
// places it on an "incoming" queue.  In this case, the "whiteboard"
// is the incoming motion jpeg stream:

std::tuple<std::vector<std::string>, int, double> Link::getNewPosts(int latestIndex) {
    std::thread::id threadID = thread.get_id();
    TRACE_SRI_METHOD(linkId, latestIndex);
    logPrefix += linkId + ": ";
    std::vector<std::string> v;
    std::vector<std::string> r;
    // char ** getMJPGPost(int identifier, const char *user);
    int i;
    logInfo(logPrefix + "in getNewPosts, processing index " + std::to_string(latestIndex));

    //    const char *tags = DEFAULT_TAGS;
    
    //    logInfo(logPrefix + "in getNewPosts taglist = " + tags);
    const char *user = address.user.c_str();

    // Modify this call to extract the user - Can we allow
    // getMJPGPost to take a user arg?  address.user or parse
    // address.hashtag?  Also, DEFAULT_TAGS needs to be a DynamicWords
    // tag list:

    logInfo(logPrefix + "in getNewPosts, selected user is " + address.user);
    //    logInfo(logPrefix + "in getNewPosts, tags list is " + tags);

    // Beware of massive memory leaks until I get around to recovery:
    //    std::unique_lock<std::mutex> lock(mutex);
    std::cout << "getNewPosts" << logPrefix << "in getNewPosts =================================  Thread: " << threadID << "\n";

    // Now collect posts that correspond to the three tags lists -
    // this will cover a 30-minute period:
    
    char **resp = getMJPGPost(latestIndex, user);
    //    lock.unlock();

    int k = 0;
    for (i = 0; resp[i]; i++) {
      std::string s = std::string(resp[i]);
      v.push_back(s);
      k++;
    }
      
    resp = getMJPGPost(latestIndex, user);
    //    lock.unlock();

    if (resp) {
      for (i = 0; resp[i]; i++) {
	std::string s = std::string(resp[i]);
	v.push_back(s);
	k++;
      }
      // if 0 only for testing.  Need to implement get and put for motion jpeg stream.
#if 0
      resp = getMJPGPost64(latestIndex, user);
      //    lock.unlock();

      for (i = 0; resp[i]; i++) {
	std::string s = std::string(resp[i]);
	v.push_back(s);
	k++;
      }
#endif
    }

    std::cout << "getNewPosts" << logPrefix << "Returning from getNewPosts\n";
    
    return {v, k, 0};
}



void Link::postOnActionThread(const std::vector<RaceHandle> &handles, uint64_t actionId) {
    TRACE_SRI_METHOD(linkId, handles, actionId);
    logPrefix += linkId + ": ";
    logDebug(logPrefix + "in postOnActionThread");

    auto iter = contentQueue.find(actionId);
    if (iter == contentQueue.end()) {
        // We really shouldn't get here, since we already check for this before queueing the action,
        // but just in case...
        logError(logPrefix +
                 "no enqueued content for given action ID: " + std::to_string(actionId));
        updatePackageStatus(handles, PACKAGE_FAILED_GENERIC);
        return;
    }

    std::string message = base64::encode(iter->second);
    auto msgHash = postedMessageHashes.addMessage(message);

    // Try to post the message, up to maxTries attempts:
    int tries = 0;
    for (; tries < address.maxTries; ++tries) {
      // Misnomer here, in that we are "posting" to a motion jpeg stream:
        if (postToWhiteboard(message)) {
	    logInfo(logPrefix + "===  postToWhiteboard successful! ! !");
            break;
        }
    }

    if (tries == address.maxTries) {
        logError(logPrefix + "retry limit exceeded: post failed");
        postedMessageHashes.removeHash(msgHash);
        updatePackageStatus(handles, PACKAGE_FAILED_GENERIC);
    } else {
        updatePackageStatus(handles, PACKAGE_SENT);
    }
    logInfo(logPrefix + "postOnActionThread exiting...");
}

void Link::updatePackageStatus(const std::vector<RaceHandle> &handles, PackageStatus status) {
    for (auto &handle : handles) {
        sdk->onPackageStatusChanged(handle, status);
    }
}

// ================================================================================
// POSTING - Legacy misnomer, but perhaps useful for thinking about
// MJPEG transport.  Instead of posting to Flickr, we throw the
// message to stcp, which will prepare it for insertion into the mjpg
// stream:

bool Link::postToWhiteboard(const std::string &message) {
  //    std::thread::id threadID = thread.get_id();
  // int  putMJPGPost(const char *msg, const char *user);
    TRACE_SRI_METHOD(linkId);
    logPrefix += linkId + ": ";
    bool success = false;

    //    std::string tagstring = getCurrentTags2(0);
    //    const char *tags = tagstring.c_str();

    logDebug(logPrefix + "in postToWhiteboard, about to call putMJPGPost");

    // #if OLD_CODE - see Link.cpp for the old code.  As above with
    // getNewPosts, we want to pass address.user to the MJPG API for
    // authentication:
    logInfo(logPrefix + "in postToWhiteboard, selected user is " + address.user);
    //    logInfo(logPrefix + "in postToWhiteboard, tags list is " + tags);

    // std::unique_lock<std::mutex> lock(mutex);
    // std::cout << "postToWhiteboard" << logPrefix << "in postToWhiteboard =================================  Thread: " << threadID << "\n";
    // if 0 just for testing compilation:
    success = putMJPGPost(1, strdup(message.c_str()), message.length(), address.user.c_str());

    // lock.unlock();

    logDebug(logPrefix + "in postToWhiteboard, putMJPGPost returns" + std::to_string(success));
    return success;
}

std::string Link::prependIdentifier(const std::string &key) {
    return key + ":" + address.hostname + ":" + std::to_string(address.port) + ":" +
      address.hashtag;
}

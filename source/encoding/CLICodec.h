#ifndef CLICODEC_H

#include <cstdint>
#include <string>
#include <vector>

#include <event2/event.h>

#ifdef __ANDROID__
#include <json/json.h>
#else
#include <jsoncpp/json/json.h>
#endif

bool fileExists (std::string path);


class MediaPath
{
 private:
  std::string _mediaPath;
  size_t      _capacity;

 public:
  MediaPath (const std::string &path, size_t capacity);

  std::string path ()     {return _mediaPath;};
  size_t      capacity () {return _capacity;};
};

typedef MediaPath *MediaPathPtr;


class MediaPaths
{
 private:
  std::vector<MediaPathPtr> _activeMediaPaths;
  std::vector<MediaPathPtr> _usedMediaPaths;
  size_t                    _minCapacity;
  size_t                    _maxCapacity;
  double                    _avgCapacity;
  double                    _stdDevCapacity;

  std::string _wcapPath;  // Store the wcap executable path
  std::string _jsonFilePath;  // Store JSON file path for recalculation

 protected:
  explicit     MediaPaths (const std::string &wcapPath, const std::string &mediaCapacities, size_t maxCapacity = 0);
  void         cleanUp ();
  friend class CLICodec;

 public:
  unsigned int size () {return _activeMediaPaths.size ();};
  MediaPathPtr getRandom ();
  size_t executeWcap(const std::string& wcapPath, const std::string& filepath);
  void writeJsonFile(const std::string& filename, const Json::Value& root);
  size_t       minCapacity    () {return _minCapacity;}
  size_t       maxCapacity    () {return _maxCapacity;}
  double       avgCapacity    () {return _avgCapacity;}
  double       stdDevCapacity () {return _stdDevCapacity;}

  bool isGood () {return size () /* > 0 */;};

  // Add methods for capacity management
  size_t getMinCapacity() const { return _minCapacity; }
  void recalculateCapacities(const std::string& commonArgs);
};

typedef MediaPaths *MediaPathsPtr;

class _RunCodec
{
 private:
  static struct event *_freeEvent (struct event *pEvent);

  static void EventOutCB   (evutil_socket_t fd, short what, void *arg);
  static void EventInOutCB (evutil_socket_t fd, short what, void *arg);
  static void EventInErrCB (evutil_socket_t fd, short what, void *arg);
  static void _EventInFn   (_RunCodec *runCodec, evutil_socket_t fd,
                            char **pIn, size_t *nIn, size_t *nextIn, size_t *capIn);
  const char *_pMsgIn;
  size_t      _nMsgIn;

  char      **_pMsgOut;
  size_t     *_nMsgOut;
  size_t      _nextOut;
  size_t      _capOut;

  char       *_pError;
  size_t      _nError;
  size_t      _nextErr;
  size_t      _capError;

  int         _rwepipe[3];
  int         _pid;

  int         _status;
  int         _pidStatus;

  struct event *_evIn;
  struct event *_evOut;
  struct event *_evErr;

  void _freeEVIn ();
  void _endLibevent ();

 public:
  explicit _RunCodec ();
  ~_RunCodec ();

  int run (std::string codecPath, std::string args,
           const void  *pMsgIn,  size_t  nMsgIn,
           void        *pMsgOut, size_t *nMsgOut);

  std::string error ();
};

class CLICodec
{
 private:
  static std::string _dirName;
  CLICodec& operator= (const CLICodec&);  // not implemented
  CLICodec (const CLICodec&);   // not implemented

 protected:
  MediaPathsPtr _mediaPaths;

  std::string   _mimeType;
  double        _encodingTime;
  std::string   _codecPath;
  std::string   _commonArgs;
  std::string   _encodeArgs;
  std::string   _decodeArgs;

  uint32_t      _secret;
  MediaPathPtr  _lastMediaPtr;

  bool          _isGood;
  bool          _isAndroid;

  // Add new members for argument overrides
  std::string _overrideCommonArgs;
  std::string _overrideEncodeArgs;
  bool _hasCommonArgsOverride;
  bool _hasEncodeArgsOverride;

  int           _runCodec (std::string args,
                           void  *pMsgIn,  size_t  nMsgIn,
                           void **pMsgOut, size_t *nMsgOut,
                           void **pError,  size_t *nError);

  CLICodec (): _mimeType ("") { _secret = 0; _isGood = 1; _isAndroid = 0; _mediaPaths = nullptr; _lastMediaPtr = nullptr; _encodingTime = 0; }
  explicit CLICodec (Json::Value jRoot);

 public:

  virtual ~CLICodec () {}

  static uint32_t  ipv4FromHost       (const std::string &ipStr);
  static uint32_t  makeSecret         (uint32_t ip1, uint32_t ip2);

  static bool      SetCodecsSpec      (const std::string &codecsSpec);
  static std::vector <std::string>
                   GetCodecNames      ();

  static void      SetDirname         (const std::string &dirName);
  static std::string
                   DirFilename        (const std::string &subPath);
  static CLICodec *GetNamedCodec      (std::string codecName);
  static CLICodec *GetCodecFromSpec   (std::string jsonSpec);
  static CLICodec *GetCodecFromStream (std::istream &is);

  virtual
  MediaPathPtr getRandomMedia ();

  size_t       minCapacity ()    {return _mediaPaths->minCapacity ();}
  size_t       maxCapacity ()    {return _mediaPaths->maxCapacity ();}
  double       avgCapacity ()    {return _mediaPaths->avgCapacity ();}
  double       stdDevCapacity () {return _mediaPaths->stdDevCapacity ();}
  std::string  mimeType ()       {return _mimeType;}
  double       encodingTime ()   {return _encodingTime;}

  virtual
  int encode (const void *pMsgIn, size_t nMsgIn, void **pMsgOut, size_t *nMsgOut, MediaPathPtr mediaPtr = nullptr);
  virtual
  int decode (const void *pMsgIn, size_t nMsgIn, void **pMsgOut, size_t *nMsgOut);

  void setSecret (const std::string &host1, const std::string &host2);
  void setSecret (uint32_t ip1, uint32_t ip2);
  void setSecret (uint32_t secret);

  // Add new method for setting argument overrides
  void setArgumentOverrides(const std::string& commonArgs, 
                            bool hasCommonOverride);

  // Add methods for capacity management
  size_t getMinimumCapacity() const;
  void recalculateCapacities(const std::string& commonArgs);

  bool isGood () {return _isGood;};
};

#define CLICODEC_H
#endif

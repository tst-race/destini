// Raw materials - low-level flickr API - Scratch that

// #define DEFAULT_AUTHFILE "/etc/race/rw_auth.json"
#define DEFAULT_AUTHFILE "/etc/race/flickr_auth/user1.ini"
#define DEFAULT_AUTHDIR "/etc/race/flickr_auth/"

#ifdef __cplusplus
extern "C" {
#endif

  #if 0
  flickcurl* flickr_open(char *auth_file, int verbosity);
  flickcurl_upload_status* flickr_upload(flickcurl* fcp, char *jpeg_file, char *title, char *tags, char* desc);
  int   flickr_delete(flickcurl* fcp, char* photo_id);
  flickcurl_photos_list* flickr_search(flickcurl* fcp, char* tags, char* text);
  char* flickr_download(flickcurl* fcp, char* photo_id, char* into_filename);
  void  flickr_close(flickcurl* fcp);
  #endif
  // With these raw materials, implement a basic post / get pair:

  
  typedef struct post_response {
    const char *data;
    unsigned long length;
    double timestamp;
  } post_response;

  char ** getFlickrPost64(int identifier, const char *user, const char *tags);
  post_response**  getFlickrPost(int identifier, const char *user, const char *tags);
  int  putFlickrPost(const char *msg, const char *user, const char *tags);

  void build_decoding_table(void);
  char *base64_encode(const unsigned char *data,
		      size_t input_length,
		      size_t *output_length);
  unsigned char *base64_decode(const char *data,
			       size_t input_length,
			       size_t *output_length);


#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdint.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif


#include <flickcurl.h>
//#include <flickcurl_cmd.h>

#include "flickr_transport.h"

#include <curl/curl.h>

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};
static int verbose = 3;

void build_decoding_table(void) {

    decoding_table = malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}


char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
      encoded_data[(int) (*output_length) - 1 - i] = '=';

    return encoded_data;
}


unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {

      uint32_t sextet_a = data[i] == '=' ? 0 & i++ : (uint32_t) decoding_table[(int) data[i++]];
      uint32_t sextet_b = data[i] == '=' ? 0 & i++ : (uint32_t) decoding_table[(int) data[i++]];
      uint32_t sextet_c = data[i] == '=' ? 0 & i++ : (uint32_t) decoding_table[(int) data[i++]];
      uint32_t sextet_d = data[i] == '=' ? 0 & i++ : (uint32_t) decoding_table[(int) data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}



/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
/* <DESC>
 * Download a given URL into a local file named page.out.
 * </DESC>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#include <curl/curl.h>
 
static size_t curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

#if 1
char* curl_get(char *url, char *pagefilename) {
  CURL *curl_handle;
  // static const char *pagefilename = "page.out";
  FILE *pagefile;
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */
  curl_handle = curl_easy_init();
 
  /* set URL to get here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
 
  /* Switch on full protocol/debug output while testing */

  // curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
 
  /* disable progress meter, set to 0L to enable it */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
 
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_data);
 
  /* open the file */
  pagefile = fopen(pagefilename, "wb");
  if(pagefile) {
 
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
 
    /* get it! */
    curl_easy_perform(curl_handle);
 
    /* close the header file */
    fclose(pagefile);
  }
 
  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);
 
  //  curl_global_cleanup();
 
  // The more general version of this could return the raw media in a
  // buffer, but for now we'll settle for download to file, and return
  // the filename:
  return pagefilename;
}
#else


char* curl_get(char *url, char *pagefilename) {
  CURL *curl_handle;
  // static const char *pagefilename = "page.out";
  FILE *pagefile;
  CURLcode c;
  char *rc;
 
  rc = NULL;
  curl_handle = curl_easy_init();
  if (!curl_handle) {
    if (verbose > 2)
      fprintf(stderr, "In curl_get, curl_easy_init failed.  Returning NULL.\n");
  } else {
    if (verbose > 2)
      fprintf(stderr, "In curl_get, curl_easy_init returned 0x%lx\n", (unsigned long) curl_handle);
    
    curl_easy_reset(curl_handle);
    /* set URL to get here */
    c = curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    c = curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    if (c != CURLE_OK) fprintf(stderr, "In curl_get, curl_easy_setopt failed with return code %d\n", (unsigned int) c);
    else {
      c = curl_easy_perform(curl_handle);
      if (c != CURLE_OK) fprintf(stderr, "In curl_get, curl_easy_perform failed with return code %d\n", (unsigned int) c);
      else {
	curl_easy_cleanup(curl_handle);
	rc = pagefilename;
      }
    }
  }
  return rc;
}


#endif

static void
print_upload_status(FILE* handle, flickcurl_upload_status* status,
                    const char* label)
{
  if(label)
    fprintf(handle, "flickr_transport: %s status\n", label);
  if(status->photoid)
    fprintf(handle, "  Photo ID: %s\n", status->photoid);
  if(status->secret)
    fprintf(handle, "  Secret: %s\n", status->secret);
  if(status->originalsecret)
    fprintf(handle, "  Original Secret: %s\n", status->originalsecret);
  if(status->ticketid)
    fprintf(handle, "  Ticket ID: %s\n", status->ticketid);
}



flickcurl_photo *flickr_photo_list_get(flickcurl_photos_list *l, int k) {
  if (k < 0 || k > l->photos_count) return NULL;
  else return l->photos[k];
}

static const char* yn_strings[2] = {"no", "yes"};

static const char*
yesno(int v)
{
  return yn_strings[(v ? 1 : 0)];
}

static void
print_upload_params(FILE* handle, flickcurl_upload_params* params,
                    const char* label)
{
  if(label)
    fprintf(handle, "flickr_transport: %s\n", label);

  fprintf(handle, "  File: %s\n", params->photo_file);
  if(params->title)
    fprintf(handle, "  Title: '%s'\n", params->title);
  else
    fprintf(handle, "  Title: none\n");
  if(params->description)
    fprintf(handle, "  Description: '%s'\n", params->description);
  else
    fprintf(handle, "  Description: none\n");
  fprintf(handle, "  Tags: %s\n", (params->tags ? params->tags : ""));
  fprintf(handle, "  Viewable by Public: %s  Friends: %s  Family: %s\n",
          yesno(params->is_public), yesno(params->is_friend),
          yesno(params->is_family));
  fprintf(handle, "  Safety level: %s (%d)\n",
          flickcurl_get_safety_level_label(params->safety_level),
          params->safety_level);
  fprintf(handle, "  Content type: %s (%d)\n",
          flickcurl_get_content_type_label(params->content_type),
          params->content_type);
  fprintf(handle, "  Hidden: %s (%d)\n",
          flickcurl_get_hidden_label(params->hidden),
          params->hidden);
}




static void
my_message_handler(void *user_data, const char *message)
{
  fprintf(stderr, "flickr_transport ERROR (%lx): %s\n", (unsigned long) user_data, message);
}


static void
flickr_find_cred(void* userdata, const char* key, const char* value)
{
  flickcurl *fc = (flickcurl *) userdata;
  
  if (verbose > 2)
    fprintf(stderr, "flickr_find_cred: key = %s;  value = %s\n", key, value);
  if(!strcmp(key, "api_key"))
    flickcurl_set_api_key(fc, value);
  else if(!strcmp(key, "secret"))
    flickcurl_set_shared_secret(fc, value);
  else if(!strcmp(key, "auth_token"))
    flickcurl_set_auth_token(fc, value);
  else if(!strcmp(key, "oauth_client_key"))
    flickcurl_set_oauth_client_key(fc, value);
  else if(!strcmp(key, "oauth_client_secret"))
    flickcurl_set_oauth_client_secret(fc, value);
  else if(!strcmp(key, "oauth_token"))
    flickcurl_set_oauth_token(fc, value);
  else if(!strcmp(key, "oauth_token_secret"))
    flickcurl_set_oauth_token_secret(fc, value);
}


int flickr_authenticate(flickcurl *fc, char *authfile) {
  int rc;

  fprintf(stderr, "Using auth file %s\n", authfile);
  rc = flickcurl_config_read_ini(fc, authfile, "flickr", fc, flickr_find_cred);
  if (verbose > 2) {
    if (!rc) {
      fprintf(stderr, "flickr_authenticate: AUTHENTICATION SUCCESSFUL!\n");
    } else {
      fprintf(stderr, "flickr_authenticate: AUTHENTICATION FAILED WITH RETURN CODE %d!\n", rc);
    }
  }

  return rc;
}


flickcurl *flickr_open(char* authfile, int verbosity) {
  
  // You need to create a flickcurl object:

  flickcurl *fc = NULL;

  verbose = verbosity;
  // Initialize flickcurl library:
  flickcurl_init();

  // PROBLEM:  Where does authentication go?
  
  // we probably don't need this:
  // flickcurl_cmdline_init();


  // fprintf(stderr, "flickr_open: after flickcurl_init()\n");
  fc = flickcurl_new();
  //  fprintf(stderr, "flickr_open: flickcurl_new() returns %lx\n", (unsigned long) fc);
  flickcurl_set_error_handler(fc, my_message_handler, NULL);

  // Insert auth stuff here:
  // fprintf(stderr, "flickr_open: calling flickr_authenticate (fc=%lx)\n", (unsigned long) fc);
  flickr_authenticate(fc, authfile);
  if (verbose > 1) {
    fprintf(stderr, "\n===========================================================================\n");
    fprintf(stderr, "flickr_transport.c: flickr_open RETURNING %lx, AUTHENTICATED USING %s\n", (unsigned long) fc, authfile);
    fprintf(stderr, "===========================================================================\n\n");
  }
  return fc;
}


//   *  *  UPLOAD  *  *


flickcurl_upload_status* flickr_upload(flickcurl *fc, char *jpeg_filename, char *title, const char *tags, char *desc) {
  // int id;
  flickcurl_upload_params params;
  flickcurl_upload_status* status = NULL;

  // Default fields to 0:
  memset(&params, '\0', sizeof(flickcurl_upload_params));
  params.safety_level = 1;       // (safe)
  params.hidden = 1;              // (default public)
  params.content_type = 1;       // (default photo)
  params.photo_file = jpeg_filename;
  params.description = desc;
  // Useful defaults?
  params.is_friend = 0;
  params.is_family = 0;
  params.is_public = 1;
  params.tags = tags; // <tags separated by spaces, null terminated.>
  params.title = title;
  // id = 0;

  if(verbose > 0)
    fprintf(stdout, "flickr_transport.c:   flickr_upload: Uploading file %s\n", params.photo_file);
  else if (verbose > 1)
    print_upload_params(stdout, &params, "Photo upload");

  status = flickcurl_photos_upload_params(fc, &params);

  if (status) {
    if (verbose > 2)
      print_upload_status(stdout, status, "Photo upload");
  } else {
    if (verbose) fprintf(stderr, "flickr_upload: upload failure.");
  }

  if(verbose > 0)
    fprintf(stdout, "flickr_transport.c:   flickr_upload: Finished uploading.  Verifying...\n");

  // The CALLER is responsible for freeing these status objects when
  // they're no longer needed:
 
  // flickcurl_free_upload_status(status);

  // Alternative: create a universal struct that holds ID, secrets,
  // etc that is valid across all the API functions here.

  return status;
}

//   *  *  DELETE  *  *

int flickr_delete(flickcurl* fc, char* photo_id) {
  return flickcurl_photos_delete(fc, photo_id);
}

// Issues with flickcurl: the URLs we get from the basic API are NOT
// images, they are UIs.  Here is a sequence that will get the correct
// stuff from the command line:

// flickcurl photos.getSizes 53409867987
//   - look for the original

// Here's the gist of the C code:

#define VIDEO_LABEL "720p"
#define VIDEO_LABEL_ALT "360p"

static char * flickr_get_image_url(flickcurl *fc, char* photo_id, int* is_video) {
  int i, j;
  char *uri = NULL;
  char *video_uri = NULL;
  flickcurl_size** sizes = NULL;
  flickcurl_photo *p;
  sizes = flickcurl_photos_getSizes(fc, photo_id);

  *is_video = 0;
  for (i = 0; sizes[i]; i++) {
    if (verbose > 2)
      fprintf(stderr, "flickr_transport.c:       flickr_get_image_url: %s => %s\n", sizes[i]->label, sizes[i]->source);
    if (!strcmp(sizes[i]->label, VIDEO_LABEL)) {
      video_uri = strdup(sizes[i]->source);
      // video_uri = malloc(128);
      // snprintf(video_uri, 128, "https://www.flickr.com/video_download.gne?id=%s", photo_id);
    } else if ( !video_uri && !strcmp(sizes[i]->label, VIDEO_LABEL_ALT) ) {
      video_uri = strdup(sizes[i]->source);
    }

    if (!strcmp(sizes[i]->label,"Original")) uri = strdup(sizes[i]->source);

  }

  if ( !video_uri ) {
    fprintf(stderr, "flickr_transport.c:       flickr_get_image_url video_uri not found.\n");
    return uri;
  } else {
    j = 20;
    p = flickcurl_photos_getInfo2(fc, photo_id, NULL);
    if (verbose > 2) {
      fprintf(stderr, "      flickr_get_image_url: %s %s %lx\n", photo_id, p->media_type, (unsigned long) p->video);
      if (p->video)
	fprintf(stderr, "      flickr_get_image_url: p->video->ready = %d\n", p->video->ready);
    }
    while (j > 0 && !p->video) {
      sleep(10);
      j -= 1;
      p = flickcurl_photos_getInfo2(fc, photo_id, NULL);
      if (verbose > 2) {
	fprintf(stderr, "      flickr_get_image_url: %s %s %lx\n", photo_id, p->media_type, (unsigned long) p->video);
	if (p->video)
	  fprintf(stderr, "      flickr_get_image_url: p->video->ready = %d\n", p->video->ready);
      }
    }
    *is_video = 1;
    return video_uri;
  }

}

#if 0
  
static char * flickr_get_video_url(flickcurl *fc, char* photo_id) {
  int i;
  char *uri = NULL;
  flickcurl_size** sizes = NULL;
  sizes = flickcurl_photos_getSizes(fc, photo_id);
  for (i = 0; sizes[i]; i++) {
    fprintf(stderr, "        flickr_get_image_url: %s => %s\n", sizes[i]->label, sizes[i]->source);
    if (!strcmp(sizes[i]->label, "Video Original")) uri = strdup(sizes[i]->source);
  }
  return uri;
}

#endif
// Flickr search params: list_params control parameters of the
// returned list:

/*
  list_params.extras
  list_params.per_page
  list_params.page
  list_params.format
*/

// params is a struct of search parameters:
/*
  params.user_id
  params.tag_mode
  params.min_upload_date
  params.max_upload_date
  params.min_token_date
  params.max_token_date
  params.license
  params.sort
  params.privacy_filter
  params.bbox
  params.accuracy
  params.safe_search = <1 for safe, 2 for moderate, 3 for restricted>
  params.content_type = <1 for photos only, 2 for screenshots, 3 for 'other' only, 4 for all types>
  params.machine_tags
  params.machine_tag_mode
  params.group_id
  params.place_id
  params.media = <'all' is the default, 'photos' or 'video'>
  params.has_geo = <1 if hits must contain geo information>
  params.lat
  params.lon
  params.radius
  params.radius_units
  params.contacts
  params.woe_id
  params.geo_context
  params.is_commons
  params.in_gallery
*/



//   *  *  SEARCH  *  *  -- Add an arg for media type, and try to constrain:
flickcurl_photos_list*  flickr_search(flickcurl *fc, const char *tags, char *text) {
  char *uri;
  int i;
  flickcurl_search_params params;
  flickcurl_photos_list_params list_params;
  flickcurl_photos_list* photos_list;
  flickcurl_photo *photo;
  int is_video;
  // params and list_params must both be initialized:

  if (verbose > 2)
    fprintf(stderr, "flickr_transport.c: in flickr_search(%lx,...) before params init\n", (unsigned long) fc);
  // This just initializes everything to 0, it seems:
  flickcurl_search_params_init(&params);
  // params.media = media;
  flickcurl_photos_list_params_init(&list_params);
  if (verbose > 2)
    fprintf(stderr, "flickr_transport.c: in flickr_search after params init\n");
  
  // For our usage, it might be okay just to default the parameters above.

  params.text = text; // <probably best bet for 'title'>
  params.tags = strdup(tags); // <comma-separated tags string>
  //  params.media = "photos";
  //  params.extras = "original_format";
  //  list_params.extras = "original_format";

  if (verbose > 2)
    fprintf(stderr, "flickr_transport.c: in flickr_search before flickcurl_photos_search_params\n");
  photos_list = flickcurl_photos_search_params(fc, &params, &list_params);    

  /* THis list just tries to summarize the findings: */
  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: Summary of flickr_search results:\n");

  for (i = 0; i < photos_list->photos_count; ++i) {
    photo = photos_list->photos[i];
    if (photo == NULL) fprintf(stderr, "flickr_transport.c: flickr_search encountered a null photo.\n");
    else {
      if (verbose > 1)
	fprintf(stderr, "flickr_transport.c: flickr_search photo %d has media type %s\n", i, photos_list->photos[i]->media_type);

      uri = flickr_get_image_url(fc, photo->id, &is_video);

      if (verbose > 2 && (photo->video != NULL || is_video) )
	fprintf(stderr, "flickr_transport.c: flickr_search  photo %d is a video (photo->video=0x%lx)\n", i, (unsigned long) photo->video);

      if (verbose > 2)
	fprintf(stderr, "flickr_transport.c: flickr_search result %d has ID %s => %s\n", i, photo->id, uri);
    }

  }
  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: flickr_search returning photos_list=%lx\n", (unsigned long) photos_list);
  
  return photos_list;
}



//   *  *  DOWNLOAD  *  *

// Video hack?  https://www.flickr.com/video_download.gne?id=53570381293
// Where the template is https://www.flickr.com/video_download.gne?id=<ID>


// First get the URI via this kind of code:

char * flickr_download(flickcurl* fc, char* photo_id, char *filename) {
  // filename can be NULL, in which case the return value is a buffer
  // containing the raw media (e.g. JPEG).  If filename is not NULL,
  // then contents are downloaded into the named file.
  char *uri;
  char *out;
  int is_video;

  // This does not seem to work:
  //  uri = flickcurl_photo_id_as_short_uri(photo_id);
  uri = flickr_get_image_url(fc, photo_id, &is_video);
  
  if (uri) {
    if (verbose > 1)
      fprintf(stdout, "flickr_transport.c: flickr_download (%lx): Short URI for photo ID %s is %s => %s\n",
	      (unsigned long) fc, photo_id, uri, filename);

    if (verbose > 2)
      fprintf(stdout, "flickr_transport.c: flickr_download: before calling curl_get(%s,%s)\n", uri, filename);


    out = curl_get(uri, filename);

    if (verbose > 2) {
      fprintf(stderr, "In flickr_download, curl_get(%s, %s) returns a string of length %lu.\n", uri, filename, strlen(out));

      if (strlen(out) < 500)
	fprintf(stderr, "   out = %s\n", out);

      fprintf(stdout, "flickr_transport.c: flickr_download: after calling curl_get (returned %s)\n", out);
    }
    // free(uri);
    // Either a filename or the raw media buffer:
    return out;
  } else {
    if (verbose > 1)
      fprintf(stderr, "flickr_transport.c: flickr_download: Failed to get short URI for photo ID %s\n",
	      photo_id);
    return 0;
  }
  return 0;
}

// Then open the URI to download - using what though?

//  *  *   CLOSE  *  *

void flickr_close(flickcurl *fp) {
  if (fp)  flickcurl_free(fp);
  flickcurl_finish();
}


static post_response* make_post_response(void) {
  post_response *pr;
  /* ** Allocate a post_response object isomorphic to the C++ JSON response */
  pr = malloc(sizeof(post_response));
  pr->data = NULL;
  pr->length = 0; // this is really an index.
  pr->timestamp = 0.0;

  return pr;
}

// Hack for testing:
#define DEFAULT_TAGS "frob128818ee,anachron229189899"
#define DEFAULT_DOWNLOAD_JPG "/tmp/flickr_transport_image.jpg"
#define DEFAULT_DOWNLOAD_VIDEO "/tmp/flickr_transport_video.mp4"

// Given some kind of identifier, grab the post that corresponds to it:
//
// Have this return multiple values:
post_response**  getFlickrPost(int identifier, const char *user, const char *tags) {
  // Note, media_type must be either "photo" or "video".
  struct stat sbuf;
  char identifier_string[64];  // Risky business
  char *data;
  char *dfile;
  post_response **pr_list;
  post_response *pr;
  int i, n;

  /* This is an extreme hack.  User names are constrained to user1
     through user6, so we need only change 1 digit, as long as the
     same convention is obeyed by the 'user' argument. */

  //   char *authfile = DEFAULT_AUTHFILE;

  char *authfile = strdup(DEFAULT_AUTHFILE);
  if (user && strlen(user) > 4)  authfile[26] = user[4];

  //  fprintf(stderr, "flickr_transport.c: In getFlickrPost(%d)\n", identifier);

  /* ** Open flickr, search for candidates, and download: */
  if (verbose > 0)
    fprintf(stderr, "flickr_transport.c: getFlickrPost(%d, %s, %s) authenticating as user %s.\n", identifier, user, tags, user);

  flickcurl *fp = flickr_open(authfile, 1);

  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: getFlickrPost(%d) flickr_open returns %lx.", identifier, (unsigned long) fp);

  sprintf(identifier_string, "%d", identifier);

  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: getFlickrPost(%d) calling flickr_search(%lx, %s, '')\n", identifier, (unsigned long) fp, tags);

  flickcurl_photos_list* plist = flickr_search(fp, tags, "");

  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: getFlickrPost: flickr_search returns %lx\n", (unsigned long) plist);

  n = (int) plist->photos_count;

  if (verbose > 0)
    fprintf(stderr, "flickr_transport.c: getFlickrPost: Number of media items found: %d\n", n);
  
  pr_list = (post_response**) calloc((unsigned long) n+1, sizeof(post_response*) );

  /* flickr_download creates a file, so we will need to read that and
     return the bytes: */
  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: In getFlickrPost(%d), walking search response list.\n", identifier);

  for ( i = 0; i < n; i++) {
    if (verbose > 2)
      fprintf(stderr, "flickr_transport.c: getFlickrPost(%d), downloading photo %d.\n", identifier, i+1);
    if (plist->photos[i] == NULL) {
      if (verbose > 2)
	fprintf(stderr, "flickr_transport.c: flickr_download: photo %d is null.\n", i+1);
      // Stop here, for now:
      break;
    } else {
      if (verbose > 2)
	fprintf(stderr, "flickr_transport.c: before calling flickr_download\n");
      // These fields are apparently meaningless!
      // fprintf(stderr, "flickr_download [%d] media_type = %s\n", i, plist->photos[i]->media_type);
      // fprintf(stderr, "flickr_download [%d] ->video = 0x%lx\n", i, (unsigned long) plist->photos[i]->video);
      // if (plist->photos[i]->media_type[0] == 'p') {
      if (plist->photos[i]->video == NULL) {
	// It's always going to go here:
	dfile = DEFAULT_DOWNLOAD_JPG;
	data = flickr_download(fp, plist->photos[i]->id, dfile);
      } else {
	dfile = DEFAULT_DOWNLOAD_VIDEO;
	data = flickr_download(fp, plist->photos[i]->id, dfile);
      }

      if (verbose > 2)
	fprintf(stderr, "flickr_transport.c: getFlickrPost(%d), flickr_download returns %s.\n", identifier, data);

      if (verbose > 2)
	fprintf(stderr, "flickr_transport.c: getFlickrPost(%d), flickr_close(%lx) returns (5d).\n", identifier, (unsigned long) fp);

      if (data != NULL) {
	pr = make_post_response();
	pr_list[i] = pr;

	/* Read the downloaded JPEG, if possible: */
	if (verbose > 1)
	  fprintf(stderr, "flickr_transport.c: opening %s.\n", dfile);
	
	int fd = open(dfile, O_RDONLY);
	if (fd >= 0) {
	  int rc = fstat(fd, &sbuf);
	  if (rc == 0) {
	    /* Get the file size: */
	    n = (int) (sbuf.st_size + 1);
	    char *buf = malloc( (unsigned long) n);
	    rc = read(fd, buf, (unsigned long) n-1);
	    buf[n-1] = 0;
	    close(fd);

	    pr->data = buf;
	    pr->length = (unsigned long) n-1;
	  }
	}
      }
    }
  }

  // flickr_close(fp);
  /* free(authfile); */
  return pr_list;
}


char**  getFlickrPost64(int identifier, const char *user, const char *tags) {
  //struct stat sbuf;
  //char identifier_string[64];
  post_response **pr_list;
  post_response *pr;
  char ** media_list;
  int i, n;

  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: in getFlickrPost64\n");

  pr_list = getFlickrPost(identifier, user, tags);

  if (verbose > 2)
    fprintf(stderr, "flickr_transport.c: getFlickrPost64: returned from getFlickrPost\n");
  
  for (n  = 0; pr_list[n] != NULL; n++) continue;

  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: getFlickrPost64 extracted %d post search results.\n", n);

  media_list = calloc((unsigned long)n, sizeof(char*));

  for (i = 0; i < n; i++) {
    pr = pr_list[i];
  
    size_t outlen;
    char *media_msg = (char *) base64_encode((const unsigned char*) pr->data, pr->length, &outlen);
    media_list[i] = media_msg;
  }

  /* Return the allocated post response object.  This will have zeros
     in its fields if we were unable to download for some reason: */
  if (verbose > 1)
    fprintf(stderr, "flickr_transport.c: getFlickrPost64() returning %lx.\n", (unsigned long) media_list);

  return media_list;
}


#define DEFAULT_UPLOAD_JPG "/tmp/post.jpg"
#define DEFAULT_UPLOAD_VIDEO "/tmp/post.mp4"

int putFlickrPost(const char *msg, const char *user, const char *tags) {
  int i, n, found;
  size_t outlen;
  flickcurl *fcp;
  flickcurl_upload_status* status = NULL;
  flickcurl_photos_list* list;
  unsigned char *base64_decode(const char *data,
			       size_t input_length,
			       size_t *output_length);
  // char *authfile = DEFAULT_AUTHFILE;

  char *authfile = strdup(DEFAULT_AUTHFILE);
  char *upload_file;
  if (user && strlen(user) > 4)  authfile[26] = user[4];

  unsigned char *jpg_msg = base64_decode(msg, strlen(msg), &outlen);

  int fd;
  if (msg[0] == 'J' && msg[1] == 'F' && msg[2] == 'I' && msg[3] == 'F') {
    upload_file = strdup(DEFAULT_UPLOAD_JPG);
  } else {
    upload_file = strdup(DEFAULT_UPLOAD_VIDEO);
  }
  fd = open(upload_file, O_WRONLY|O_CREAT, 0777);
  if (verbose > 1)
    fprintf(stderr, "In putFlickrPost: open(%s, O_WRONLY) returns %d\n", upload_file, fd);


  if (fd < 0) {
    perror("putFlickrPost ");
    return 0;
  } else {
    write(fd, jpg_msg, outlen);
    close(fd);
  }
  free(jpg_msg);
  
  if (verbose > 0)
    fprintf(stderr, "In putFlickrPost: authenticating as user %s with tags %s\n", user, tags);

  fcp = flickr_open(authfile, 1);
  
  if (verbose > 0) {
    if (verbose > 2)
      fprintf(stderr, "putFlickrPost: flickr_open returns %lx\n", (unsigned long) fcp);
    fprintf(stderr, "putFlickrPost: Posting with tags %s\n", tags);
  }
   
  status = flickr_upload(fcp, upload_file, "My Title 0", tags, NULL);

  if (verbose > 0)
    fprintf(stderr, "putFlickrPost: flickr_upload returns %lx\n", (unsigned long) status);

  if (!status) return 0;
    
  // flickcurl_photo *uploaded = flickcurl_photos_getInfo2(fcp, status->photoid, NULL);

  char *id = status->photoid;
  if (verbose > 0)
    fprintf(stderr, "putFlickrPost:  Uploaded; polling to verify post....\n");

  found = 0;
  for (i = 0; i < 20 && !found; i++) {
    list = flickr_search(fcp, tags, 0);
    n = list->photos_count;
    if (verbose > 0)
      fprintf(stderr, "putFlickrPost: still looking....\n");
    for (i = 0; i < n; i++) {
      if (!strcmp(list->photos[i]->id, id)) {
	found = 1;
	if (verbose > 0)
	  fprintf(stderr, "putFlickrPost:  MESSAGE POSTED SUCCESSFULLY\n");
	break;
      }
    }
    sleep(5);
  }
  /* free(authfile); */
  if (found) return 1;
  else return 0;
}


/* ---------------------------------------------------------------------- */

/* Mains for standalone testing:                                          */



#ifdef TEST1

// gcc -o ft -DTEST1 flickr_transport.c /usr/local/lib/libflickcurl.a -lxml2 -lcurl

/* This version (-DTEST1) tests the primitive flickr transport API - low-level calls.
 */
int main(int argc, char **argv) {
  char *media_file;
  char *auth_file;
  char *fname;
  char *id;
  flickcurl *fcp;
  flickcurl_upload_status* status = NULL;
  flickcurl_photos_list* list;
  flickcurl_photo* uploaded;
  int found;
  int i,n;
  int rc;

  if (argc != 4) {
    printf("Usage: %s <media_in> <auth_file>\n", argv[0]);
    exit(-1);
  }
  
  media_file = argv[1];
  auth_file = argv[2];

  fcp = flickr_open(auth_file, 1);

  /* flickr_upload is ok taking video: */

  status = flickr_upload(fcp, media_file, "My Title 0", DEFAULT_TAGS, NULL);

  if (!status) {
    printf("Failed to upload.  Quitting...\n");
    exit(-1);
  }
  uploaded = flickcurl_photos_getInfo2(fcp, status->photoid, NULL);

  id = status->photoid;
  printf("Uploaded; polling to see when search finds it...\n");

  found = 0;
  for (i = 0; i < 20 && !found; i++) {
    list = flickr_search(fcp, DEFAULT_TAGS, 0);
    n = list->photos_count;
    for (i = 0; i < list->photos_count; i++) {
      if (!strcmp(list->photos[i]->id, status->photoid)) {
	found = 1;
	break;
      }
    }
    sleep(2);
  }

  if (found) printf("Photo has been uploaded and indexed.\n");
  
  printf("Uploaded photo object has:\n");
  printf("    Photo ID = %s\n", uploaded->id);
  printf("    URI = %s\n", uploaded->uri);

  verbose = 10;
  list = flickr_search(fcp, DEFAULT_TAGS, 0);

  n = list->photos_count;
  if (n <= 50) {
    printf("Search done --  %d results:\n", n);
  } else {
    printf("Search done -- %d results, limiting to 50:\n", n);
    n = 50;
  }

  for (i = 0; i < n; i++)
    printf("Image %d id: %s ; media_type: %s\n", i, list->photos[i]->id, list->photos[i]->media_type);

  for (i = 0; i < list->photos_count; i++)
    if (!strcmp(list->photos[i]->id, status->photoid))
      printf("Found it! (photo id %s)\n", status->photoid);

  sleep(10);

  fname = flickr_download(fcp, status->photoid, "/tmp/flickr_transport.media");

  printf("Download done; wait 0 seconds...\n");
  // sleep(60);

  // flickr_delete(fcp, status->photoid);

  printf("Delete done; now close...\n");

  flickr_close(fcp);
}

#endif



/* ---------------------------------------------------------------------- */

#ifdef TEST2

// gcc -o ft -DTEST2 flickr_transport.c /usr/local/lib/libflickcurl.a -lxml2 -lcurl

/* This version (-DTEST2) tests cross-user posting and retrieval of
 * media (jpg or video)
 */

int main(int argc, char **argv) {
  char *msg;
  char *media_file_in, *media_file_out;
  char *auth_file_in, *auth_file_out;
  char *fname;
  char *id;
  flickcurl *fcp;
  flickcurl_upload_status* status = NULL;
  flickcurl_photos_list* list;
  flickcurl_photo* uploaded;
  int found, autodelete;
  int i,n;
  int rc;

  verbose = 3;
  
  if (argc < 5) {
    printf("Usage: %s <auth_in> <media_in> <auth_out> <media_out> [autodelete]\n", argv[0]);
    printf("       Flickr Round Trip Test:\n");
    printf("       Using credentials in auth_in, upload media_in to Flickr.\n");
    printf("       Then, using credentials in auth_out, download media_out from Flickr.\n");
    exit(-1);
  }
  
  auth_file_in = argv[1];
  media_file_in = argv[2];

  auth_file_out = argv[3];
  media_file_out = argv[4];

  if (argc > 5) autodelete = 1;
  else autodelete = 0;

  /* -------------------- UPLOAD -------------------- */

  /* Authenticate for upload: */
  fcp = flickr_open(auth_file_in, 1);

  status = flickr_upload(fcp, media_file_in, "My Title 0", DEFAULT_TAGS, NULL);

  if (!status) {
    printf("Failed to upload.  Quitting...\n");
    exit(-1);
  }
  uploaded = flickcurl_photos_getInfo2(fcp, status->photoid, NULL);

  id = status->photoid;

  flickr_close(fcp);

  printf("Uploaded; closing.\n");

  /* -------------- SEARCH AND DOWNLOAD -------------- */
  
  /* Authenticate for upload: */
  fcp = flickr_open(auth_file_out, 1);
  printf("Download:  flickr_open returns 0x%lx.\n", (unsigned long) fcp);

  found = 0;
  for (i = 0; i < 20 && !found; i++) {
    list = flickr_search(fcp, DEFAULT_TAGS, 0);
    n = list->photos_count;
    for (i = 0; i < list->photos_count; i++) {
      if (!strcmp(list->photos[i]->id, status->photoid)) {
	found = 1;
	break;
      }
    }
    sleep(2);
  }

  if (found) printf("Media has been uploaded, indexed, and found.\n");
  
  printf("Uploaded photo object has:\n");
  printf("    Photo ID = %s\n", uploaded->id);
  printf("    URI = %s\n", uploaded->uri);

  verbose = 10;
  list = flickr_search(fcp, DEFAULT_TAGS, 0);

  n = list->photos_count;
  if (n <= 50) {
    printf("Search done --  %d results:\n", n);
  } else {
    printf("Search done -- %d results, limiting to 50:\n", n);
    n = 50;
  }

  for (i = 0; i < n; i++)
    printf("Image %d id: %s ; media_type: %s\n", i, list->photos[i]->id, list->photos[i]->media_type);

  for (i = 0; i < list->photos_count; i++)
    if (!strcmp(list->photos[i]->id, status->photoid))
      printf("Found it! (photo id %s)\n", status->photoid);

  sleep(10);

  fname = flickr_download(fcp, status->photoid, media_file_out);

  printf("Download done; now close...\n");

  flickr_close(fcp);

  if (autodelete) {
    /* Authenticate for delete: */
    fcp = flickr_open(auth_file_in, 1);
    i = flickr_delete(fcp, id);
    printf("flickr_delete called with id = %s returns %d\n", id, i);
  }
}

#endif

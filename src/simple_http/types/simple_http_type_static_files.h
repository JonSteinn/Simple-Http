#ifndef f5n4cd1uRJ_SIMPLE_HTTP_TYPE_STATIC_FILES_H
#define f5n4cd1uRJ_SIMPLE_HTTP_TYPE_STATIC_FILES_H

#include <stdbool.h>
#include <glib.h>

/**
 * A data structure that stores data needed to support
 * static files serving from the server.
 * 
 *  has_static
 *      Has a static folder been created by the
 *      framework user?
 *  supported_media_types
 *      Map of supported file extesions to their mediatype
 *  files
 *      A hashset of all full paths (with static folder as base)
 *      to supported static files
 */
typedef struct {
    bool            has_static;
    GHashTable*     supported_media_types;
    GHashTable*     files;
} StaticFiles;

#endif

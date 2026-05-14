#pragma once

#include <stdint.h>

#ifndef RDF_TERMTYPE_DEFINED
#define RDF_TERMTYPE_DEFINED
typedef enum {
        URI = 0,
        BNODE = 1,
        TYPEDLITERAL = 2,
        LANGLITERAL = 3
} TERMTYPE;
#endif

#ifndef TRIPLEHANDLER_DEFINED
#define TRIPLEHANDLER_DEFINED
/*
 * Use TERMTYPE for subject_type, object_type and graph_type.
 * If graphid is NULL, the default graph is used.
 */
typedef int8_t TripleHandler(
                const char* subject, uint8_t subject_type,
                const char* predicate,
                const char* object, const char* object_suffix,
                uint8_t object_type,
                const char* graphid, uint8_t graph_type,
                void* user);

#endif //TRIPLEHANDLER_DEFINED

typedef struct nquadsSerializer NQuadsSerializer;

/**
 * Parse given nquads file and call triplehandler with any found triple.
 * Returns 0 on success.
 */
int nquads_parse_file(const char* filename, TripleHandler* triplehandler, void* user);

/**
 * Parse given nquads string and call triplehandler with any found triple.
 * Returns 0 on success.
 */
int nquads_parse(const char* input, TripleHandler* triplehandler, void* user);

NQuadsSerializer* NQuadsRDF_SER_start();
int64_t NQuads_SER_add(const char* subject, uint8_t subject_type,
                const char* predicate,
                const char* object, const char* object_suffix,
                uint8_t object_type,
                const char* graph_id, uint8_t graph_type,
                NQuadsSerializer* serializer);
char* NQuadsRDF_SER_finish(NQuadsSerializer*);

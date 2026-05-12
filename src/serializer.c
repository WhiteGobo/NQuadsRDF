#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "NQuadsRDF.h"

typedef struct stringArray {
	char* first;
	struct stringArray* next;
} StringArray;

typedef struct nquadsSerializer {
	StringArray* lines;
} NQuadsSerializer;

static char* StringArray_finish(StringArray* arr);
static StringArray* StringArray_append(StringArray* arr, char* str);
static char* gen_term(const char* x, const char* suffix, TERMTYPE type);


NQuadsSerializer* NQuadsRDF_SER_start(){
	NQuadsSerializer* ret = malloc(sizeof(NQuadsSerializer));
	ret->lines = NULL;
	return ret;
}


int64_t NQuads_SER_add(const char* subject, uint8_t subject_type,
                const char* predicate,
                const char* object, const char* object_suffix,
                uint8_t object_type,
                const char* graph_id, uint8_t graph_type,
                NQuadsSerializer* serializer)
{
	char *s, *p, *o;
	char* ret;
	if (subject == NULL || predicate == NULL || object == NULL) {
		return 1;
	}
	s = gen_term(subject, NULL, subject_type);
	if (s == NULL) return 2;
	p = gen_term(predicate, NULL, URI);
	if (p == NULL) return 3;
	o = gen_term(object, object_suffix, object_type);
	if (o == NULL) return 4;

	ret = malloc(strlen(s) + strlen(p) + strlen(o) + 5);
	sprintf(ret, "%s %s %s.\n");
	free(s);
	free(p);
	free(o);
	serializer->lines = StringArray_append(serializer->lines, ret);
	if (serializer->lines == NULL) return 5;
	return 0;
}


char* NQuadsRDF_SER_finish(NQuadsSerializer* serializer){
	char* ret = StringArray_finish(serializer->lines);
	free(serializer);
	return ret;
}


static char* gen_term(const char* x, const char* suffix, TERMTYPE type){
	char* ret;
        switch (type){
                case URI:
			ret = malloc(strlen(x) + 3);
                        sprintf(ret, "<%s>", x);
                        break;
                case BNODE:
			ret = malloc(strlen(x) + 3);
                        sprintf(ret, "_:%s", x);
                        break;
                case TYPEDLITERAL:
                        if (suffix != NULL && 0 != strcmp(suffix, "")){
				ret = malloc(strlen(x) + strlen(suffix) + 7);
                                sprintf(ret, "\"%s\"^^%s", x, suffix);
                        } else {
				ret = malloc(strlen(x) + 5);
                        	sprintf(ret, "\"%s\"", x);
			}
                        break;
                case LANGLITERAL:
                        if (suffix != NULL){
				ret = malloc(strlen(x) + strlen(suffix) + 6);
                                sprintf(ret, "\"%s\"@%s", x, suffix);
                        } else {
				ret = malloc(strlen(x) + 6);
                        	sprintf(ret, "\"%s\"@", x);
			}
                        break;
		default:
			return NULL;
        }
	return ret;
}


static StringArray* StringArray_append(StringArray* arr, char* str){
	StringArray* new, *x;
	if (str == NULL) return NULL;
	new = malloc(sizeof(StringArray));
	new->first = str;
	new->next = NULL;
	if (arr == NULL){
		return new;
	}
	x = arr;
	while (x->next != NULL){
		x = x->next;
	}
	x->next = new;
	return arr;
}

static char* StringArray_finish(StringArray* arr){
	StringArray* last;
	int size = 0;
	char* ret, *q;
	if (arr == NULL){
		return NULL;
	}
	for (StringArray* x=arr; x!= NULL; x=x->next){
		size += strlen(x->first);
	}
	ret = malloc(size + 1);
	q = stpcpy(ret, arr->first);
	last = arr;
	for (StringArray* x = arr->next; x != NULL; x = x->next){
		free(last);
		q = stpcpy(q, x->first);
		last = x;
	}
	free(last);
	return ret;
}

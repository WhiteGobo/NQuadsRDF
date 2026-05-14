/* nquads_parser.y */

%{
#include "NQuadsRDF.h"
#include "term.h"
#include <stdio.h>

int yyerror(char *s);
int yylex(void);

static TripleHandler* my_triplehandler;
static void* my_triplehandler_context;

extern void nquads_scanner_tidy_up();

Term* new_term(char*, char*, TERMTYPE);
Term* new_term2(char*, char*, TERMTYPE);
void my_send_triple(Term* subject, char* predicate, Term* object);
void my_send_quad(Term* subject, char* predicate, Term* object, Term* graphid);

%}

%union{
  Term*		term_val;
  char*		string_val;
}

%start	statements

%token	<string_val>	IRIREF_T
%token	<string_val>	BNODE_T
%token	<string_val>	STRING_T
%type	<term_val>	subject
%type	<string_val>	predicate
%type	<term_val>	object
%type	<term_val>	graphid
%type	<term_val>	langliteral
%type	<term_val>	typedliteral
%left	END_TRIPLE
%left	ATSIGN
%left	DOUBLECARET

%destructor {free($$);} subject object graphid

%%

statements:	/* empty */
		| triple statements
		| quad statements
		;

quad:		subject predicate object graphid END_TRIPLE {my_send_quad($1, $2, $3, $4);};
triple:		subject predicate object END_TRIPLE {my_send_triple($1, $2, $3);};

subject:	IRIREF_T { $$ = new_term($1, NULL, URI); }
		| BNODE_T { $$ = new_term($1, NULL, BNODE); };

predicate:	IRIREF_T ;

object:		IRIREF_T { $$ = new_term($1, NULL, URI); }
		| BNODE_T { $$ = new_term($1 , NULL, BNODE); }
		| langliteral
		| typedliteral ;

graphid:	IRIREF_T { $$ = new_term($1, NULL, URI); }
		| BNODE_T { $$ = new_term($1, NULL, BNODE); };

langliteral:	STRING_T ATSIGN STRING_T { $$ = new_term($1, $3, LANGLITERAL); } ;

typedliteral:	STRING_T DOUBLECARET IRIREF_T { $$ = new_term($1, $3, TYPEDLITERAL); }
		| STRING_T { $$ = new_term($1, NULL, TYPEDLITERAL); } ;

%%

int yyerror(char *s)
{
	extern int yylineno;	// defined and maintained in lex.c
	extern char *yytext;	// defined and maintained in lex.c
	fprintf(stderr, "ERROR at symbol %s on line %d\n", yytext, yylineno);
	return 1;
}

int nquads_parse(const char* input, TripleHandler* triplehandler, void* user)
{
	int err;
	extern FILE* yyin;
	FILE* myfile;
	if (triplehandler == NULL) return 1;
	myfile = fmemopen(input, strlen(input), "r");
	if (myfile == NULL) return 1;
	my_triplehandler = triplehandler;
	my_triplehandler_context = user;
	yyin = myfile;
	err = yyparse();
	nquads_scanner_tidy_up();
	fclose(myfile);
	return err;
}

int nquads_parse_file(const char* filename, TripleHandler* triplehandler, void* user)
{
	int err;
	FILE* myfile;
	extern FILE* yyin;
	if (triplehandler == NULL) return 1;
	my_triplehandler = triplehandler;
	my_triplehandler_context = user;
	myfile = fopen(filename, "r");
	if (myfile == NULL){
		fprintf(stderr, "nquads_parser failed to open inputfile\n");
		return 1;
	}
	yyin = myfile;
	err = yyparse();
	nquads_scanner_tidy_up();
	fclose(myfile);
	return err;
}

void my_send_triple(Term* subject, char* predicate, Term* object){
	my_triplehandler(
		subject->value, subject->type,
		predicate,
		object->value, object->suffix, object->type,
		NULL, BNODE, my_triplehandler_context);
	free(subject);
	free(object);
}

void my_send_quad(Term* subject, char* predicate, Term* object, Term* graph){
	my_triplehandler(
		subject->value, subject->type,
		predicate,
		object->value, object->suffix, object->type,
		graph->value, graph->type, my_triplehandler_context);
	free(subject);
	free(object);
	free(graph);
}

Term* new_term(char* value, char* suffix, TERMTYPE type){
	Term* new = malloc(sizeof(Term));
	new->value = value;
	new->suffix = suffix;
	new->type = type;
	return new;
}

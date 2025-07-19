/*
 *  compact-regex.h: C89/ANSI-C compatible extension for the regex.h POSIX/GNU regular expression library from 1992
 *
 *  Sources: 
 *      regex.h:                       https://pubs.opengroup.org/onlinepubs/7908799/xsh/regex.h.html
 *      POSIX Regular Expressions:     https://pubs.opengroup.org/onlinepubs/009696899/basedefs/xbd_chap09.html
 *      GNU Regex:                     https://web.mit.edu/gnu/doc/html/regex_toc.html
 *      regex.h-Library-Documentation: Regex (edition 0.12) - Kathryn A. Hargreaves, Karl Berry
 *      regex.h function comments:     https://www.ibm.com/docs/en
 *
 *  Version: 1.0b (06/2025)
*/

#ifndef COMPACT_REGEX_H_
#define COMPACT_REGEX_H_ 1

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __GNUC__
#error "__GNUC__ not defined"
#else

/* Define __USE_GNU to declare GNU extensions that violate the
   POSIX name space rules.  */
#ifdef _GNU_SOURCE
# define __USE_GNU 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/* Memory limiters */
unsigned int MAX_TEXT_LENGTH = 8192;
unsigned int MAX_PATTERN_LENGTH = 1024;
unsigned int MAX_NUM_MATCHES = 1024;
unsigned int MAX_PRINT_TEXT_LENGTH = 512;
unsigned int MAX_FILENAME_LENGTH = 512;

/* Extended option flags */
#define REG_DEFAULT 0
#define REG_GLOBAL (1 << 4)
#define REG_MULTILINE (1 << 5)
#define REG_NOSUBEXP (1 << 6)
#define REG_SUBEXP (1 << 7)

int DEFAULT_REG_FLAGS[7] = {
    REG_GLOBAL,
    REG_EXTENDED,
    REG_NEWLINE
};

/* RegEx-subobject for option flag status*/
typedef struct t_substruct__flags {
    int GLOBAL;
    int EXTENDED;
    int ICASE;
    int MULTILINE;
    int NEWLINE;
    int NOSUB;           /* note: REG_NOSUB is deactivated in the program */
    int NOSUBEXP;
    int SUBEXP;
} cregflags_t;

/* RegEx-subobject for result data of the regular expression matches */
typedef struct t_substruct__match_offsets {
    int number_match;    /* number of the match */
    int number_submatch; /* number of the group or submatch */
    int start;           /* byte offset from string's start to substring's start. */
    int end;             /* byte offset from string's start to substring's end. */
    char* string;        /* string of the sub-expression match */
} cregmatches_t;

typedef struct t_substruct__file {
    FILE* ptr;           /* pointer to the FILE object for reading and writing*/
    char* name;          /* filename string */
    char* content;       /* file content */
    int status;          /* status of fread */
    int length;          /* file-length */
} cregfile_t;

/* regex.h processing status flags */
#define REGLIB_NOT_COMPILED 0
#define REGLIB_COMPILED 1
#define REGLIB_EXECUTED 2
#define REGLIB_CLOSED 3
#define REGLIB_ERROR 4

/* RegEx-subobject for internal regex.h references */
typedef struct t_substruct__regex_h_ref {
    int reglib_status;         /* status of regex.h memory */
    int reglib_flags;          /* option flags value */
    regex_t compiled_regex;    /* pointer to compiled regular expression */
    regmatch_t* match_offsets; /* array of offsets matching the corresponding sub-expressions in preg. */
} regex_h_ref;

/* Status flags for regex_compile and regex_exec */
#define REGEX_ERROR -1
#define REGEX_INIT_SUCCESS 5
#define REGEX_COMP_SUCCESS 0
#define REGEX_MATCH_SUCCESS 0
#define REGEX_MATCH_OFFSET_DIST 1

/* Holds the result data of the regular expresion matches and some internal regegex.h references. */
typedef struct t_struct__compact_regex {
    cregflags_t flags;          /* status of option flags */
    cregmatches_t* matches;     /* array with the match start and end string positions and the substring */
    cregfile_t* file;
    int num_matches;            /* number of matches */
    int num_pattern_subexpr;    /* number of corresponding sub-expressions */
    int return_code;            /* return code of the expression string compilation */
    char* text;                 /* the regular expression input text string */
    char* pattern;              /* the regular expression string pattern */
    char error_message[128];    /* error message buffer */
    regex_h_ref regex_h;        /* reference to internal regex.h-variables */
} compact_regex_t;

typedef compact_regex_t* RegEx;
typedef cregfile_t* RegExFile;

/* (Internal) Memory allocation */
static void* __MALLOC(size_t MEM_SIZE);

/* (Internal) Memory re-allocation */
static void *__REALLOC(void* ptr, size_t MEM_SIZE);

/* (Internal) Memory de-allocation */
static void __FREE(void* ptr);

#define ASSERT_TYPE_PTR 0
#define ASSERT_TYPE_INT 1
#define ASSERT_TYPE_STRUCT 2

/* (Internal) Function parameter check */
static void __ASSERT_PARAM(void* parameter, char* name, int TYPE);

#define REPLACE_NO_BRACKETS 0
#define REPLACE_SET_BRACKETS 1
#define REPLACE_LOCAL 0
#define REPLACE_GLOBAL 1

/* (Internal) Replaces the character class with a compatible substring. */
static char* __SUBSTITUTE_STRING(char* input_text_string, char* search_substring, char* replace_substring, int SET_BRACKETS, int GLOBAL, int start_position);

/* (Internal) Converts the character classes in the regular expression. */
static char* _COMPILE__CONVERT_SEQUENCES(char* regex_pattern_string);

/* (Internal) Initializes a RegEx object. */
static RegEx _COMPILE__INIT_REGEX(char* regex_pattern_string, int OPTION_FLAGS);

/* (Internal) Copies the strings from regex_h of regexec into RegEx-subobject. */
static void _EXEC__GET_MATCHED_STRINGS(char* input_text_string, RegEx regex);

/* (Internal) Searches for the first occurence of the regular expression pattern in the input text string. */
static int _EXEC__SEARCH_LOCAL(char* input_text_string, RegEx regex);

/* (Internal) Searches for all occurences of the regular expression pattern in the input text string. */
static int _EXEC__SEARCH_GLOBAL(char* input_text_string, RegEx regex);

static char* _REPLACE__PREPROCESS_GROUPS(RegEx regex_data, char* replace_substring, int i);

/* (Intenal) Concatinates substrings of option flag names to the option falgs string */
static char* _PRINT__CONCAT_OPTION_FLAGS(RegEx regex_data);

/* (Internal) Processes the input text string for printing or writing  */
static char* _PRINT__GET_TEXT_STRING(RegEx regex_data);

/* (Internal) Processes the data string of the RegEx Object for printing or writing  */
static char* _PRINT__GET_REGEX_STATS(RegEx regex_data, char* option_flags_string);

/* (Internal) Processes the result data string of the RegEx Object for printing or writing  */
static char* _PRINT__GET_RESULTS(RegEx regex_data, int PRINT_LAYOUT, int print_header, int print_pos);

/* (Internal) Processes the complete string (text + data + results) for printing or writing  */
static char* _PRINT__GET_OUTPUT_STRING(RegEx regex_data, int PRINT_LAYOUT);

/* Sets the default REG_ flags for regex_compile() and regcomp() */
void set_default_reg_flags(int OPTION_FLAGS);

/* Compiles a regular expression pattern with given option flags. */
extern RegEx regex_compile(char* regex_pattern_string, int OPTION_FLAGS);

/* Writes the error code message of regerror() into the error message buffer
   and prints the error message to stderr.  */
extern int regex_error(RegEx regex);

/* Executes a compiled regular expression pattern and compares it with a given text input string. */
extern int regex_exec(char* input_text_string, RegEx regex);

/* Frees the memory of allocated regex.h buffers and sets the regfree status to 1. */
extern void regex_close(RegEx regex);

/* Compiles a regular expression pattern with given option flags, executes the
   regular expression and returns the RegEx Object with the regular expression results. */
extern RegEx regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS);

/* Replaces regular expression matches with a substring. */
extern char* regex_replace(char* input_text_string, char* regex_pattern_string, char* replace_substring, int OPTION_FLAGS);

/* Reads a file and matches its contents. */
extern RegExFile regex_readfile(char* file_name);

/* Frees the allocated memory for a input file. */
extern void regex_closefile(RegExFile regex_file);

/* Writes the contents of a RegEx Object into a file. */
extern int regex_writefile(RegEx regex_data, int PRINT_LAYOUT, char* file_name);

/* int regex_writefile_string(char*, char*): Writes a string into a file. */
extern int regex_writefile_string(char* output_string, char* file_name);

/* Option flags for regex_print */
#define REGEX_PRINT_NONE -1
#define REGEX_PRINT_TABLE (1 << 1)
#define REGEX_PRINT_LIST (1 << 2)
#define REGEX_PRINT_LIST_FULL (1 << 3)
#define REGEX_PRINT_PLAIN (1 << 4)
#define REGEX_PRINT_CSV (1 << 5)
#define REGEX_PRINT_JSON (1 << 6)
#define REGEX_PRINT_EXT (1 << 7)

#define REGEX_PRINT_FULLTEXT (1 << 8)
#define REGEX_PRINT_NOTEXT (1 << 9)
#define REGEX_PRINT_NOSTATS (1 << 10)
#define REGEX_PRINT_NORESULTS (1 << 11)
#define REGEX_PRINT_NOINDEX (1 << 12)
#define REGEX_PRINT_FILTER (1 << 13)

unsigned int PRINT_COLORED = 0;

/* Prints the input text and the regular expression results and contents of a
   RegEx Object as a table or as a list. */
extern void regex_print(RegEx regex, int PRINT_LAYOUT);


/*** Internal linking ***/
#include "compact-regex.c"

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* COMPACT_REGEX_H_ */

#endif /* GNU_C */
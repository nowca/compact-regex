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

#include "compact-regex.h"


/* (Internal) Memory allocation */
static void* __MALLOC(size_t MEM_SIZE)
{
    void* ptr = malloc(MEM_SIZE);

    if (ptr == NULL && MEM_SIZE)
    {
        fprintf(stderr,"Error: Could not allocate memory (%ld bytes)\n", MEM_SIZE);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/* (Internal) Memory re-allocation */
static void *__REALLOC(void* ptr, size_t MEM_SIZE)
{
    ptr = realloc(ptr, MEM_SIZE);

    if (ptr == NULL && MEM_SIZE)
    {
        fprintf(stderr,"Error: Could not allocate memory (%ld bytes)\n", MEM_SIZE);
        exit(EXIT_FAILURE);
    }

   return ptr;
}

/* (Internal) Memory de-allocation */
static void __FREE(void* ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = NULL;
    }
    else
    {
        fprintf(stderr,"Error: Could not free memory\n");
        exit(EXIT_FAILURE);
    }
}

/* (Internal) Function parameter check */
static void __ASSERT_PARAM(void* parameter, char* name, int TYPE)
{
    if (parameter == NULL)
    {
        fprintf(stderr,"Error: Missing parameter \"%s\".\n", name);
        exit(EXIT_FAILURE);
    }
    if (TYPE == 0 && strlen(parameter) == 0)
    {
        fprintf(stderr,"Error: Parameter \"%s\" is empty.\n", name);
        exit(EXIT_FAILURE);
    }
}

/* (Internal) Replaces the character class with a compatible substring. */
static char* __SUBSTITUTE_STRING(char* input_text_string, char* search_substring, char* replace_substring, int SET_BRACKETS, int TYPE, int start_position)
{
    const int STRLEN_INPUT = strlen(input_text_string);
    const int STRLEN_SEARCH = (int)strlen(search_substring);
    const int STRLEN_REPLACE = (int)strlen(replace_substring);
    const int MAX_STRLEN = MAX_TEXT_LENGTH + 1;

    char* ptr_pos = 0;
    char* output_string = __MALLOC(MAX_STRLEN * sizeof(char));
    char* temp_string = __MALLOC(MAX_STRLEN * sizeof(char));

    if (STRLEN_INPUT < MAX_STRLEN)
    {
        int index = -1;
        int IS_NOT_IN_LIST = 0;;

        strcpy(output_string, input_text_string);

        while (1)
        {
            /* set index = 0 if first iteration, else add replaced length */
            index = index == -1 ? start_position : index + STRLEN_REPLACE;

            /* get new pointer position of next search substring match position */
            ptr_pos = strstr(output_string + index, search_substring);

            /* break if no match anymore */
            if (ptr_pos == NULL)
            {
                break;
            };

            strcpy(temp_string, output_string);
            index = ptr_pos - output_string;

            /* check if pointer position is in list */
            if (SET_BRACKETS && index == 0)
            {
                IS_NOT_IN_LIST = 1;
            }
            else if (index > 0)
            {
                IS_NOT_IN_LIST = (output_string[index - 1] != '[' && output_string[index - 1] != '|');
            };

            /* set opening bracket around substitution if it is not in list */
            if (SET_BRACKETS == 1 && IS_NOT_IN_LIST)
            {
                output_string[index] = '[';
                SET_BRACKETS = 2;
                index++;
            }

            output_string[index] = '\0';
            strcat(output_string, replace_substring);
            
            /* set closing bracket around substitution if it is not in list */
            if (SET_BRACKETS == 2)
            {
                output_string[index + STRLEN_REPLACE] = ']';
                output_string[index + STRLEN_REPLACE + 1] = '\0';
                index--;
            }

            /* add rest of string to the output buffer */
            ptr_pos = temp_string + index + STRLEN_SEARCH;
            strcat(output_string, ptr_pos);
            
            /* add two index positions of additional brackets */
            if (SET_BRACKETS == 2)
            {
                index += 2;
                SET_BRACKETS = 1;
            }

            output_string[index + STRLEN_REPLACE + (int)strlen(ptr_pos)] = '\0';

            if (TYPE != REPLACE_GLOBAL)
            {
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: Substitution exceeds MAX_STRLEN: %d.\n%s", MAX_STRLEN, input_text_string);
        exit(EXIT_FAILURE);
    }

    __FREE(input_text_string);
    __FREE(temp_string);

    return output_string;
}

/* (Internal) Converts the character classes in the regular expression. */
static char* _COMPILE__CONVERT_SEQUENCES(char* regex_pattern_string)
{
    struct replace_substrings { char* search; char* replace; };
    struct replace_substrings replace_substring_list[12] = {
        { "\\w", "A-Za-z0-9_" },    /* Alphanumeric characters plus "_" */
        { "\\W", "^A-Za-z0-9_" },   /* Non-word characters */
        { "\\a", "A-Za-z" },        /* Alphabetic characters */
        { "\\s", " " },             /* Space */
        { "\\t", "   " },           /* Space and tab */
        { "\\d", "0-9" },           /* Digits */
        { "\\D", "^0-9" },          /* Non-Digits */
        { "\\l", "a-z" },           /* Lowercase letters */
        { "\\p", "\\x20-\\x7E" },   /* Visible characters and the space character */
        { "\\u", "A-Z" },           /* Uppercase letters */
        { "\\r", "\r" },            /* Carriage Return */
        { "\\n", "\\n" },            /* Newline */
    };

    int i;
    char converted_sequence[2][8];
    char* converted_regex_pattern_string = __MALLOC(MAX_PATTERN_LENGTH * sizeof(char));

    strcpy(converted_regex_pattern_string, regex_pattern_string);

    /* replace listed substrings */
    for (i = 0; i < 12; i++)
    {
        converted_regex_pattern_string = __SUBSTITUTE_STRING(converted_regex_pattern_string, replace_substring_list[i].search, replace_substring_list[i].replace, REPLACE_SET_BRACKETS, REPLACE_GLOBAL, 0);
    }

    /* convert ascii sequences given as arguments */
    for (i = 33; i < 128; i++)
    {
        sprintf(converted_sequence[0], "\\x%x", i);
        converted_sequence[1][0] = i;
        converted_sequence[1][1] = '\0';
        converted_regex_pattern_string = __SUBSTITUTE_STRING(converted_regex_pattern_string, converted_sequence[0], converted_sequence[1], REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
    }

    return converted_regex_pattern_string;
}

/* (Internal) Initializes a RegEx object. */
static RegEx _COMPILE__INIT_REGEX(char* regex_pattern_string, int OPTION_FLAGS)
{ 
    int i = 0;

    /* initialize RegEx */
    RegEx regex_data = __MALLOC(sizeof(compact_regex_t));

    __ASSERT_PARAM(regex_pattern_string, "regex_pattern_string", ASSERT_TYPE_PTR);

    /* get default flags */
    for (i = 0; i < (int)(sizeof(DEFAULT_REG_FLAGS) / sizeof(DEFAULT_REG_FLAGS[0])); i++)
    {
        if ((OPTION_FLAGS & DEFAULT_REG_FLAGS[i]) != DEFAULT_REG_FLAGS[i])
        {
            OPTION_FLAGS += DEFAULT_REG_FLAGS[i];
        }
    }

    regex_data->file = NULL;
    
    /* set options */
    regex_data->flags.GLOBAL = ((OPTION_FLAGS & REG_GLOBAL) == REG_GLOBAL);
    regex_data->flags.EXTENDED = ((OPTION_FLAGS & REG_EXTENDED) == REG_EXTENDED);
    regex_data->flags.ICASE = ((OPTION_FLAGS & REG_ICASE) == REG_ICASE);
    regex_data->flags.NOSUB = ((OPTION_FLAGS & REG_NOSUB) == REG_NOSUB);
    regex_data->flags.NOSUBEXP = ((OPTION_FLAGS & REG_NOSUBEXP) == REG_NOSUBEXP);
    regex_data->flags.NEWLINE = ((OPTION_FLAGS & REG_NEWLINE) == REG_NEWLINE);
    regex_data->flags.MULTILINE = ((OPTION_FLAGS & REG_MULTILINE) == REG_MULTILINE);
    regex_data->flags.SUBEXP = ((OPTION_FLAGS & REG_SUBEXP) == REG_SUBEXP);

    /* deactivate REG_NEWLINE if REG_MULTILINE is set, to catch newline-characters */
    if (regex_data->flags.MULTILINE == 1 && regex_data->flags.NEWLINE == 1)
    {
        regex_data->flags.NEWLINE = 0;
        OPTION_FLAGS -= REG_NEWLINE;
    }

    /* disable NOSUB */
    if (regex_data->flags.NOSUB == 1)
    {
        printf("REG_NOSUB is set. Ignoring NO_SUB option.\n\n");
        OPTION_FLAGS -= REG_NOSUB;
    }

    if (regex_data->flags.NOSUBEXP == 1 && regex_data->flags.SUBEXP == 1)
    {
        printf("REG_NOSUBEXP is set. Ignoring REG_SUBEXP option.\n\n");
        regex_data->flags.SUBEXP = 0;
        OPTION_FLAGS -= REG_SUBEXP;
    }

    /* copy option flags */
    regex_data->regex_h.reglib_flags = OPTION_FLAGS;

    /* initialize members */
    regex_data->num_matches = 0;
    regex_data->num_pattern_subexpr = 0;
    regex_data->return_code = REGEX_INIT_SUCCESS;
    regex_data->text = __MALLOC(16 * sizeof(char));
    strcpy(regex_data->text, "<No input text>");
    regex_data->pattern = regex_pattern_string;
    strcpy(regex_data->error_message, "");
    regex_data->regex_h.reglib_status = REGLIB_NOT_COMPILED;
    
    return regex_data;
}

/* (Internal) Copies the strings from regex_h of regexec into RegEx-subobject. */
static void _EXEC__GET_MATCHED_STRINGS(char* input_text_string, RegEx regex_data)
{
    int i;
    char* temp_string;

    for (i = 0; i < regex_data->num_matches; i++)
    {
        int STR_SIZE = regex_data->regex_h.match_offsets[i].rm_eo - regex_data->regex_h.match_offsets[i].rm_so;

        regex_data->matches[i].start = (int)regex_data->regex_h.match_offsets[i].rm_so;
        regex_data->matches[i].end = (int)regex_data->regex_h.match_offsets[i].rm_eo;
        regex_data->matches[i].string = __MALLOC((STR_SIZE + 1) * sizeof(char));

        /* copy substring to temp string */
        temp_string = __MALLOC((strlen(input_text_string) + 1) * sizeof(char));
        strcpy(temp_string, input_text_string + regex_data->matches[i].start);
        temp_string[STR_SIZE] = '\0';

        /* copy temp string to match string */
        strcpy(regex_data->matches[i].string, temp_string);
        regex_data->matches[i].string[STR_SIZE] = '\0';

        __FREE(temp_string);
    }
}

/* (Internal) Searches for the first occurence of the regular expression pattern in the input text string. */
static int _EXEC__SEARCH_LOCAL(char* input_text_string, RegEx regex_data)
{
    int i = 0;
    int ITEM_NUMBER = 0;
    int MATCH_NUMBER = 0;
    int return_code = -1;

    /* int regexec(const regex_t * preg, const char*  string, size_t nmatch, regmatch_t * pmatch, int eflags);
    -------------------------------------------------------------------------------------------------------
    Compares the NULL-terminated string specified by string against the compiled regular expression, preg. 

    preg:   is a pointer to a compiled regular expression to compare against STRING.
    string: is a pointer to a string to be matched.
    nmatch: is the number of sub-expressions to match.
    pmatch: is an array of offsets into STRING which matched the corresponding sub-expressions in preg.
            typedef struct
            {
              regoff_t rm_so;  // Byte offset from string's start to substring's start.
              regoff_t rm_eo;  // Byte offset from string's start to substring's end.
            } regmatch_t;

    eflags: is a bit flag defining customizable behavior of regexec():
            REG_NOTBOL: Indicates that the first character of STRING is not the beginning of the line.
            REG_NOTEOL: Indicates that the first character of STRING is not the end of the line. 

    return: if a match is found, regexec() returns 0. */
    regmatch_t* match_iteration = __MALLOC(MAX_NUM_MATCHES * sizeof(regmatch_t));        /* match of one iteration */
    regex_data->regex_h.match_offsets = __MALLOC(MAX_NUM_MATCHES * sizeof(regmatch_t));
    regex_data->matches = __MALLOC(MAX_NUM_MATCHES * sizeof(cregmatches_t));

    /*return_code = regexec(&(regex_data->regex_h.compiled_regex), input_text_string, MAX_NUM_MATCHES, (regmatch_t*)regex_data->regex_h.match_offsets, 0);*/

    return_code = regexec(&(regex_data->regex_h.compiled_regex), input_text_string, MAX_NUM_MATCHES, (regmatch_t*)match_iteration, 0);

    /* copy matched strings */
    if (return_code == REGEX_MATCH_SUCCESS)
    {
        /* next match successful */
        MATCH_NUMBER += 1;

        /* get number of matches */
        for (i = 0; i < MAX_NUM_MATCHES; i++)
        {
            /* some library bug on (a)|(b) matching second group at next element instead of this */
            if (match_iteration[i].rm_so == (size_t)-1)
            {
                break;
            }

            /* if this match is valid */
            if (match_iteration[i].rm_so != (size_t)-1)
            { 
                /* set the item number to one lesser, if previous match was invalid, to step over invalid matches */
                if (i > 0 && match_iteration[i-1].rm_so == -1)
                {
                    ITEM_NUMBER = i - 1;
                }
                else
                {
                    ITEM_NUMBER = i;
                }

                regex_data->matches[ITEM_NUMBER].number_match = MATCH_NUMBER;
                regex_data->matches[ITEM_NUMBER].number_submatch = regex_data->flags.SUBEXP == 0 ? ITEM_NUMBER : ITEM_NUMBER + 1;

                if ((regex_data->flags.SUBEXP == 0 && regex_data->flags.NOSUBEXP == 0) ||                       /* add every result, if SUBEXP = 0 */
                    (regex_data->flags.SUBEXP == 0 && regex_data->flags.NOSUBEXP == 1 && ITEM_NUMBER == 0) ||   /* add just the main matches, if NOSUBEXP = 1 */
                    (regex_data->flags.SUBEXP == 1 && ITEM_NUMBER > 0))                                    
                {
                    memcpy(&regex_data->regex_h.match_offsets[regex_data->num_matches], &match_iteration[i], sizeof(regmatch_t));
                    regex_data->num_matches++;
                }
            }
        }

        __FREE(match_iteration);

        /* resize regex.h buffer from MAX_NUM_MATCHES to num_matches + 1 */
        regex_data->regex_h.match_offsets = __REALLOC(regex_data->regex_h.match_offsets, (regex_data->num_matches + 1) * sizeof(regmatch_t));

        /* copy substrings from input text */
        _EXEC__GET_MATCHED_STRINGS(input_text_string, regex_data);
        regex_data->regex_h.reglib_status = REGLIB_EXECUTED;
    }

    return return_code;
}

/* (Internal) Searches for all occurences of the regular expression pattern in the input text string. */
static int _EXEC__SEARCH_GLOBAL(char* input_text_string, RegEx regex_data)
{
    int return_code_local = -1;         /* return code for loop */
    int return_code_global = -1;        /* return code for function */ 

    int i = 0;                          /* loop counters */
    int j = 0;       
    int BREAK = 0;                      /* loop break flag */
    
    int READ_POS_OFFSET = 0;            /* read position in input string */
    int NEXT_VALID_MATCH_DISTANCE = 0;  /* distance to next valid match in regmatch_t array */
    
    int NUMBER_MATCH = 0;
    int NUMBER_GROUP = 0;
    int NUMBER_SUBMATCH = 0;

    regmatch_t* match_iteration = __MALLOC(MAX_NUM_MATCHES * sizeof(regmatch_t));        /* match of one iteration */
    regex_data->regex_h.match_offsets = __MALLOC(MAX_NUM_MATCHES * sizeof(regmatch_t)); /* matches of all iterations */
    regex_data->matches = __MALLOC(MAX_NUM_MATCHES * sizeof(cregmatches_t));

    /* full initialize regmatch_t array fields */
    for (i = 0; i < MAX_NUM_MATCHES; i++)
    {
        match_iteration[i].rm_so = -1;
        match_iteration[i].rm_eo = -1;
        
    }

    /* fix segmentation fault in _REPLACE__PREPROCESS_GROUPS */
    for (i = 0; i <= 99; i++)
    {
        regex_data->matches[i].number_match = 0;
        regex_data->matches[i].number_submatch = 0;
    }

    /* get next global match in: input string + read position offset */
    while (1)
    {
        if (regex_data->return_code != REGEX_ERROR)
        {
            return_code_local = regexec(&regex_data->regex_h.compiled_regex, input_text_string + READ_POS_OFFSET, MAX_NUM_MATCHES, (regmatch_t*)match_iteration, 0);
        }

        /* copy single iteration match into regex_h matches, until no matches found */
        if (return_code_local == REGEX_MATCH_SUCCESS)
        {
            /* set function return code to 0 */
            return_code_global = REGEX_MATCH_SUCCESS;

            /* this match successful */
            NUMBER_MATCH += 1;

            /* copy matches into regex.h buffer */
            /* for each match found in regmatch_t array */
            for (i = 0; i <= regex_data->num_pattern_subexpr; i++)
            {
                /* for each next match ahead */
                for (j = i + 1; j <= regex_data->num_pattern_subexpr; j++)
                {
                    /* if first match /= set top match */
                    if (i == 0)
                    {
                        NUMBER_GROUP = 0;
                    }
                    if (i == 1 && return_code_local == REGEX_MATCH_OFFSET_DIST)
                    {
                        NUMBER_GROUP = NEXT_VALID_MATCH_DISTANCE;
                    }
                    
                    /* if one of the next matches is valid -> break */
                    if (match_iteration[j].rm_so != (size_t) - 1)
                    {   
                        BREAK = 0;
                        NEXT_VALID_MATCH_DISTANCE = j - i;
                     
                        /* if one of the next valid matches is more away, instead the next one */
                        if (NEXT_VALID_MATCH_DISTANCE > 1)
                        {
                            return_code_local = REGEX_MATCH_OFFSET_DIST;
                        }

                        break;
                    }
                    
                    /* break if maximum next possible matches checked */
                    if (j == regex_data->num_pattern_subexpr)
                    {
                        BREAK = 1;
                    }
                }

                /* -> break if match is invalid and no valid matches ahead */
                if (match_iteration[i].rm_so == (size_t)-1 && BREAK == 1)
                {
                    break; 
                }

                /* if this match is valid */
                if (match_iteration[i].rm_so != -1)
                {
                    /* get start and end positions in original string */
                    match_iteration[i].rm_so += READ_POS_OFFSET;  /* add last read offset to start position */
                    match_iteration[i].rm_eo += READ_POS_OFFSET;  /* add last read offset to end position */

                    NUMBER_SUBMATCH = (return_code_local == REGEX_MATCH_OFFSET_DIST) ? NUMBER_GROUP : i;

                    /* set the match number and group number */
                    regex_data->matches[regex_data->num_matches].number_match = NUMBER_MATCH;
                    regex_data->matches[regex_data->num_matches].number_submatch = NUMBER_SUBMATCH;

                    if ((regex_data->flags.SUBEXP == 0 && regex_data->flags.NOSUBEXP == 0) ||                         /* add every result, if SUBEXP = 0 */
                        (regex_data->flags.SUBEXP == 0 && regex_data->flags.NOSUBEXP == 1 && NUMBER_SUBMATCH == 0) || /* add just the main matches, if NOSUBEXP = 1 */
                        (regex_data->flags.SUBEXP == 1 && NUMBER_SUBMATCH > 0))                                    
                    {
                        /* copy match data */
                        memcpy(&regex_data->regex_h.match_offsets[regex_data->num_matches], &match_iteration[i], sizeof(regmatch_t));

                        if (regex_data->num_matches < MAX_NUM_MATCHES)
                        {
                            regex_data->num_matches++;
                        }
                        else
                        {
                            regex_data->return_code = REGEX_ERROR;
                            return_code_local = REGEX_ERROR;
                            fprintf(stderr, "regex_exec() error: Number of matches exceeds maximum numer of MAX_NUM_MATCHES: %d.\n", MAX_NUM_MATCHES);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            break;
        }

        /* end position of match = next read positon */
        READ_POS_OFFSET = READ_POS_OFFSET + (match_iteration[0].rm_eo - READ_POS_OFFSET);

        /* if "^" or "$": only start or end of string -> break */
        if (READ_POS_OFFSET == strlen(input_text_string) ||
            match_iteration[0].rm_so == match_iteration[0].rm_eo)
        {
            break;
        }
    }

    __FREE(match_iteration);

    /* resize regex.h buffer from MAX_NUM_MATCHES to num_matches + 1 */
    regex_data->regex_h.match_offsets = __REALLOC(regex_data->regex_h.match_offsets, (regex_data->num_matches + 1) * sizeof(regmatch_t));

    /* copy substrings from input text with match_offsets data */
    if (return_code_local != REGEX_ERROR && return_code_global == REGEX_MATCH_SUCCESS)
    {
        _EXEC__GET_MATCHED_STRINGS(input_text_string, regex_data);
        regex_data->regex_h.reglib_status = REGLIB_EXECUTED;
    }
    else
    {
        return_code_global = return_code_local;
    }

    return return_code_global;
}

/* (Internal) Replaces all group classifiers in a replacement string (\1, \2...) with the matched groups of a regular expression. */
static char* _REPLACE__PREPROCESS_GROUPS(RegEx regex_data, char* replace_substring, int match_number)
{
    int n = 0;
    int group_classifier = 1;
    int top_match = 0;

    char* converted_replace_substring = __MALLOC(MAX_PATTERN_LENGTH * sizeof(char));
    char* temp_string_1 = __MALLOC(MAX_PATTERN_LENGTH * sizeof(char));
    char* temp_string_2 = __MALLOC(8 * sizeof(char));

    strcpy(converted_replace_substring, replace_substring);

    /* \1 ... \99 */
    for (group_classifier = 1; group_classifier <= 99; group_classifier++)
    {
        n = match_number + group_classifier;

        strcpy(temp_string_1, "\\");
        sprintf(temp_string_2, "%d", group_classifier);
        strcat(temp_string_1, temp_string_2);

        if (top_match == 0)
        {
            if (regex_data->matches[n].number_submatch == 0)
            {
                top_match = 1;
            }
            else
            {
                converted_replace_substring = __SUBSTITUTE_STRING(converted_replace_substring, temp_string_1, regex_data->matches[n].string, REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0); 
            }
        }
        if (top_match == 1)
        {
            converted_replace_substring = __SUBSTITUTE_STRING(converted_replace_substring, temp_string_1, "", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
        }
    }

    __FREE(temp_string_1);
    __FREE(temp_string_2);

    return converted_replace_substring;
}

/* (Intenal) Concatinates substrings of option flag names to the option falgs string */
static char* _PRINT__CONCAT_OPTION_FLAGS(RegEx regex_data)
{
    char* option_flags_string = __MALLOC(128 * sizeof(char));
    strcpy(option_flags_string, "");

    if (regex_data->flags.GLOBAL == 1)
    {
        strcpy(option_flags_string, "REG_GLOBAL");
    }
    if (regex_data->flags.EXTENDED == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_EXTENDED");
    }
    if (regex_data->flags.ICASE == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_ICASE");
    }
    if (regex_data->flags.MULTILINE == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_MULTILINE");
    }
    if (regex_data->flags.NEWLINE == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_NEWLINE");
    }
    if (regex_data->flags.NOSUB == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "!(REG_NOSUB)");
    }
    if (regex_data->flags.NOSUBEXP == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_NOSUBEXP");
    }
    if (regex_data->flags.SUBEXP == 1)
    {
        if (strcmp(option_flags_string, ""))
        {
            strcat(option_flags_string, " | ");
        }
        strcat(option_flags_string, "REG_SUBEXP");
    }

    return option_flags_string;
}

/* (Internal) Processes the input text string for printing or writing  */
static char* _PRINT__GET_TEXT_STRING(RegEx regex_data)
{
    char* text_string;
    char* temp_string;

    if (MAX_PRINT_TEXT_LENGTH > 0)
    {
        text_string = __MALLOC((MAX_PRINT_TEXT_LENGTH + 512) * sizeof(char));
        temp_string = __MALLOC((MAX_PRINT_TEXT_LENGTH + 512) * sizeof(char));

        if (strlen(regex_data->text) > MAX_PRINT_TEXT_LENGTH)
        {
            memcpy(temp_string, regex_data->text, MAX_PRINT_TEXT_LENGTH * sizeof(char));
        }
        else
        {
            memcpy(temp_string, regex_data->text, strlen(regex_data->text) * sizeof(char));
        }
    }
    else
    {
        text_string = __MALLOC((MAX_TEXT_LENGTH + 512) * sizeof(char));
        temp_string = __MALLOC((MAX_TEXT_LENGTH + 512) * sizeof(char));
    }

    if (MAX_PRINT_TEXT_LENGTH > 0 && (strlen(regex_data->text) > MAX_PRINT_TEXT_LENGTH))
    {
        if (PRINT_COLORED == 1)
        {
            sprintf(text_string, "\033[34mText (first %d characters):\033[0m\n", MAX_PRINT_TEXT_LENGTH);
        }
        else
        {
            sprintf(text_string, "Text (first %d characters):\n", MAX_PRINT_TEXT_LENGTH);
        }
        
        temp_string[MAX_PRINT_TEXT_LENGTH] = '\0';

        if (PRINT_COLORED == 1)
        {
            strcat(temp_string, "\033[90m[...]\n[...]\033[0m");
        }
        else
        {
            strcat(temp_string, "[...]\n[...]");
        }

        strcat(text_string, temp_string);
    }
    else
    {
        text_string = __REALLOC(text_string, (MAX_TEXT_LENGTH + 512) * sizeof(char));

        if (PRINT_COLORED == 1)
        {
            sprintf(text_string, "\033[34mText:\033[0m\n");
        }
        else
        {
            sprintf(text_string, "Text:\n");
        }
        
        strcat(text_string, regex_data->text);
    }

    if (PRINT_COLORED == 1)
    {
        sprintf(temp_string, "%s\n\n\033[36mString-length:\033[0m    %d\n\n", text_string, (int)strlen(regex_data->text));
    }
    else
    {
        sprintf(temp_string, "%s\n\nString-length:    %d\n\n", text_string, (int)strlen(regex_data->text));
    }
   
    strcpy(text_string, temp_string);

    if (MAX_PRINT_TEXT_LENGTH > 0)
    {
        text_string = __REALLOC(text_string, (MAX_PRINT_TEXT_LENGTH + MAX_FILENAME_LENGTH + 512) * sizeof(char));
    }
    else
    {
        text_string = __REALLOC(text_string, (MAX_TEXT_LENGTH + MAX_FILENAME_LENGTH + 512) * sizeof(char));
    }

    __FREE(temp_string);

    return text_string;
}

/* (Internal) Processes the data string of the RegEx Object for printing or writing  */
static char* _PRINT__GET_REGEX_STATS(RegEx regex_data, char* option_flags_string)
{
    char* regex_data_string = __MALLOC((MAX_PATTERN_LENGTH + 512) * sizeof(char));
    char* temp_string = __MALLOC((MAX_PATTERN_LENGTH + 256) * sizeof(char));
    char* filename_string;

    strcpy(temp_string, regex_data->pattern);
    temp_string = __SUBSTITUTE_STRING(temp_string, "\r", "\\r", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
    temp_string = __SUBSTITUTE_STRING(temp_string, "\n", "\\n", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);

    if (PRINT_COLORED == 1)
    {
        sprintf(regex_data_string, "\
\033[34mRegex-Pattern:\033[0m    %s\n\
\033[36mRegex-Flags:\033[0m      %s\n\
\033[36mSub-Expressions:\033[0m  %d\n\
\033[36mTotal Matches:\033[0m    %d\n\
\033[36mReturn Code:\033[0m      %i\n\
\033[36mError Message:\033[0m    %s\n\n",
temp_string,
option_flags_string,
regex_data->num_pattern_subexpr,
regex_data->num_matches,
regex_data->return_code,
regex_data->error_message);
    }
    else
    {
        sprintf(regex_data_string, "\
Regex-Pattern:    %s\n\
Regex-Flags:      %s\n\
Sub-Expressions:  %d\n\
Total Matches:    %d\n\
Return Code:      %i\n\
Error Message:    %s\n\n",
temp_string,
option_flags_string,
regex_data->num_pattern_subexpr,
regex_data->num_matches,
regex_data->return_code,
regex_data->error_message);
    }

    if (regex_data->file != NULL && regex_data->file->length > 0)
    {
        filename_string = __MALLOC((MAX_FILENAME_LENGTH) * sizeof(char));
        regex_data_string = __REALLOC(regex_data_string, (MAX_FILENAME_LENGTH + MAX_PATTERN_LENGTH + 512) * sizeof(char));

        /* shift and prepend */
        if (PRINT_COLORED == 1)
        {
            sprintf(filename_string, "\033[36mFilename:\033[0m         %s\n\n", regex_data->file->name);
        }
        else
        {
            sprintf(filename_string, "Filename:         %s\n\n", regex_data->file->name);
        }
        
        memmove(regex_data_string + strlen(filename_string), regex_data_string, strlen(regex_data_string) + 1);
        memcpy(regex_data_string, filename_string, strlen(filename_string));
    }

    __FREE(option_flags_string);
    __FREE(temp_string);

    return regex_data_string;
}

/* (Internal) Processes the result data string of the RegEx Object for printing or writing  */
static char* _PRINT__GET_RESULTS(RegEx regex_data, int PRINT_LAYOUT, int print_header, int print_position)
{
    int i;
    char temp_str_buffer[4][32];
    char PRINTF_FORMAT_STR[64];
    char* result_string = __MALLOC(256 * sizeof(char));
    char* output_string = __MALLOC(256 * sizeof(char));
    char* substring = __MALLOC(256 * sizeof(char));
    
    strcpy(result_string, "");
    
    if (regex_data->num_matches > 0)
    {
        if (print_header == 1)
        {
            if (PRINT_COLORED == 1)
            {
                sprintf(result_string, "\033[34mResults:\033[0m\n");
            }
            else
            {
                sprintf(result_string, "Results:\n");
            }
        }
        if (PRINT_LAYOUT == REGEX_PRINT_CSV)
        {
            if (print_position)
            {
                if (PRINT_COLORED == 1)
                {
                    strcat(result_string, "\033[36mnumber;match;submatch;start;end;substring;\033[0m\n");
                }
                else
                {
                    strcat(result_string, "number;match;submatch;start;end;substring;\n");
                }
            }
            else
            {
                if (PRINT_COLORED == 1)
                {
                    strcat(result_string, "\033[36mnumber;match;submatch;substring;\033[0m\n");
                }
                else
                {
                    strcat(result_string, "number;match;submatch;substring;\n");
                }
            }       
        }
        if (PRINT_LAYOUT == REGEX_PRINT_TABLE)
        {
            /* count number of digits to set space holder */
            sprintf(temp_str_buffer[0], "%d", regex_data->matches[regex_data->num_matches-1].end);
            
            if (print_position)
            {
                if (PRINT_COLORED == 1)
                {
                    sprintf(output_string, "\033[36m#   Match   Submatch    Start  %*sEnd     Substring\033[0m\n", (int)strlen(temp_str_buffer[0]), " ");
                }
                else
                {
                    sprintf(output_string, "#   Match   Submatch    Start  %*sEnd     Substring\n", (int)strlen(temp_str_buffer[0]), " ");
                }
            }
            else
            {
                if (PRINT_COLORED == 1)
                {
                    sprintf(output_string, "\033[36m#   Match   Submatch     Substring\033[0m\n");
                }
                else
                {
                    sprintf(output_string, "#   Match   Submatch     Substring\n");
                }
            }

            strcat(result_string, output_string);
        }
        if (PRINT_LAYOUT == REGEX_PRINT_JSON)
        {
            output_string = __REALLOC(output_string, (MAX_PATTERN_LENGTH + 256) * sizeof(char));
            result_string = __REALLOC(result_string, (MAX_PATTERN_LENGTH + 256) * sizeof(char));
            substring = __REALLOC(substring, (MAX_PATTERN_LENGTH + 256) * sizeof(char));
            /* temporary: use substring for pattern substitution */
            strcpy(substring, regex_data->pattern);
            substring = __SUBSTITUTE_STRING(substring, "\r", "\\r", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
            substring = __SUBSTITUTE_STRING(substring, "\n", "\\n", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
            substring = __SUBSTITUTE_STRING(substring, "\\", "\\\\", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);

            if (PRINT_COLORED == 1)
            {
                sprintf(output_string, "{\n\
\t\033[36m\"text-length\":\033[0m %d\033[90m,\033[0m\n\
\t\033[36m\"regular expression\":\033[0m \"%s\"\033[90m,\033[0m\n\
\t\033[36m\"sub-expressions\":\033[0m %d\033[90m,\033[0m\n\
\t\033[36m\"total matches\":\033[0m %d\033[90m,\033[0m\n\
\t\033[36m\"matches\":\033[0m",
                (int)strlen(regex_data->text),
                substring,
                regex_data->num_pattern_subexpr,
                regex_data->num_matches);
                strcat(output_string, " [\n");
                strcat(result_string, output_string);
            }
            else
            {
                sprintf(output_string, "{\n\
\t\"text-length\": %d,\n\
\t\"regular expression\": \"%s\",\n\
\t\"sub-expressions\": %d,\n\
\t\"total matches\": %d,\n\
\t\"matches\":",
                (int)strlen(regex_data->text),
                substring,
                regex_data->num_pattern_subexpr,
                regex_data->num_matches);
                strcat(output_string, " [\n");
                strcat(result_string, output_string);
            }
        }

        for (i = 0; i < regex_data->num_matches; i++)
        {
            /* substitute newline characters in substring */
            substring = __REALLOC(substring, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));
            strcpy(substring, regex_data->matches[i].string);
            substring = __SUBSTITUTE_STRING(substring, "\r", "\\r", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);
            substring = __SUBSTITUTE_STRING(substring, "\n", "\\n", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);

            if (PRINT_LAYOUT == REGEX_PRINT_TABLE)
            {
                /* set placeholders for #   Match   Submatch */
                if (i < 9)
                {
                    strcpy(PRINTF_FORMAT_STR, "%d    %4d       %4d");
                }
                if (i >= 9)
                {
                    strcpy(PRINTF_FORMAT_STR, "%d   %4d       %4d");
                }
                if (i >= 99)
                {
                    strcpy(PRINTF_FORMAT_STR, "%d  %4d       %4d");
                }
                if (i >= 999)
                {
                    strcpy(PRINTF_FORMAT_STR, "%d %4d       %4d");
                }

                if (print_position)
                {
                    /* set placeholders for Start  %*sEnd */
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(temp_str_buffer[0], "%d", regex_data->matches[regex_data->num_matches-1].end);
                        strcat(PRINTF_FORMAT_STR, " \033[90m%8d -> %");
                        sprintf(temp_str_buffer[1], "%d", 1 + (int)strlen(temp_str_buffer[0]));
                        strcat(PRINTF_FORMAT_STR, temp_str_buffer[1]);
                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            strcat(PRINTF_FORMAT_STR, "d\033[0m  %c  \033[35m%s\033[0m\n");
                        }
                        else
                        {
                            strcat(PRINTF_FORMAT_STR, "d\033[0m  %c  %s\n");
                        }
                    }
                    else
                    {
                        sprintf(temp_str_buffer[0], "%d", regex_data->matches[regex_data->num_matches-1].end);
                        strcat(PRINTF_FORMAT_STR, " %8d -> %");
                        sprintf(temp_str_buffer[1], "%d", 1 + (int)strlen(temp_str_buffer[0]));
                        strcat(PRINTF_FORMAT_STR, temp_str_buffer[1]);
                        strcat(PRINTF_FORMAT_STR, "d  %c  %s\n");
                    }
                }
                else
                {
                    /* set placeholders for Start  %*sEnd */
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(temp_str_buffer[0], "%d", regex_data->matches[regex_data->num_matches-1].end);
                        strcat(PRINTF_FORMAT_STR, " \033[90m");
                        sprintf(temp_str_buffer[1], " ");
                        strcat(PRINTF_FORMAT_STR, temp_str_buffer[1]);
                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            strcat(PRINTF_FORMAT_STR, "\033[0m%c  \033[35m%s\033[0m\n");
                        }
                        else
                        {
                            strcat(PRINTF_FORMAT_STR, "\033[0m%c  %s\n");
                        }
                    }
                    else
                    {
                        sprintf(temp_str_buffer[0], "%d", regex_data->matches[regex_data->num_matches-1].end);
                        strcat(PRINTF_FORMAT_STR, " ");
                        sprintf(temp_str_buffer[1], " ");
                        strcat(PRINTF_FORMAT_STR, temp_str_buffer[1]);
                        strcat(PRINTF_FORMAT_STR, "%c  %s\n");
                    }
                }

                /* set all values */
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 512) * sizeof(char));
                if (print_position)
                {
                    sprintf(output_string, PRINTF_FORMAT_STR,
                        i + 1,
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        regex_data->matches[i].start,
                        regex_data->matches[i].end,
                        (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0 ? '*' : ' '),
                        substring
                    );
                }
                else
                {
                    sprintf(output_string, PRINTF_FORMAT_STR,
                        i + 1,
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0 ? '*' : ' '),
                        substring
                    );
                }

                /* concatinate strings to table row */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
            else if (PRINT_LAYOUT == REGEX_PRINT_LIST)
            {
                /* set all values */
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));

                if (print_position)
                {
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(temp_str_buffer[0], "\033[32m[%d]\033[0m", i+1);
                        sprintf(temp_str_buffer[1], "\033[90m(%d-%d)\033[0m", regex_data->matches[i].start, regex_data->matches[i].end);

                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            sprintf(output_string, "%-4s %12s: \033[35m%s\033[0m", temp_str_buffer[0], temp_str_buffer[1], substring);
                        }
                        else
                        {
                            sprintf(output_string, "%-4s %12s: %s", temp_str_buffer[0], temp_str_buffer[1], substring);
                        }
                        
                    }
                    else
                    {
                        sprintf(temp_str_buffer[0], "[%d]", i+1);
                        sprintf(temp_str_buffer[1], "(%d-%d)", regex_data->matches[i].start, regex_data->matches[i].end);
                        sprintf(output_string, "%-4s %12s: %s", temp_str_buffer[0], temp_str_buffer[1], substring);
                    }
                }
                else
                {
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(temp_str_buffer[0], "\033[32m[%d]\033[0m", i+1);

                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            sprintf(output_string, "%-4s: \033[35m%s\033[0m", temp_str_buffer[0], substring);
                        }
                        else
                        {
                            sprintf(output_string, "%-4s: %s", temp_str_buffer[0], substring);
                        }
                        
                    }
                    else
                    {
                        sprintf(temp_str_buffer[0], "[%d]", i+1);
                        sprintf(output_string, "%-4s %12s: %s", temp_str_buffer[0], temp_str_buffer[1], substring);
                    }
                }

                
                
                /* add Substring */
                strcat(output_string, "\n");

                /* concatinate all strings to list entry */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
            else if (PRINT_LAYOUT == REGEX_PRINT_LIST_FULL)
            {
                /* set all values and substring*/
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));

                if (print_position)
                {
                    if (PRINT_COLORED == 1)
                    {
                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            sprintf(output_string, "\033[36m[%d]\033[0m\n\033[32mMatch:\033[0m     %d\n\033[32mSubmatch:\033[0m  %d\n\033[32mStart:\033[0m     %d\n\033[32mEnd:\033[0m       %d\n\033[32mSubstring:\033[0m \033[35m%s\033[0m\n",
                                i + 1,
                                regex_data->matches[i].number_match,
                                regex_data->matches[i].number_submatch,
                                regex_data->matches[i].start,
                                regex_data->matches[i].end,
                                regex_data->matches[i].string
                            ); 
                        }
                        else
                        {
                            sprintf(output_string, "\033[36m[%d]\033[0m\n\033[32mMatch:\033[0m     %d\n\033[32mSubmatch:\033[0m  %d\n\033[32mStart:\033[0m     %d\n\033[32mEnd:\033[0m       %d\n\033[32mSubstring:\033[0m %s\n",
                                i + 1,
                                regex_data->matches[i].number_match,
                                regex_data->matches[i].number_submatch,
                                regex_data->matches[i].start,
                                regex_data->matches[i].end,
                                regex_data->matches[i].string
                            ); 
                        }
                        
                    }
                    else
                    {
                        sprintf(output_string, "[%d]\nMatch      %d\nSubmatch:  %d\nStart:     %d\nEnd:       %d\nSubstring: %s\n",
                            i + 1,
                            regex_data->matches[i].number_match,
                            regex_data->matches[i].number_submatch,
                            regex_data->matches[i].start,
                            regex_data->matches[i].end,
                            regex_data->matches[i].string
                        );
                    } 
                }
                else
                {
                    if (PRINT_COLORED == 1)
                    {
                        if (regex_data->num_pattern_subexpr > 0 && regex_data->matches[i].number_submatch == 0)
                        {
                            sprintf(output_string, "\033[36m[%d]\033[0m\n\033[32mMatch:\033[0m     %d\n\033[32mSubmatch:\033[0m  %d\n\033[32mSubstring:\033[0m \033[35m%s\033[0m\n",
                                i + 1,
                                regex_data->matches[i].number_match,
                                regex_data->matches[i].number_submatch,
                                regex_data->matches[i].string
                            ); 
                        }
                        else
                        {
                            sprintf(output_string, "\033[36m[%d]\033[0m\n\033[32mMatch:\033[0m     %d\n\033[32mSubmatch:\033[0m  %d\n\033[32mSubstring:\033[0m %s\n",
                                i + 1,
                                regex_data->matches[i].number_match,
                                regex_data->matches[i].number_submatch,
                                regex_data->matches[i].string
                            ); 
                        }
                        
                    }
                    else
                    {
                        sprintf(output_string, "[%d]\nMatch      %d\nSubmatch:  %d\nSubstring: %s\n",
                            i + 1,
                            regex_data->matches[i].number_match,
                            regex_data->matches[i].number_submatch,
                            regex_data->matches[i].string
                        );
                    } 
                }
                

                if (i < regex_data->num_matches-1)
                {
                    strcat(output_string, "\n");
                }

                /* concatinate all strings to list entry */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
            else if (PRINT_LAYOUT == REGEX_PRINT_PLAIN)
            {
                /* copy substring*/
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));
                sprintf(output_string, "%s\n",
                    regex_data->matches[i].string
                );

                /* concatinate result substring to output */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
            else if (PRINT_LAYOUT == REGEX_PRINT_CSV)
            {
                /* set all values and substring*/
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));

                if (print_position)
                {
                    sprintf(output_string, "%d;%d;%d;%d;%d;%s;\n",
                        i, regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        regex_data->matches[i].start,
                        regex_data->matches[i].end,
                        substring
                    );
                }
                else
                {
                    sprintf(output_string, "%d;%d;%d;%s;\n",
                        i, regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        substring
                    );
                }

                /* concatinate result substring to output */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
            else if (PRINT_LAYOUT == REGEX_PRINT_JSON)
            {
                substring = __SUBSTITUTE_STRING(substring, "\\", "\\\\", REPLACE_NO_BRACKETS, REPLACE_GLOBAL, 0);

                /* set all values and substring*/
                output_string = __REALLOC(output_string, (strlen(regex_data->matches[i].string) + 256) * sizeof(char));

                if (print_position)
                {
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(output_string, "\t\t{\n\
\t\t\t\033[32m\"match-number\":\033[0m %d\033[90m,\033[0m\n\
\t\t\t\033[32m\"group-number\":\033[0m %d\033[90m,\033[0m\n\
\t\t\t\033[32m\"sub-string\":\033[0m \"%s\"\033[90m,\033[0m\n\
\t\t\t\033[32m\"start\":\033[0m %d\033[90m,\033[0m\n\
\t\t\t\033[32m\"end\":\033[0m %d\n",
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        substring,
                        regex_data->matches[i].start,
                        regex_data->matches[i].end);
                    }
                    else
                    {
                        sprintf(output_string, "\t\t{\n\
\t\t\t\"match-number\": %d,\n\
\t\t\t\"group-number\": %d,\n\
\t\t\t\"sub-string\": \"%s\",\n\
\t\t\t\"start\": %d,\n\
\t\t\t\"end\": %d\n",
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        substring,
                        regex_data->matches[i].start,
                        regex_data->matches[i].end);
                    }
                }
                else
                {
                    if (PRINT_COLORED == 1)
                    {
                        sprintf(output_string, "\t\t{\n\
\t\t\t\033[32m\"match-number\":\033[0m %d\033[90m,\033[0m\n\
\t\t\t\033[32m\"group-number\":\033[0m %d\033[90m,\033[0m\n\
\t\t\t\033[32m\"sub-string\":\033[0m \"%s\"\n",
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        substring);
                    }
                    else
                    {
                        sprintf(output_string, "\t\t{\n\
\t\t\t\"match-number\": %d,\n\
\t\t\t\"group-number\": %d,\n\
\t\t\t\"sub-string\": \"%s\"\n",
                        regex_data->matches[i].number_match,
                        regex_data->matches[i].number_submatch,
                        substring);
                    }
                }

                strcat(output_string, "\t\t}");
                
                if (i < regex_data->num_matches-1)
                {
                     if (PRINT_COLORED == 1)
                    {
                        strcat(output_string, "\033[90m,\033[0m\n");
                    }
                    else
                    {
                        strcat(output_string, ",\n");
                    }
                }
                else
                {
                    strcat(output_string, "\n");
                }

                /* concatinate result substring to output */
                result_string = __REALLOC(result_string, (strlen(result_string) + strlen(output_string) + 32) * sizeof(char));
                strcat(result_string, output_string);
            }
        }
        if (PRINT_LAYOUT == REGEX_PRINT_JSON)
        {
            strcat(result_string, "\t]\n}\n");
        }
    }
    else
    {
        if (PRINT_LAYOUT != REGEX_PRINT_CSV &&
            PRINT_LAYOUT != REGEX_PRINT_JSON)
        {
            strcpy(result_string, "\nNo match\n");
        }
        else
        {
            strcpy(result_string, "No match\n");
        }
    }
    
    __FREE(substring);
    __FREE(output_string);
    
    return result_string;
}

/* (Internal) Processes the complete string (text + data + results) for printing or writing  */
static char* _PRINT__GET_OUTPUT_STRING(RegEx regex_data, int PRINT_LAYOUT)
{
    int PRINT_OPTIONS = PRINT_LAYOUT;
    char* output_string;
    char* temp_string;

    /* filter print options from print layout */
    if ((PRINT_LAYOUT & REGEX_PRINT_FULLTEXT) == REGEX_PRINT_FULLTEXT)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_FULLTEXT;
        MAX_PRINT_TEXT_LENGTH = 0;
    }
    if ((PRINT_LAYOUT & REGEX_PRINT_NOTEXT) == REGEX_PRINT_NOTEXT)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_NOTEXT;
    }
    if ((PRINT_LAYOUT & REGEX_PRINT_NOSTATS) == REGEX_PRINT_NOSTATS)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_NOSTATS;
    }
    if ((PRINT_LAYOUT & REGEX_PRINT_NORESULTS) == REGEX_PRINT_NORESULTS)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_NORESULTS;
    }
    if ((PRINT_LAYOUT & REGEX_PRINT_NOINDEX) == REGEX_PRINT_NOINDEX)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_NOINDEX;
    }
    if ((PRINT_LAYOUT & REGEX_PRINT_FILTER) == REGEX_PRINT_FILTER)
    {
        PRINT_LAYOUT = PRINT_LAYOUT - REGEX_PRINT_FILTER;
    }

    if (1)
    {
        temp_string = _PRINT__CONCAT_OPTION_FLAGS(regex_data);

        if ((PRINT_OPTIONS & REGEX_PRINT_NOTEXT) != REGEX_PRINT_NOTEXT &&
            (regex_data->return_code == REGEX_COMP_SUCCESS ||
             regex_data->return_code == REGEX_MATCH_SUCCESS))
        {
            output_string = _PRINT__GET_TEXT_STRING(regex_data);
        }
        else
        {
            output_string = __MALLOC(512 * sizeof(char)); 
            strcpy(output_string, "");
        }

        if ((PRINT_OPTIONS & REGEX_PRINT_NOSTATS) != REGEX_PRINT_NOSTATS)
        {
            temp_string = _PRINT__GET_REGEX_STATS(regex_data, temp_string);
            strcat(output_string, temp_string);
        }
        
        strcpy(temp_string, "");
        __FREE(temp_string);
    }
    else
    {
        output_string = __MALLOC(32 * sizeof(char)); 
        strcpy(output_string, "");
    }
    
    if ((PRINT_OPTIONS & REGEX_PRINT_NORESULTS) != REGEX_PRINT_NORESULTS &&
        (regex_data->return_code == REGEX_COMP_SUCCESS ||
         regex_data->return_code == REGEX_MATCH_SUCCESS))
    {
        temp_string = _PRINT__GET_RESULTS(
            regex_data,
            PRINT_LAYOUT,
            !((PRINT_OPTIONS & REGEX_PRINT_NOTEXT) == REGEX_PRINT_NOTEXT &&
              (PRINT_OPTIONS & REGEX_PRINT_NOSTATS) == REGEX_PRINT_NOSTATS),    /* print header or not */
            !((PRINT_OPTIONS & REGEX_PRINT_NOINDEX) == REGEX_PRINT_NOINDEX)         /* print match position or not */
        );

        output_string =__REALLOC(output_string, (strlen(output_string) + strlen(temp_string) + 32) * sizeof(char));
        strcat(output_string, temp_string);
        strcpy(temp_string, "");
        __FREE(temp_string);
    }

    return output_string;
}

/* set_default_reg_flags() - Sets the default REG_ flags for regex_compile() and regcomp()
   ---------------------------------------------------------------------------------------
   regex.h-Library-Documentation:
   REG_EXTENDED: Support extended regular expressions.
   REG_ICASE:    Ignore case in match.
   REG_NEWLINE:  Eliminate any special significance to the newline character.
   REG_NOSUB:    Report only success or fail in regexec(), that is, verify the syntax of a regular expression.
                 If this flag is set, the regcomp() function sets re_nsub to the number of parenthesized sub-expressions found in pattern.
                 Otherwise, a sub-expression results in an error.
                 (-> This option is not deactivated)

   extended flags:
   REG_GLOBAL    Uses global-search with multiple matches instead of single matching
   REG_MULTILINE Catches the newline character, automaticly deactivates REG_NEWLINE
   REG_NOSUBEXP  Ignore matching of grouped submatches by SUBEXPpressions */
void set_default_reg_flags(int OPTION_FLAGS)
{
    int i;

    for (i = 0; i < 7; i++)
    {
        DEFAULT_REG_FLAGS[i] = 0;
    }

    for (i = 0; i < 7; i++)
    {
        if ((OPTION_FLAGS & REG_GLOBAL) == REG_GLOBAL)
        {
            DEFAULT_REG_FLAGS[i] = REG_GLOBAL;
            i++;
        }
        if ((OPTION_FLAGS & REG_EXTENDED) == REG_EXTENDED)
        {
            DEFAULT_REG_FLAGS[i] = REG_EXTENDED;
            i++;
        }
        if ((OPTION_FLAGS & REG_ICASE) == REG_ICASE)
        {
            DEFAULT_REG_FLAGS[i] = REG_ICASE;
            i++;
        }
        if ((OPTION_FLAGS & REG_NEWLINE) == REG_NEWLINE)
        {
            DEFAULT_REG_FLAGS[i] = REG_NEWLINE;
            i++;
        }
        if ((OPTION_FLAGS & REG_NOSUB) == REG_NOSUB)
        {
            DEFAULT_REG_FLAGS[i] = REG_NOSUB;
            i++;
        }
        if ((OPTION_FLAGS & REG_MULTILINE) == REG_MULTILINE)
        {
            DEFAULT_REG_FLAGS[i] = REG_MULTILINE;
            i++;
        }
        if ((OPTION_FLAGS & REG_NOSUBEXP) == REG_NOSUBEXP)
        {
            DEFAULT_REG_FLAGS[i] = REG_NOSUBEXP;
            i++;
        }
    }
}

/* regex_compile(char*, int) - Compiles the regular expression pattern.
   --------------------------------------------------------------------
   Description:
     Compiles a regular expression pattern with given option flags.

   Parameters:
     char* regex_pattern_string: The regular expression pattern
     int option_flags:     The regular expression option flags:
                           REG_EXTENDED: Support extended regular expressions.
                           REG_ICASE:    Ignore case in match.
                           REG_NEWLINE:  Eliminate any special significance to the newline character.

   Return Value:
     returns:              The unexecuted RegEx Object without the regular expression results. */
RegEx regex_compile(char* regex_pattern_string, int OPTION_FLAGS)
{
    RegEx regex_data = _COMPILE__INIT_REGEX(regex_pattern_string, OPTION_FLAGS);

    __ASSERT_PARAM(regex_pattern_string, "regex_pattern_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(&OPTION_FLAGS, "OPTION_FLAGS", ASSERT_TYPE_INT);

    if (MAX_PATTERN_LENGTH > 16384)
    {
        sprintf(regex_data->error_message, "MAX_PATTERN_LENGTH exceeds maximum size of 16384\n");
        regex_data->return_code = REGEX_ERROR;
    }

    if (regex_data != NULL && regex_data->return_code == REGEX_INIT_SUCCESS)
    {
        /* compile regular expression */
        if (strlen(regex_pattern_string) < MAX_PATTERN_LENGTH)
        {
            /* convert regular expression character classes */
            char* converted_regex_pattern_string = _COMPILE__CONVERT_SEQUENCES(regex_pattern_string);

            /*  int regcomp(regex_t * preg, const char*  pattern, int cflags);
                --------------------------------------------------------------
                Compiles the regular expression specified by pattern into an executable string of op-codes.

                preg:       is a pointer to a compiled regular expression.
                pattern:    is a pointer to a character string defining a source regular expression (described below).
                cflags:     is a bit flag defining configurable attributes of compilation process:
                            REG_EXTENDED: Support extended regular expressions.
                            REG_ICASE:    Ignore case in match.
                            REG_NEWLINE:  Eliminate any special significance to the newline character.
                            REG_NOSUB:    Report only success or fail in regexec(), that is, verify the syntax of a regular expression.
                                          If this flag is set, the regcomp() function sets re_nsub to the number of parenthesized sub-expressions found in pattern.
                                          Otherwise, a sub-expression results in an error.

                return:     If successful, regcomp() returns 0.
                            If unsuccessful, regcomp() returns nonzero, and the content of preg is undefined. */
            regex_data->return_code = regcomp(&regex_data->regex_h.compiled_regex, converted_regex_pattern_string, regex_data->regex_h.reglib_flags);
            regex_data->num_pattern_subexpr = (int)regex_data->regex_h.compiled_regex.re_nsub;
            __FREE(converted_regex_pattern_string);
        }
        else
        {
            regex_data->return_code = REGEX_ERROR;
            sprintf(regex_data->error_message, "Pattern string exceeds maximum length of %d\n", MAX_PATTERN_LENGTH);
        }

        if (strlen(regex_pattern_string) == 0)
        {
            regex_data->return_code = REGEX_ERROR;
            sprintf(regex_data->error_message, "Regular expression pattern is empty\n");
        }

        /* set regex.h status */
        if (regex_data->return_code == REGEX_COMP_SUCCESS)
        {
            strcpy(regex_data->error_message, "No error");
            regex_data->regex_h.reglib_status = REGLIB_COMPILED;
        }
        else if (regex_data->return_code == REGEX_ERROR)
        {
            regex_data->regex_h.reglib_status = REGLIB_ERROR;
        }
    }
    else
    {
        fprintf(stderr, "regex_compile() error: Regex object is not initialized.\n");
    }

    return regex_data;
}

/* regex_error(RegEx) - Writes and prints the error message buffer.
   ----------------------------------------------------------------
   Description:
     Writes the error code message of regerror() into the error message buffer
     and prints the error message to stderr.

   Parameters:
     RegEx regex_data: The RegEx object

   Return Value:
     returns:     The return code of regerror() */
int regex_error(RegEx regex_data)
{
    __ASSERT_PARAM(regex_data, "RegEx regex data", ASSERT_TYPE_STRUCT);

    if (regex_data != NULL)
    {
        /*  size_t regerror(int errcode, const regex_t *preg, char* errbuf, size_t errbuf_size);
            ------------------------------------------------------------------------------------
            Finds the description for errcode. 

            preg:        is a pointer to a compiled regular expression.
            errbuf:      is the message string buffer for the error description.
            errbuf_size: is the size of the message string buffer

            return:      regerror() returns the integer value that is the size of the buffer needed to
                         hold the generated description string for the error condition corresponding to errcode.

            regerror() returns the following messages:

                REG_BADBR:    Invalid \{ \} range exp
                REG_BADPAT:   Invalid regular expression
                REG_BADRPT:   ?*+ not preceded by valid RE
                REG_EBOL:     ¬ anchor and not BOL
                REG_EBRACE:   \{ \} or { } imbalance
                REG_EBRACK:   [] imbalance
                REG_ECHAR:    Invalid multibyte character
                REG_ECOLLATE: Invalid collating element
                REG_ECTYPE:   Invalid character class
                REG_EEOL:     $ anchor and not EOL
                REG_EESCAPE:  Last character is \
                REG_EPAREN:   \( \) or () imbalance
                REG_ERANGE:   Invalid range exp endpoint
                REG_ESPACE:   Out of memory
                REG_ESUBREG:  Invalid number in \digit
                REG_NOMATCH:  RE pattern not found */
        regerror(regex_data->return_code, &regex_data->regex_h.compiled_regex, regex_data->error_message, (size_t)sizeof(regex_data->error_message));
        fprintf(stderr, "Error: %s\n\n", regex_data->error_message);
        regex_data->regex_h.reglib_status = REGLIB_ERROR;
    }
    else
    {
        fprintf(stderr, "regex_error(): Regex object is not initialized.\n");
    }

    return regex_data->return_code;
}

/* regex_exec(char*, RegEx) - Executes a compiled regular expression pattern.
   --------------------------------------------------------------------------
   Description:
     Executes a compiled regular expression pattern and compares it with a given text input string.

   Parameters:
     char* input_text_string: The text input string for the regular expression
     RegEx regex_data:  The compiled RegEx object

   Return Value:
     returns:      The return code of regexec() */
int regex_exec(char* input_text_string, RegEx regex_data)
{
    int return_code = -1;

    __ASSERT_PARAM(input_text_string, "input text string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(regex_data, "RegEx regex data", ASSERT_TYPE_STRUCT);

    if (regex_data != NULL)
    {
        if (MAX_NUM_MATCHES > 3200000)
        {
            sprintf(regex_data->error_message, "MAX_NUM_MATCHES exceeds maximum size of 8192\n");
            regex_data->return_code = REGEX_ERROR;
        }

        if (regex_data->regex_h.reglib_status == REGLIB_COMPILED && regex_data->return_code == REGEX_COMP_SUCCESS)
        {
            regex_data->text = __REALLOC(regex_data->text, (strlen(input_text_string) + 1) * sizeof(char));
            strcpy(regex_data->text, input_text_string);

            /* REG_GLOBAL set: search the whole text string for multiple matches of the regular expression */
            if (regex_data->flags.GLOBAL == 0)
            {
                return_code = _EXEC__SEARCH_LOCAL(input_text_string, regex_data);
            }
            else
            {
                return_code = _EXEC__SEARCH_GLOBAL(input_text_string, regex_data);
            }
        }
        else
        {
            fprintf(stderr, "regex_exec() error: Regular expression is not compiled.\n");
        }
    }
    else
    {
        fprintf(stderr, "regex_exec() error: RegEx object is not initialized.\n");
    }
    
    return return_code;
}

/* regex_close(RegEx) - Frees the allocated memory.
   ------------------------------------------------
   Description:
     Frees the memory of allocated regex.h buffers and sets the regfree status to 1

   Parameters:
     RegEx regex_data: The RegEx object */
void regex_close(RegEx regex_data)
{
    __ASSERT_PARAM(regex_data, "RegEx regex data", ASSERT_TYPE_STRUCT);

    if (regex_data != NULL &&
        regex_data->regex_h.reglib_status != REGLIB_ERROR)
    {
        if (regex_data->regex_h.reglib_status == REGLIB_COMPILED ||
            regex_data->regex_h.reglib_status == REGLIB_EXECUTED)
        {
            int i;
            regex_data->regex_h.reglib_status = REGLIB_CLOSED;

            if (regex_data->return_code != REGEX_ERROR)
            {
                for (i = 0; i < regex_data->num_matches; i++)
                {
                    __FREE(regex_data->matches[i].string);
                }
                __FREE(regex_data->matches);
                __FREE(regex_data->text);
                /*  void reg__FREE(regex_t *preg);
                    ----------------------------
                    Frees any memory that was allocated by regcomp() to implement preg.
                    The expression defined by preg is no longer a compiled regular or extended expression. 
                    
                    preg: is a pointer to a compiled regular expression. */
                __FREE(regex_data->regex_h.match_offsets);
                regfree(&regex_data->regex_h.compiled_regex);
            }
            __FREE(regex_data);            
        }
    }
    else
    {
        fprintf(stderr, "regex_close() error: Regex object is not initialized.\n");
    }
}

/* regex_match(char*, char*, int) - Matches regular expression pattern in a string.
   --------------------------------------------------------------------------------
   Description:
     Compiles a regular expression pattern with given option flags, executes the
     regular expression and returns the RegEx Object with the regular expression results.

   Parameters:
     char* input_text_string:    The string with the substrings to replace.
     char* regex_pattern_string: The regular expression pattern
     int OPTION_FLAGS:           The regular expression option flags:
                                 REG_EXTENDED: Support extended regular expressions.
                                 REG_ICASE:    Ignore case in match.
                                 REG_NEWLINE:  Eliminate any special significance to the newline character.

   Return Value:
     returns:              The RegEx Object with the regular expression results.

     RegEx-Object:         matches[] matches:       The same array as match_offsets, but with sub-expression
                                                    Strings of the matches and more readable offset variables
                           int num_matches:         Number of matches
                           int num_pattern_subexpr: Number of corresponding sub-expressions
                           int return_code:         Return code of the expression string compilation
                           char[] text;             The input text string 
                           char[] pattern:          The regular expression string pattern
                           char[] error_message:    Error message buffer
                           regex_h_ref regex_h:     Reference to regex.h-variables */
RegEx regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS)
{
    RegEx regex_data;

    __ASSERT_PARAM(input_text_string, "input_text_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(regex_pattern_string, "regex_pattern_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(&OPTION_FLAGS, "OPTION_FLAGS", ASSERT_TYPE_INT);

    regex_data = regex_compile(regex_pattern_string, OPTION_FLAGS);

    if (regex_data != NULL && regex_data->return_code != REGEX_ERROR)
    {
        if (regex_data->return_code == REGEX_COMP_SUCCESS)
        {
            regex_exec(input_text_string, regex_data);
        }
        else
        {
            regex_error(regex_data);
        }
    }

    return regex_data;
}

/* regex_replace(char*, char*, char*,int) - Replaces regular expression matches with a substring.
   ----------------------------------------------------------------------------------------------
   Description:
     Replaces words in a input text string found by the regular expression pattern with the
     replacement substring.

   Parameters:
     char* input_text_string:    The string with the substrings to replace.
     char* regex_pattern_string: The regular expression pattern
     char* replace_substring:    The replcment substring
     int option_flags:           The regular expression option flags:
                                 REG_EXTENDED: Support extended regular expressions.
                                 REG_ICASE:    Ignore case in match.
                                 REG_NEWLINE:  Eliminate any special significance to the newline character.

   Return Value:
     returns:              The output string with the replaced substring values */
char* regex_replace(char* input_text_string, char* regex_pattern_string, char* replace_substring, int OPTION_FLAGS)
{
    int i = 0;
    int start_position_difference = 0;
    char* converted_input_text_string = __MALLOC((strlen(input_text_string) + 1) * sizeof(char)); 
    char* converted_replace_substring;

    RegEx regex_data = regex_compile(regex_pattern_string, OPTION_FLAGS);

    __ASSERT_PARAM(input_text_string, "input_text_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(replace_substring, "regex_pattern_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(replace_substring, "replace_substring", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(&OPTION_FLAGS, "OPTION_FLAGS", ASSERT_TYPE_INT);

    if (regex_data->return_code == REGEX_COMP_SUCCESS)
    {
        regex_exec(input_text_string, regex_data);
        strcpy(converted_input_text_string, input_text_string);

        /* replace the match with the replace substring */
        for (i = 0; i < regex_data->num_matches; i++)
        {
            /* if match "$" = end of string: replace end of string with substring */
            if (regex_data->matches[i].start == strlen(converted_input_text_string))
            {
                converted_input_text_string = __REALLOC(converted_input_text_string, (strlen(converted_input_text_string) + strlen(replace_substring) + 1) * sizeof(char));
                strcat(converted_input_text_string, replace_substring);

                break;
            }

            /* if match "^" = end of string: replace start of string with substring */
            if (regex_data->matches[i].start == 0 &&
                regex_data->matches[i].end == 0)
            {
                char* temp_string = __MALLOC((strlen(converted_input_text_string) + 1) * sizeof(char));
                converted_input_text_string = __REALLOC(converted_input_text_string, (strlen(converted_input_text_string) + strlen(replace_substring) + 1) * sizeof(char));

                strcpy(temp_string, converted_input_text_string);
                converted_input_text_string[0] = '\0';
                strcat(converted_input_text_string, replace_substring);
                strcat(converted_input_text_string, temp_string);

                __FREE(temp_string);

                break;
            }

            if (regex_data->matches[i].start != regex_data->matches[i].end)
            { 
                if (regex_data->matches[i].number_submatch == 0)
                {
                    converted_replace_substring = _REPLACE__PREPROCESS_GROUPS(regex_data, replace_substring, i);
                    converted_input_text_string = __SUBSTITUTE_STRING(converted_input_text_string, regex_data->matches[i].string, converted_replace_substring, REPLACE_NO_BRACKETS, REPLACE_LOCAL, regex_data->matches[i].start - start_position_difference);
                    start_position_difference += strlen(regex_data->matches[i].string) - strlen(converted_replace_substring);
                    __FREE(converted_replace_substring);
                }
            }
        }
    }
    else
    {
        regex_error(regex_data);
    }

    regex_close(regex_data);
    
    return converted_input_text_string;
}

/* void regex_readfile(char*, char*, int): Reads a file and matches its contents.
   ------------------------------------------------------------------------------
   Description:
     Reads the contentsfrom a file and matches its contents in the file-buffer.

   Parameters:
     file_name:                  The file-name of the file to read.
     regex_data 

   Return Value:
     returns:                    The contents of a file as a pointer ot a char array */
RegExFile regex_readfile(char* file_name)
{
    int error_code = 0;
    RegExFile regex_file = __MALLOC(sizeof(cregfile_t));

    __ASSERT_PARAM(file_name, "file_name", ASSERT_TYPE_PTR);

    if (strlen(file_name) > MAX_FILENAME_LENGTH)
    {
        fprintf(stderr, "Error: Length of Filename-Path exceeds maximum length of %d characters.\n", MAX_FILENAME_LENGTH);
        error_code = REGEX_ERROR;;
    }

    if (MAX_FILENAME_LENGTH > 1048576)
    {
        fprintf(stderr, "Error: MAX_FILENAME_LENGTH exceeds maximum size of 1048576 characters.\n");
        error_code = REGEX_ERROR;
    }

    regex_file->ptr = NULL;
    regex_file->name = "";
    regex_file->length = 0;
    regex_file->status = 0;

    if (error_code != REGEX_ERROR)
    {
        /* open file for reading */
        regex_file->name = file_name;
        regex_file->ptr = fopen(regex_file->name, "rb");

        if (regex_file->ptr != NULL)
        {
            /* get filesize */
            fseek(regex_file->ptr, 0, SEEK_END);
            regex_file->length = ftell(regex_file->ptr);
            rewind(regex_file->ptr);

            if (MAX_TEXT_LENGTH > 104857600)
            {
                fprintf(stderr, "Error: MAX_TEXT_LENGTH exceeds maximum size of 104857600 characters or 100 MB.\n");
                regex_file->content = (char*)__MALLOC(1 * sizeof(char));
                regex_file->status = 0;
            }
            else
            {   
                if (regex_file->length <= MAX_TEXT_LENGTH)
                {
                     /* read in file into buffer */
                    regex_file->content = (char*)__MALLOC((regex_file->length + 1) * sizeof(char));
                    regex_file->status = fread(regex_file->content,regex_file->length, 1, regex_file->ptr);
                }
                else
                {
                    fprintf(stderr, "Error: File length exceeds maximum length of %d characters.\n", MAX_TEXT_LENGTH);
                    regex_file->content = (char*)__MALLOC(1 * sizeof(char));
                    regex_file->status = 0;
                }
            }

            if (regex_file->content != NULL &&
                regex_file->status > 0)
            {
                regex_file->content[regex_file->length] = '\0';
            }
            else
            {
                strcpy(regex_file->content, "");
            }

            fclose(regex_file->ptr);
        }
        else
        {
            fprintf(stderr, "Error: File not found.\n");
        }
    }
    else
    {
        fprintf(stderr, "regex_readfile() error: RegExFile object is not initialized.\n");
    }
    
    return regex_file;
}

/* regex_closefile(RegExFile) - Frees the allocated memory for a input file.
   -------------------------------------------------------------------------
   Description:
     Frees the memory of allocated buffers fór a file

   Parameters:
     RegExFile regex_file: The RegExFile object */
void regex_closefile(RegExFile regex_file)
{
    if (regex_file != NULL)
    {
        __FREE(regex_file->content);
        __FREE(regex_file);
    }
}

/* int regex_writefile(RegEx, int, char*): Writes the contents of a RegEx Object into a file.
   ------------------------------------------------------------------------------------------
   Description:
     Writes the input text and the regular expression results and contents of a
     RegEx Object as a table or table into a file.

   Parameters:
     regex_data    The regular expression RegEx Object of compact-regex.h
     PRINT_LAYOUT: The layout for the printed result data:
     file_name:    The name of the file

   Return Value:
     Return 1 if the write was successful, or 0 if not */
int regex_writefile(RegEx regex_data, int PRINT_LAYOUT, char* file_name)
{
    char* output_string;
    /* char *full_filepath; */

    __ASSERT_PARAM(regex_data, "RegEx regex_data", ASSERT_TYPE_STRUCT);
    __ASSERT_PARAM(&PRINT_LAYOUT, "PRINT_LAYOUT", ASSERT_TYPE_INT);
    __ASSERT_PARAM(file_name, "file_name", ASSERT_TYPE_PTR);
    
    if (MAX_FILENAME_LENGTH > 1048576)
    {
        fprintf(stderr, "Error: MAX_FILENAME_LENGTH exceeds maximum size of 1048576 characters.\n");
        regex_data->return_code = REGEX_ERROR;
    }

    if (regex_data->return_code != REGEX_ERROR)
    {
        if (regex_data != NULL &&
            PRINT_LAYOUT != REGEX_PRINT_NONE &&
            regex_data->regex_h.reglib_status != REGLIB_ERROR)
        {
            FILE* file_ptr = fopen(file_name, "w");

            if (regex_data->regex_h.reglib_status == REGLIB_COMPILED ||
                regex_data->regex_h.reglib_status == REGLIB_EXECUTED)
            {
                if (!((PRINT_LAYOUT & REGEX_PRINT_FILTER) == REGEX_PRINT_FILTER))
                {
                    printf("\n");
                }

                if (PRINT_COLORED == 1)
                {
                    PRINT_COLORED = 2;
                }

                /* full_filepath = realpath(file_name, NULL); // not ANSI or Windows compatible */
                if ((PRINT_LAYOUT & REGEX_PRINT_JSON) == REGEX_PRINT_JSON)
                {
                    output_string = _PRINT__GET_OUTPUT_STRING(regex_data, PRINT_LAYOUT | REGEX_PRINT_NOTEXT | REGEX_PRINT_NOSTATS);
                }
                else
                {
                    output_string = _PRINT__GET_OUTPUT_STRING(regex_data, PRINT_LAYOUT);
                }

                if (PRINT_COLORED == 2)
                {
                    PRINT_COLORED = 1;
                }

                if (PRINT_COLORED == 1)
                {
                    printf("\033[34mOutput file:\033[0m      %s\n", file_name);
                    printf("\033[34mFilesize:\033[0m         %.2f KB\n", (double)(strlen(output_string) / 1024.0));
                }
                else
                {
                    printf("Output file:      %s\n", file_name);
                    printf("Filesize:         %.2f KB\n", (double)(strlen(output_string) / 1024.0));
                }
                
                if (fprintf(file_ptr, "%s", output_string) < 0)
                {
                    printf("\nfile write error.");
                    __FREE(output_string);
                    fclose(file_ptr);
                    return 0;
                }
                
                __FREE(output_string);
            }
            else
            {
                fprintf(stderr, "regex_writefile() error: Compiled regular expression already closed.\n");
                return 0;
            }  

            fclose(file_ptr);
        }
        else
        {
            fprintf(stderr, "regex_writefile() error: RegEx object is not initialized.\n");
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "regex_writefile() error: File i/o can not be initialized.\n");
        return 0;
    }

    return 1;
}

/* int regex_writefile_string(char*, char*): Writes a string into a file.
   ------------------------------------------------------------------------------------------
   Description:
     Writes a string into a file.

   Parameters:
     output_string: The string of the file content
     file_name:     The name of the file

   Return Value:
     Return 1 if the write was successful, or 0 if not */
int regex_writefile_string(char* output_string, char* file_name)
{
    FILE* file_ptr;

    __ASSERT_PARAM(output_string, "output_string", ASSERT_TYPE_PTR);
    __ASSERT_PARAM(file_name, "file_name", ASSERT_TYPE_PTR);
    
    if (MAX_FILENAME_LENGTH > 1048576)
    {
        fprintf(stderr, "Error: MAX_FILENAME_LENGTH exceeds maximum size of 1048576 characters.\n");
    }

    file_ptr = fopen(file_name, "w");

    printf("\nOutput file:      %s\n", file_name);
    printf("Filesize:         %.2f KB\n", (double)(strlen(output_string) / 1024.0));
        
    if (fprintf(file_ptr, "%s", output_string) < 0)
    {
        printf("\nfile write error.");
        fclose(file_ptr);
        return 0;
    }
        
    fclose(file_ptr);

    return 1;
}

/* void regex_print(RegEx): Prints the contents of a RegEx Object.
   ---------------------------------------------------------------
   Description:
     Prints the input text and the regular expression results and contents of a
     RegEx Object as a table or as a list.

   Parameters:
     regex_data:   The regular expression RegEx Object of compact-regex.h
     PRINT_LAYOUT: The layout for the printed result data:
                   REGEX_PRINT_NONE       - no result output
                   REGEX_PRINT_TABLE      - table
                   REGEX_PRINT_LIST       - short list
                   REGEX_PRINT_LIST_FULL  - long full list
                   REGEX_PRINT_PLAIN      - only the result contents
                   REGEX_PRINT_CSV        - list with comma seperated values
                   REGEX_PRINT_JSON       - JavaScript Object Notation

   Note:
     The variable RX_PRINT_TABLE determines the print style table or list. */
void regex_print(RegEx regex_data, int PRINT_LAYOUT)
{
    char* output_string;
    
    __ASSERT_PARAM(regex_data, "RegEx regex_data", ASSERT_TYPE_STRUCT);
    __ASSERT_PARAM(&PRINT_LAYOUT, "PRINT_LAYOUT", ASSERT_TYPE_INT);

    if (regex_data != NULL &&
        PRINT_LAYOUT != REGEX_PRINT_NONE &&
        regex_data->regex_h.reglib_status != REGLIB_ERROR)
    {
        if (regex_data->regex_h.reglib_status == REGLIB_COMPILED ||
            regex_data->regex_h.reglib_status == REGLIB_EXECUTED)
        {
            output_string = _PRINT__GET_OUTPUT_STRING(regex_data, PRINT_LAYOUT);
            printf("%s", output_string);
            __FREE(output_string);
        }
        else
        {
            fprintf(stderr, "regex_print() error: Compiled regular expression already closed.\n");
        }  
    }
    else
    {
        fprintf(stderr, "regex_print() error: Regex object is not initialized.\n");
    }
}
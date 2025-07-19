#include "include/compact-regex.h"

/* example for most simple regex */
void simple_usage()
{
    /* simple matching with regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS) */
    RegEx regex_data = regex_match("abc 123 ABC xyz 456789 abc", "abc ((xy)z) (\\d{2})", REG_ICASE);
    printf("\nExample of simple usage:\n------------------------\n");
    regex_print(regex_data, REGEX_PRINT_TABLE);
    regex_close(regex_data);
}

/* example for most simple regex (only statistics) */
void simple_usage_only_statistics()
{
    /* simple matching with regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS) */
    RegEx regex_data = regex_match("abc 123 ABC xyz 456789 abc", "abc ((xy)z) (\\d{2})", REG_ICASE);
    printf("\nExample of simple usage:\n------------------------\n");
    regex_print(regex_data, REGEX_PRINT_TABLE | REGEX_PRINT_NOTEXT | REGEX_PRINT_NORESULTS);
    regex_close(regex_data);
}


/* example for most simple regex (colored ANSI output) */
void simple_usage_with_colors()
{
    /* simple matching with regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS) */
    PRINT_COLORED = 1;
    simple_usage();
}

/* example for single step regex-parser */
void extended_usage()
{
    /* extended step by step matching with:
          - regex_compile(char* regex_pattern_string, int OPTION_FLAGS)
          - return_code status flags
          - regex_exec(char* input_text_string, RegEx regex)
          - regex_error(RegEx regex)
          - using the data fields of the RegEx-Object
       */
    int i;
    int option_flags = REG_GLOBAL | REG_EXTENDED | REG_ICASE | REG_MULTILINE;
    char* input_string ="01\nabc\nABC\nxyz\n123\ntest\n45678";
    char* regex_pattern = "(\\d+)[^0-9]+(\\d+)";

    RegEx regex_data = regex_compile(regex_pattern, option_flags);

    if (regex_data->return_code == REGEX_COMP_SUCCESS)
    {
        regex_exec(input_string, regex_data);

        if (regex_data->return_code == REGEX_MATCH_SUCCESS)
        {
            /* show internals of regex struct as variables */
            printf("\n\nExample of extended usage:\n--------------------------\n");
            printf("Text:\n\"%s\"\n\n", regex_data->text);
            printf("Regular-Expression:       %s\n", regex_data->pattern);
            printf("Number of matches:        %d\n", regex_data->num_matches);
            printf("Number of subexpressions: %d\n", regex_data->num_pattern_subexpr);
            printf("\nResults:\n");

            for (i = 0; i < regex_data->num_matches; i++)
            {
                printf("%d:\n", i);
                printf("Start-Position:     %d\n", regex_data->matches[i].start);
                printf("End-Position:       %d\n", regex_data->matches[i].end);
                printf("Substring:          \"%s\"\n", regex_data->matches[i].string);
                printf("\n");
            }
        }
    }
    else
    {
        regex_error(regex_data);
    }

    regex_close(regex_data);
}

/* example for basic regular expression string replacement */
void replace_strings()
{
    char* input_string = "Mr Black is changing his 6 strings on his Brown guitar";
    char* output_string = regex_replace(input_string, "black|Brown", "Blue", REG_GLOBAL | REG_ICASE);

    printf("\nExample of string replacement:\n------------------------------\n");
    printf("Original Text: %s\nReplaced text: %s\n", input_string, output_string);
    free(output_string);
}

/* example for replacing multiple regular expression strings */
void replace_multiple_strings()
{
    int option_flags = REG_GLOBAL | REG_ICASE;
    char* input_string = "Mr Black is changing his 6 strings on his Brown guitar";
    char* output_string_1;
    char* output_string_2;
    char* output_string_3;
    char* output_string_4;
    char* output_string_5;

    printf("\n\nExample of multiple string replacements:\n----------------------------------------\n");
    printf("Original text: %s\n\n", input_string);

    output_string_1 = regex_replace(input_string, "black|Brown", "Blue", option_flags);
    printf("1.Replacement: %s\n", output_string_1);

    output_string_2 = regex_replace(output_string_1, "guitar", "acoustic guitar", option_flags);
    printf("2.Replacement: %s\n", output_string_2);

    output_string_3 = regex_replace(output_string_2, "his \\d", "3 old", option_flags);
    printf("3.Replacement: %s\n", output_string_3);

    output_string_4 = regex_replace(output_string_3, "$", ".", option_flags);
    printf("4.Replacement: %s\n", output_string_4);

    output_string_5 = regex_replace(output_string_4, "^", "The musician ", option_flags);
    printf("5.Replacement: %s\n", output_string_5);

    free(output_string_1);
    free(output_string_2);
    free(output_string_3);
    free(output_string_4);
    free(output_string_5);
}

int main(int argc, char* argv[])
{
    int selection_number = argc > 1 ? atoi(argv[1]) : -1;

    if (argc < 2)
    {
        printf(" [1] simple_usage()\n\
 [2] simple_usage_only_statistics()\n\
 [3] simple_usage_with_colors()\n\
 [4] extended_usage()\n\
 [5] replace_strings()\n\
 [6] replace_multiple_strings()\n\n\
 [0] exit\n\
\n\
Select an example function by the number: ");

        if (scanf("%d", &selection_number) == 1)
        {
            switch (selection_number)
            {
                case 1:
                {
                    simple_usage();
                    break;
                }
                case 2:
                {
                    simple_usage_only_statistics();
                    break;
                }
                case 3:
                {
                    simple_usage_with_colors();
                    break;
                }
                
                case 4:
                {
                    extended_usage();
                    break;
                }
                case 5:
                {
                    replace_strings();
                    break;
                }
                case 6:
                {
                    replace_multiple_strings();
                    break;
                }
                case 0:
                {
                    exit(EXIT_SUCCESS);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
    else if ((argc == 2) | (argc > 4))
    {
        printf("compact-regex.h: regex.h library POSIX/GNU extension wrapper example executable\nArguments: ... <input-text> <regular-expression> [(optional:) <replace-string>]\n");
    }
    else if (argc == 3)
    {
        RegEx regex_data = regex_match(argv[1], argv[2], REG_GLOBAL);
        regex_print(regex_data, REGEX_PRINT_TABLE);
        regex_close(regex_data);
    }
    else if (argc == 4)
    {
        char* output_string = regex_replace(argv[1], argv[2], argv[3], REG_GLOBAL | REG_ICASE);
        printf("%s\n", output_string);
        free(output_string);
    }
    
    return 0;
}
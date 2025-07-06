#include "compact-regex.h"

void read_port_numbers_csv_file()
{
    RegExFile regex_file;

    /* file length is 13926 bytes */
    MAX_TEXT_LENGTH = 13926;
    MAX_PRINT_TEXT_LENGTH = 512;

    regex_file = regex_readfile("example-text-files/service-names-port-numbers.csv");

    if (regex_file->status > 0)
    {
        RegEx regex_data = regex_compile("(\\d+);(.*UDP.*);(.*mail.*);", REG_GLOBAL | REG_ICASE);
        regex_exec(regex_file->content, regex_data);

        if (regex_data->return_code == REGEX_MATCH_SUCCESS)
        {
            regex_data->file = regex_file;
            regex_print(regex_data, REGEX_PRINT_LIST | REGEX_PRINT_FULLTEXT);
            regex_writefile(regex_data, REGEX_PRINT_TABLE | REGEX_PRINT_NOTEXT | REGEX_PRINT_NOSTATS, "output_portnumbers_table.txt");
        }

        regex_closefile(regex_file);
        regex_close(regex_data);
    }
}

void read_windows_formatted_file()
{
    RegExFile regex_file = regex_readfile("example-text-files/windows-formatted-regfile.reg");

    if (regex_file->status > 0)
    {
        RegEx regex_data = regex_compile(".*HKEY_CLASSES_ROOT.*\r\n", REG_GLOBAL | REG_ICASE);
        regex_exec(regex_file->content, regex_data);

        if (regex_data->return_code == REGEX_MATCH_SUCCESS)
        {
            regex_data->file = regex_file;
            regex_print(regex_data, REGEX_PRINT_TABLE);
            regex_writefile(regex_data, REGEX_PRINT_JSON, "output_reg_file.json");
        }

        regex_closefile(regex_file);
        regex_close(regex_data);
    }
}

void read_large_word_list_file()
{
    RegExFile regex_file;

    /* file length is 4862984 bytes */
    MAX_TEXT_LENGTH = 4862984;
    /* more than 15000 matches */
    MAX_NUM_MATCHES = 16000;

    regex_file = regex_readfile("example-text-files/words.txt");

    printf("\nReading large wordlist file (example-text-files/words.txt: 466550 lines, 4.7 MB)...please wait.\n\n");
   
    if (regex_file->status > 0)
    {
        RegEx regex_data = regex_compile("^.*ion.*$", REG_GLOBAL | REG_ICASE);
        regex_exec(regex_file->content, regex_data);

        if (regex_data->return_code == REGEX_MATCH_SUCCESS)
        {
            regex_data->file = regex_file;
            regex_print(regex_data, REGEX_PRINT_NOTEXT | REGEX_PRINT_NORESULTS);
            regex_writefile(regex_data, REGEX_PRINT_PLAIN, "output_words.txt");
            printf("\nResults are exported to output_words.txt.\n");
        }

        regex_closefile(regex_file);
        regex_close(regex_data);
    }
}

/* example for reading file contents and match a regular expression */
int main(int argc, char* argv[])
{
    int selection_number = argc > 1 ? atoi(argv[1]) : -1;
    char* output_string;

    if (argc == 1)
    {
        printf(" [1] read_port_numbers_csv_file()\n\
 [2] read_windows_formatted_file()\n\
 [3] read_large_word_list_file()\n\
\n\
Select an example function by the number: ");

        if (scanf("%d", &selection_number) == 1)
        {
            switch (selection_number)
            {
                case 1:
                {
                    read_port_numbers_csv_file();
                    break;
                }
                case 2:
                {
                    read_windows_formatted_file();
                    break;
                }
                case 3:
                {
                    read_large_word_list_file();
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
    else if ((argc == 2) || (argc > 4))
    {
        printf("Arguments: ... <file-name> <regular-expression> [(optional:) <replace-string>]\n");
    }
    else if (argc == 3 || argc == 4)
    {
        RegExFile regex_file = regex_readfile(argv[1]);

        if (regex_file->status > 0)
        {
            RegEx regex_data = regex_compile(argv[2], REG_GLOBAL | REG_ICASE);
            regex_exec(regex_file->content, regex_data);

            if (regex_data->return_code == REGEX_MATCH_SUCCESS)
            {
                if (argc == 3)
                {
                    regex_data->file = regex_file;
                    regex_print(regex_data, REGEX_PRINT_LIST);
                }
                else if (argc == 4)
                {
                    output_string = regex_replace(regex_file->content, regex_data->pattern, argv[3], REG_ICASE);
                    printf("%s\n", output_string);
                    free(output_string);
                }
            }

            regex_closefile(regex_file);
            regex_close(regex_data);
        }
    }
    
    return 0;
}
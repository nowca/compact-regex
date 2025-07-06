#include "compact-regex.h"


void match_and_print_example(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS, int PRINT_LAYOUT)
{
    RegEx regex_data = regex_match(input_text_string, regex_pattern_string, OPTION_FLAGS);
    regex_print(regex_data, PRINT_LAYOUT);
    regex_close(regex_data);
}

void only_numbers_validation()
{
    char* input_string =
"12324445\n\
CNI324234\n\
039624\n\
443 \n\
-6583478";
    char* regex_pattern = "^[0-9]+$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void word_length_between_range()
{
    char* input_string =
"Car\n\
Television\n\
Computer\n\
Phone\n\
Mountainbike\n\
Guitar";
    char* regex_pattern = "^\\w{5,10}$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void number_value_between_range()
{
    char* input_string =
"8567\n\
092384\n\
199\n\
299\n\
399\n\
1391\n\
90";
    char* regex_pattern = "^([1-2]{0,1}[0-9][0-9])$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void limit_decimal_places()
{
    char* input_string =
"123\n\
123.4\n\
123.45\n\
123.456\n\
123.4567\n\
123.45678\n\
123.456789";
    char* regex_pattern = "^\\d+(\\.\\d{0,3})?$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void phone_number()
{
    char* input_string =
"+49 171 2345678\n\
+1 212 456 7890\n\
+91 987 654 3210\n\
+7 9123456789\n\
0 20101234567";
    char* regex_pattern = "^\\+?[\\d|\\s]+$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void binary_number()
{
    char* input_string =
"1010001011111111\n\
0xA2FF\n\
01101100\n\
102010";
    char* regex_pattern = "^[01]+$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void hexadecimal_number()
{
    char* input_string =
"3B7C9A\n\
F1E4\n\
2000G9\n\
Z101010\n\
0xABF9";
    char* regex_pattern = "^(0x?[0-9a-fA-F]+)$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void repeating_numbers()
{
    char* input_string = "123 444 567 890";
    char* regex_pattern = "(\\d)\\d*\\1\\d*\\1";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void validate_email_address()
{
    char* input_string = 
"abc@def.com\n\
john.smith@example.com\n\
moel@gmail.co.in\n\
sarah_johnson@my:example.net\n\
mailagmx.com";
    char* regex_pattern = "([a-zA-Z0-9\\_\\.\\+\\-]+)@(([a-zA-Z0-9\\-]+)\\.([a-zA-Z0-9\\-\\.]+))";
    match_and_print_example(input_string, regex_pattern, REG_GLOBAL | REG_EXTENDED | REG_NEWLINE | REG_NOGROUPS, REGEX_PRINT_TABLE);  
}

void validate_domain_name()
{
    char* input_string =
"example.com\n\
www.example.co.uk\n\
example\n\
.net\n\
www.exa mple.com\n\
www.example.my";
    char* regex_pattern = "^([-A-Za-z0-9]+\\.)+[A-Za-z]{2,6}$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void validate_ip_addresses()
{
    char* input_string =
"255.255.255.255\n\
192.156.292.123\n\
1233.156.255.123\n\
123.156.255.123:8080\n\
123.156.2550.123\n\
123.156.255.255.1230\n\
123.156.255.123";
    char* regex_pattern = "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(:\\d{1,5})?$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void validate_time()
{
    char* input_string =
"23:59\n\
08:34\n\
5:02\n\
24:01\n\
15:60\n\
9:55";
    char* regex_pattern = "^([01][0-9]|2[0-3]):[0-5][0-9]$";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void validate_timestamp()
{
    char* input_string =
"12-05-1991T03:02:00+00:00\n\
38-05-1991T03:02:00+00:00\n\
29-12-0806T00:00:00+00:00\n\
30-32-1991T12:00:00+00:00\n\
29-12-0806T24:00:00+00:00\n\
29-12-0806T10:60:00+00:00\n\
01-02-1970T14:11:32+00:00\n\
31-12-0000T00:59:59+00:00\n\
32-02-1970T14:11:32+00:00\n\
22-13-1970T14:11:32+00:00\n\
22-11-1970T24:11:32+00:00\n\
22-11-1970T23:60:32+00:00\n\
22-11-1970T23:30:62+00:00";
    char* regex_pattern = "([0-2][0-9]|[3][0|1])[-]([0][1-9]|[1][1|2])[-]([0-9]{4})[T]([0|1][0-9]|[2][0-3])([:][0-5][0-9]){2}(\\+[0|1][0-9]|[2][0-3])([:][0-5][0-9])";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

void get_youtube_url()
{
    char* input_string =
"https://www.youtube.com/watch?v=XNADRNnzP8I\n\
https://www.youtube.com/watch?v=z4plxUWOHxw&t=1s\n\
https://youtu.be/UW3pBCArsTg\n\
https://www.youtube.com/watch?v=4HpjbvLeM3Y&t=124s\n\
https://youtu.be/7Jub0c-qQGM\n\
https://www.youtube.com/watch?v=vksABX_yYN0&t=1451s\n\
https://youtu.be/3dHtbo5wwx4\n\
https://www.youtube.com/watch?v=7QPhMbs2Bd8&t=41s\n\
https://www.youtube.com/watch?v=YD2_pXJTcAU\n\
https://youtu.be/9bDSzO8wtCA\n\
https://www.youtube.com/watch?v=4xfNhseN25E&t=272s\n\
https://youtu.be/pKUggMsbQt0?123=asd";
    char* regex_pattern = "^.*(youtu.?be\\/|v\\/|u\\/\\w+\\/|embed|watch\\?v|\\?v=|\\&v=)([^#\\&\\?]{11,11}).*";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
}

/* https://www.skybert.net/emacs/how-to-get-unicode-in-the-terminal/ */
void special_characters()
{
    /* http://www.unicode.org/charts/ */
    char* input_string = "? € µ ¶ ½\n\
ÜòÄ $£¥ ©\n\
\"\"\" \'\'\' Æ Ø";
    char* regex_pattern = "\x21|\u20AC|\u00b5|\x22+|\x27{2}";
    match_and_print_example(input_string, regex_pattern, REG_DEFAULT, REGEX_PRINT_TABLE);
    printf("\n* The regular expression pattern is written with ascii- and unicode numbers \"\\x21|\\u20AC|\\u00b5|\\x22+|\\x27{2}\"\n");
}

int main(int argc, char* argv[])
{
    int selection_number = argc > 1 ? atoi(argv[1]) : -1;

    /* sets the default reg- flags for regex_compile() */
    set_default_reg_flags(REG_GLOBAL | REG_EXTENDED | REG_NEWLINE);

    if (selection_number < 0 || selection_number > 14)
    {
        printf(" [1] only_numbers_validation()\n\
 [2] word_length_between_range()\n\
 [3] number_value_between_range()\n\
 [4] phone_number()\n\
 [5] binary_number()\n\
 [6] hexadecimal_number()\n\
 [7] repeating_numbers()\n\
 [8] validate_email_address()\n\
 [9] validate_domain_name()\n\
[10] validate_ip_addresses()\n\
[11] validate_time_format()\n\
[12] validate_timestamp()\n\
[13] get_youtube_url()\n\
[14] special_characters()\n\
\n\
Select an example function by the number: ");
    }
    
    if ((selection_number >= 0 && selection_number <= 14) || scanf("%d", &selection_number) == 1)
    {
        printf("\n");
        switch (selection_number)
        {
            case 1:
            {
                only_numbers_validation();
                break;
            }
            case 2:
            {
                word_length_between_range();
                break;
            }
            case 3:
            {
                number_value_between_range();
                break;
            }
            case 4:
            {
                phone_number();
                break;
            }
            case 5:
            {
                binary_number();
                break;
            }
            case 6:
            {
                hexadecimal_number();
                break;
            }
            case 7:
            {
                repeating_numbers();
                break;
            }
            case 8:
            {
                validate_email_address();
                break;
            }
            case 9:
            {
                validate_domain_name();
                break;
            }
            case 10:
            {
                validate_ip_addresses();
                break;
            }
            case 11:
            {
                validate_time();
                break;
            }
            case 12:
            {
                validate_timestamp();
                break;
            }
            case 13:
            {
                get_youtube_url();
                break;
            }
/* only if compiler-version is above c99 */            
#if __STDC_VERSION__ >= 199901L
#ifdef __linux__
            case 14:
            {
                special_characters();
                break;
            }
#endif
#endif
            default:
            {
                break;
            }
        }
    }

    return 0;
}
# compact-regex.h

This is a *C89/ANSI-C* compatible extension for the *regex.h POSIX/GNU Regular Expression library* from 1992.

It is designed as a very simple and modern extension wrapper library for fast implementation of Regex in C or C-compatible projects
like prototyping or basic tasks with regular expressions.

The program is using the POSIX-compatible [*Basic Regular Expressions (BRE)*](#basic-regular-syntax) and [*Extended Regular Expressions (ERE)*](#extended-regular-syntax) standard by the Open Group with extended functions:

* simple and intuitive usage in C sourcecode
* easy implementation
* global search, multiple options
* better matching of subexpressions
* use of single-letter character classes like `\d` or `\s`
* handling of ASCII- and Unicode escape sequences
* string replacement with group substitution
* formatted terminal output functions
* file read processing and export functions

<br>

*Note: The library is not thread-safe and not Perl Regex compatible, it's currently just POSIX Regex with extensions.*

<br>

## Table of contents

- [Project files](#project-files)
    - [Library files](#library-files)
    - [Example and usage files](#example-and-usage-files)
- [Compiling](#compiling)
    - [Linux](#linux)
    - [Windows](#windows)
- [Integration into your project](#integration-into-your-project)
- [How to use](#how-to-use)
    - [Simple use](#simple-use)
    - [Extended use](#direct-use)
    - [Example program files](#example-program-files)
- [Program functions](#program-functions)
    - [regex_match()](#regex_match)
    - [regex_compile()](#regex_compile)
    - [regex_exec()](#regex_exec)
    - [regex_close()](#regex_close)
    - [regex_error()](#regex_error)
    - [regex_replace()](#regex_replace)
    - [regex_print()](#regex_print)
    - [set_default_reg_flags()](#set_default_reg_flags)
- [Program functions for filesystem](#program-functions)
    - [regex_readfile()](#regex_readfile)
    - [regex_closefile()](#regex_closefile)
    - [regex_writefile()](#regex_writefile)
    - [regex_writefile_string()](#regex_writefile_string)
- [Program options](#program-functions)
    - [Option flags](#option-flags)
    - [Print layout](#print-layout)
    - [Colored terminal output](#colored-terminal-output)
- [RegEx Object](#regex-object)
- [POSIX Standard](#posix-standard)
    - [Basic Regular Syntax](#basic-regular-syntax)
    - [Extended Regular Syntax](#extended-regular-syntax)
- [Character classes](#character-classes)
- [ASCII and Unicode sequences](#ascii-and-unicode-sequences)
    - [ASCII codes](#ascii-codes)
    - [Unicode](#unicode)
- [Matching subexpressions](#matching-subexpressions)
- [Replace strings with regular expressions](#replace-strings-with-regular-expressions)
- [File reading and writing](#file-reading-and-writing)
    - [RegExFile Object](#regexfile-object)
    - [Reading from a file](#reading-from-a-file)
    - [Maximum value limiters](#maximum-value-limiters)
    - [Writing to a file](#writing-to-a-file)

<br>

## Project files

### Library files

> `include/compact-regex.h`
-  Main header file which can be included in your project

> `inculde/compact-regex.c`
- Related source code file

### Example and usage files

> `compact-regex_usage.c`
- Basic usage examples of *compact-regex.h*

> `examples/compact-regex_examples.c`
- Many basic regular expression examples

> `examples/compact-regex_file-reading.c`
- File reading and writing with *compact-regex.h*

<h3>Additional files</h3>

> `documentation/Regex - edition 0.12a -  1992.pdf`
- Official documentation of *regex.h* from 1992

> `examples/example-text-files/*`
- Some example text files for parsing

<br>

## Compiling

### Linux

Build the example program by typing in:

```console
user@pc:~$ make
```

...or compile it directly with the GNU-C-Compiler:

```console
user@pc:~$ gcc -Wall -static compact-regex_usage.c -o compact-regex_usage
```

- The GNU Extensions with the regex.h library are needed for successful compilation. Please take care of including the neccesary header and library files.

<br>

### Windows

To compile the program on windows, you will need a compiler version with the *regex.h library*, from GNU extensions included:

```console
C:\Users\pcuser>gcc.exe -static -IC:\MinGW-W64\mingw32\opt\include compact-regex_usage.c -o compact-regex_usage.exe -LC:\MinGW-W64\mingw32\opt\lib -lregex
```

- MinGW-W64 includes the regex.h library in the `\opt\include` and `\opt\lib` folders.
- The paths of the header and library must be included with `-I` and `-L`, with an additional `-lregex` parameter at the end of the command.
- `-static` can be used to make your project independend from libraries.
- The path of gcc.exe must be added to the Windows PATH user-variable

<br>

## Integration into your project

Just include the `compact-regex.h` file into your project, that's all. *(You don't need to tell the gcc compiler the paths of the library files via the command-line)*

```c
#include "compact-regex.h"
```

<br>

## How to use

### Simple use

The most fast and simple usage:

```c
RegEx regex_data = regex_match("abc 123 ABC xyz", "abc", REG_ICASE)
regex_print(regex_data, REGEX_PRINT_TABLE);
regex_close(regex_data);
```

- This means: *match the the regular expression `"abc"` in the input text `"abc 123 ABC xyz"` with the regex.h [option flags](#option-flags) for searching with insensitive case option and print the results as a table.*

<br>

### Extended use

A more detailed step by step code usage:

```c
int option_flags = REG_GLOBAL | REG_EXTENDED | REG_ICASE;
char* input_string = "abc";
char* regex_pattern = "abc 123 ABC xyz";

RegEx regex_data = regex_compile(regex_pattern, option_flags);

if (regex_data->return_code == REGEX_COMP_SUCCESS)
{
    regex_exec(input_string, regex_data);

    if (regex_data->return_code == REGEX_MATCH_SUCCESS)
    {
        /* show internals of regex struct as variables */
        printf("Text:\n%s\n\n", regex_data->text);
        printf("Regular-Expression:       %s\n", regex_data->pattern);
        printf("Number of matches:        %d\n", regex_data->num_matches);
        
        printf("\nResults:\n");

        for (i = 0; i < regex_data->num_matches; i++)
        {
            printf("Result Nubmer %d:\n", i);
            printf("Start-Position:     %d\n", regex_data->matches[i].start);
            printf("End-Position:       %d\n", regex_data->matches[i].end);
            printf("Substring:          %s\n", regex_data->matches[i].string);
        }
    }
}
else
{
    regex_error(regex_data);
}

regex_close(regex_data);
```

* *compile the regular expression pattern `"abc"` with the `regex.h` [option flags](#option-flags)*
* *execute the compiled regular expression on the input text `"abc 123 ABC xyz"`, if compilation is successful*
* *print the data on the screen, if execution is successful*

<br>

### Example program files

The example files can be used by just running them or working with program arguments:

**compact-regex_usage.c**


```console
user@pc:~$ ./compact-regex_usage
```

- This runs the basic example implementation with examples for:

    - basic usage and extended usage
    - string replacements

<br>

```console
user@pc:~$ ./compact-regex_usage "abc 123 XYZ" "\d+"
```

- This searches for all digit words `"\d+"` in the string `"abc 123 XYZ"`

<br>

```console
user@pc:~$ ./compact-regex_usage "abc 123 XYZ" "\d+" "###"
```

- This searches for all digit words `"\d+"` in the string `"abc 123 XYZ"` and replace it with the substring `"###"`, so the result will output `"abc ### XYZ"`


<br>

**examples/compact-regex_examples.c**

```console
user@pc:~$ ./compact-regex_examples
```

- This contains many basic regular expression examples:

    - word length between range
    - number value between range
    - phone number
    - binary number
    - hexadecimal number
    - repeating numbers
    - validate email address
    - validate domain name
    - validate ip addresses
    - validate time format
    - validate timestamp
    - get youtube url
    - special characters

- The example function can be selected by a number or typed in as a number as first argument.

<br>

**examples/compact-regex_file-reading.c**

```console
user@pc:~$ ./compact-regex_file-reading
```

- This runs the a file reading and writing example implementation with examples for:

    - read of a csv file, with write of table txt-file
    - read of a windows formatted reg file, with write of a json-file
    - read of a large word list file, with write of plain txt-file

<br>

```console
user@pc:~$ ./compact-regex_file-reading Makefile "SRC"
```

- This matches the substring `"SRC"` in the file `"Makefile"` and prints the output as a list.

<br>


```console
user@pc:~$ ./compact-regex_file-reading Makefile "SRC" "###"
```

- This matches the substring `"SRC"` in the file `"Makefile"` and replace all matches with the substring `"###"`.

<br>

## Program functions

### regex_match()

```c
RegEx regex_match(char* input_text_string, char* regex_pattern_string, int OPTION_FLAGS)
```
Compiles a *regular expression pattern* with given [*option flags*](#option-flags) and executes the *regular expression*.

**Return value**: returns the [`RegEx`](#regex-object) Object with the *regular expression results*.

<br>

### regex_compile()

```c
RegEx regex_compile(char* regex_pattern_string, int OPTION_FLAGS)
```
Compiles a *regular expression pattern* with given [*option flags*](#option-flags) and returns the [`RegEx`](#regex-object) Object for execution processing.

**Return value**: returns the [`RegEx`](#regex-object) Object with the *compiled regular expression pattern*.

<br>

### regex_exec()

```c
int regex_exec(char* input_text_string, RegEx regex)
```
Executes a *compiled regular expression pattern* and compares it with a given *input text string*.

**Return value**: returns the return code of `regexec()`.

<br>

### regex_close()

```c
void regex_close(RegEx regex)
```
Frees the memory of allocated library and *regex.h* buffers.

<br>

### regex_error()

```c
int regex_error(RegEx regex)
```
Writes the error code message of `regerror()` into the *error message buffer* and prints the error message to `stderr`.

**Return value**:  returns the return code of `regerror()`.

<br>

### regex_replace()

```c
char* regex_replace(char* input_text_string, char* regex_pattern_string, char* replace_substring, int OPTION_FLAGS)
```
Replaces *regular expression matches* with a *replacement substring* and the given [*option flags*](#option-flags).

The *replacement substring* can be used with group references (`"\1"`, `"\2"`, `"\3"`...)

**Return value**:  The output string with the replaced substring values.

<br>

### regex_print()

```c
void regex_print(RegEx regex, int PRINT_LAYOUT)
```
Prints the *input text* and the *regular expression results* and contents of a [`RegEx`](#regex-object) Object as a table or as a list with a given [print layout](#print-layout).

The terminal screen output can be set to colored ANSI output with the global variable `PRINT_COLORED` 

<br>

### set_default_reg_flags()

```c
void set_default_reg_flags(int OPTION_FLAGS)
```
Sets the default `REG_` [option flags](#option-flags) for `regex_compile()` and `regcomp()`

<br>

### regex_readfile()

```c
RegExFile regex_readfile(char* file_name)
```
Reads a file, its attributes and contents by the given *filename*.

**Return value**:  The [`RegExFile`](#regexfile-object) object

<br>

### regex_closefile()

```c
void regex_closefile(RegExFile regex_file)
```
Closes a [`RegExFile`](#regexfile-object) object, frees attributes and contents and its allocated memory

<br>

### regex_writefile()

```c
int regex_writefile(RegEx regex_data, int PRINT_LAYOUT, char* file_name)
```
Writes the input text and the regular expression results and contents of a [`RegEx`](#regex-object) Object as a table or table into a file with a given [print layout](#print-layout).

**Return value**:  Return 1 if the write was successful, or 0 if not

<br>

### regex_writefile_string()

```c
int regex_writefile_string(char* output_string, char* file_name)
```
Writes a string into a file.

**Return value**:  Return 1 if the write was successful, or 0 if not

<br>

## Program options

### Option flags

The *option flags* passed to [`regex_compile()`](#regex_compile) and [`regex_match()`](#regex_match) are processed and passed through to the internal *regex.h* function `regcomp()`.

<br>

According to the *regex.h* documentation they are:

* `REG_EXTENDED` - Support *extended regular expressions*.

* `REG_ICASE` - Ignore case in match.

* `REG_NEWLINE` - Eliminate any special significance to the newline character.

Additionaly `REG_NEWLINE` is documented in *Regex - edition 0.12a - 1992* as follows:

> - match-any-character operator (see Section 3.2 [Match-any-character Operator],
page 9) doesn’t match a newline.
> - nonmatching list not containing a newline (see Section 3.6 [List Operators],
page 13) matches a newline.`REG_NOSUB`
> - match-beginning-of-line operator (see Section 3.9.1 [Match-beginning-of-line Op-
erator], page 18) matches the empty string immediately after a newline, regardless
of how REG_NOTBOL is set (see Section 7.2.3 [POSIX Matching], page 37, for an
explanation of REG_NOTBOL).
> - match-end-of-line operator (see Section 3.9.1 [Match-beginning-of-line Operator],
page 18) matches the empty string immediately before a newline, regardless of how
REG_NOTEOL is set (see Section 7.2.3 [POSIX Matching], page 37, for an explanation
of REG_NOTEOL).

* `REG_NOSUB` - Report only success or fail in `regexec()`, that is, verify the syntax of a *regular expression*. If this flag is set, the `regcomp()` function sets `re_nsub` to the number of parenthesized *sub-expressions* found in pattern. Otherwise, a sub-expression results in an error.

**(!) The `REG_NOSUB` option flag is deactivated in the program.**

<br>


The *compact-regex.h* library adds the following additional flags as optional functions:

* `REG_GLOBAL` - Uses global-search with multiple matches instead of single matching

* `REG_MULTILINE` - Catches the newline character, automaticly deactivates `REG_NEWLINE`

* `REG_NOSUBEXP` - Ignore matching of grouped submatches by subexpressions

* `REG_SUBEXP` - Match only subexpressions 

<br>

You can use them directly as function arguments like this:

```c
regex_compile("[a-z]*", REG_GLOBAL | REG_EXTENDED | REG_ICASE | REG_MULTILINE);
```

<br>

...or you can pass them as a integer variable with the options:

```c
int option_flags = REG_GLOBAL | REG_EXTENDED | REG_ICASE | REG_MULTILINE;

regex_compile("[a-z]*", option_flags);
```


<br>

#### Default Option flags

The [option flags](#option-flags) `REG_GLOBAL`, `REG_EXTENDED`, `REG_NEWLINE` are set by default so you don't need to pass them.

If you don't want to search for all occurrences of a substring, use basic regular expression syntax or search with special newline treatment, you can set the *default option flags* by yourself with:

```c
set_default_reg_flags(int OPTION_FLAGS)
```

<br>

**Example:**
```c
set_default_reg_flags(REG_GLOBAL | REG_EXTENDED | REG_ICASE);
```

- This will set the [*option flags*](#option-flags) `REG_GLOBAL`, `REG_EXTENDED`, `REG_ICASE` to the *default option flags* for working with the library functions.

<br>


```c
regex_compile("[a-z]*", REG_DEFAULT);
```

- This will use the *default option flags*, even if you don't pass them to the regular expression compiler.

<br>

```c
regex_compile("[a-z]*", REG_ICASE);
```

- This would mean: use the default option flags `REG_GLOBAL`, `REG_EXTENDED`, `REG_NEWLINE` with the additional option flag `REG_ICASE`.

<br>

### Print layout

The layout flags for *printing* and *file export* are:

* `REGEX_PRINT_TABLE` - print as table

* `REGEX_PRINT_LIST` - print as list

* `REGEX_PRINT_LIST_FULL` - print as full list

* `REGEX_PRINT_PLAIN` - just print the results only

* `REGEX_PRINT_CSV` - print the results as CSV-Format (Character-Separated Values)

* `REGEX_PRINT_JSON` - print the results in JSON-Format (JavaScript Object-Notation)

#### Additional printing layout filter

The *print layout flag* can be extended with a additional *layout filter flag*:

* `REGEX_PRINT_FULLTEXT` - print the full input text

* `REGEX_PRINT_NOTEXT` - don't print the input text

* `REGEX_PRINT_NOSTATS` - don't print the regular expression statistic data

* `REGEX_PRINT_NORESULTS` - don't print the results of the regular expression execution

* `REGEX_PRINT_NOINDEX` - don't print the index positions of the results

<br>

You can use the layout printing flag and its filters like this:

```c
regex_print(regex_data, REGEX_PRINT_TABLE | REGEX_PRINT_NOTEXT | REGEX_PRINT_NOSTATS);
```

- This prints just the results as a table ***without** the input text string* and ***without** the regular expression statistics*.

<br>

### Colored terminal output

The terminal screen output can be set to colored ANSI text:
```c
/* Prints the terminal output in colored ANSI text */
unsigned int PRINT_COLORED = 0;
```

<br>

## RegEx Object

The [`RegEx`](#regex-object) object contains all the related data of the *regular expression* process:

```c
cregflags_t flags;          /* status of option flags */
cregmatches_t* matches;     /* array with the match start and end string positions and the substring */
cregfile_t file;            /* file object */
int num_matches;            /* number of matches */
int num_pattern_subexpr;    /* number of corresponding sub-expressions */
int return_code;            /* return code of the expression string compilation */
char* text;                 /* the regular expression input text string */
char* pattern;              /* the regular expression string pattern */
char error_message[128];    /* error message buffer */
regex_h_ref regex_h;        /* reference to internal regex.h-variables */
```

- The fields `flags`, `num_pattern_subexpr` and `pattern` are related to [`regex_compile()`](#regex_compile). The [`regex_compile()`](#regex_compile) function returns a [`RegEx`](#regex-object) object with the compiled data in the field `regex_h` with a `return_code` and an `error_essage` on failure.

- The returned [`RegEx`](#regex-object) object will be executed with [`regex_exec()`](#regex_exec) and contains the related field `matches`, `num_matches`, `text` and a internal values in the field `regex_h`.

- The RegEx object can be printed with [`regex_print()`](#regex_print) on screen with or closed with [`regex_close()`](#regex_close) after comilation and execution.

- [`regex_compile()`](#regex_compile) and [`regex_exec()`](#regex_exec) can both be used combined as [`regex_match()`](#regex_match).

- The [`RegEx`](#regex-object) object must be closed with [regex_close()](#regex_close) to free the allocated memory.

<br>

**RegEx subobjects**:

**Field: `flags`**

The subobject `regexobj->flags` contains the *regex [option flags](#option-flags) status*.

```c
int GLOBAL;
int EXTENDED;
int ICASE;
int MULTILINE;
int NEWLINE;
int NOSUB;           /* note: REG_NOSUB is deactivated in the program */
int NOSUBEXP;
int SUBEXP
```

- The can be set with the [Option flags](#option-flags) on [`regex_compile()`](#regex_compile) or [`regex_match()`](#regex_match).

<br>

**Field: `matches`**

The subobject `regexobj->matches[i]` contains the *regex match result data*:
```c
int number_match;    /* number of the match */
int number_submatch; /* number of the group or submatch */
int start;           /* byte offset from string's start to substring's start. */
int end;             /* byte offset from string's start to substring's end. */
char* string;        /* string of the sub-expression match */
```

<br>

**Field: `file`**

The subobject `regexobj->file` can be connected to a [`RegExFile`](#regexfile-object) object. This can be useful for printing or other functions.

<br>

## POSIX Standard 

### Basic Regular Syntax

| Metacharacter | Description                                         | 
| :------------:|:----------------------------------------------------|
| ^             | Matches the starting position within the string. In line-based tools, it matches the starting position of any line. |
| .             | Matches any single character (many applications exclude newlines, and exactly which characters are considered newlines is flavor-, character-encoding-, and platform-specific, but it is safe to assume that the line feed character is included). Within POSIX bracket expressions, the dot character matches a literal dot. For example, a.c matches "abc", etc., but [a.c] matches only "a", ".", or "c". |
| [ ]           | A bracket expression. Matches a single character that is contained within the brackets. For example, [abc] matches "a", "b", or "c". [a-z] specifies a range which matches any lowercase letter from "a" to "z". These forms can be mixed: [abcx-z] matches "a", "b", "c", "x", "y", or "z", as does [a-cx-z]. The - character is treated as a literal character if it is the last or the first (after the ^, if present) character within the brackets: [abc-], [-abc], [^-abc]. Backslash escapes are not allowed. The ] character can be included in a bracket expression if it is the first (after the ^, if present) character: []abc], [^]abc]. |
| [^ ]         | Matches a single character that is not contained within the brackets. For example, [^abc] matches any character other than "a", "b", or "c". [^a-z] matches any single character that is not a lowercase letter from "a" to "z". Likewise, literal characters and ranges can be mixed. |
| $            | Matches the ending position of the string or the position just before a string-ending newline. In line-based tools, it matches the ending position of any line.
| ( )          | Defines a marked subexpression, also called a capturing group, which is essential for extracting the desired part of the text (See also the next entry, \n). BRE mode requires \( \). |
| \n           | Matches what the nth marked subexpression matched, where n is a digit from 1 to 9. This construct is defined in the POSIX standard.[36] Some tools allow referencing more than nine capturing groups. Also known as a back-reference, this feature is supported in BRE mode. |
| *            | Matches the preceding element zero or more times. For example, ab*c matches "ac", "abc", "abbbc", etc. [xyz]* matches "", "x", "y", "z", "zx", "zyx", "xyzzy", and so on. (ab)* matches "", "ab", "abab", "ababab", and so on. |
| \{m,n\}        | Matches the preceding element at least m and not more than n times. For example, a{3,5} matches only "aaa", "aaaa", and "aaaaa". This is not found in a few older instances of regexes. BRE mode requires \{m,n\}. |

### Extended Regular Syntax

| Metacharacter | Description                                         | 
|:-------------:|:----------------------------------------------------|
| ?             | Matches the preceding element zero or one time. For example, ab?c matches only "ac" or "abc". |
| +             | Matches the preceding element one or more times. For example, ab+c matches "abc", "abbc", "abbbc", and so on, but not "ac". |
| \|            | The choice (also known as alternation or set union) operator matches either the expression before or the expression after the operator. For example, abc|def matches "abc" or "def". |

Source: https://en.wikipedia.org/wiki/Regular_expression#POSIX_basic_and_extended

<br>

## Character classes

| Description                      | POSIX           | Shortcode    | ASCII              |
| :------------------------------- |:----------------|:-------------|:-------------------|
| ASCII characters                 |                 | \x[Bytecode] |                    |
| Alphanumeric characters          | [:alnum:]       |              | [A-Za-z0-9]        |
| Alphanumeric characters plus "_" |                 | \w           | [A-Za-z0-9_]       |
| Non-word characters              |                 | \W           | [^A-Za-z0-9_]      |
| Alphabetic characters            | [:alpha:]       | \a           | [A-Za-z]           |
| Space and tab                    | [:space:]       | \s           |                    |
|                                  | [:blank:]       | \t           |                    |
| Non-whitespace characters        |                 | \S           | [^ ]               |
| Word boundaries                  |                 | \b           |                    |
| Non-word boundaries              |                 | \B           |                    |
| Digits                           | [:digit:]       | \d           | [0-9]              |
| Non-digits                       |                 | \D           | [^0-9]             |
| Lowercase letters                | [:lower:]       | \l           | [a-z]              |
| Uppercase letters                | [:upper:]       | \u           | [A-Z]              |
| Visible characters               | [:print:]       | \p           | [\x20-\x7E]        |

<br>

## ASCII and Unicode sequences

### ASCII Codes

You can use a hexadecimal bytecode sequences to match a specific ASCII character:

```c
RegEx regex_data = regex_compile("\x61\x62\x63", REG_DEFAULT);
regex_exec("abc 123 ABC xyz", regex_data);
```

- This matches the ASCII characters of the `"abc"` of the *regular expression pattern* in the *input text string* `"abc 123 ABC xyz"`

<br>

### Unicode

You can do the same with *Unicode sequences*:

```c
RegEx regex_data = regex_compile("\u20AC|\u00b5", REG_DEFAULT);
regex_exec("! € µ ? x y z", regex_data);
```

- This matches the special characters `"€"` or `"µ"` in the *input text string* `"! € µ ? x y z"`


<br>

## Matching subexpressions

Groups (or submatches) in a regular expression can be matched in a text with the use of parentheses as *subexpressions* to create numbered capture groups.

**Example:**

- The input-string: `"abc 123 ABC xyz"`

- and the regular expression pattern: `"((\w+) (\d+)) (.*)"`

- matches:

    - the whole string `abc 123 ABC xyz`

    - 4 groups:

        - *1:* `abc 123` by the subexpression `((\w+) (\d+))`
        - *2:* `abc` by the subexpression `(\w+)`
        - *3:* `123`by the subexpression `(\d+)`
        - *4:* `ABC xyz` by the subexpression `(.*)`

<br>

It is possible to match only the *subexpressions* with the `REG_SUBEXP` option flag, see [option flags](#option-flags).

<br>

## Replace strings with regular expressions

The [regex_replace()](#regex_replace) function replaces all matches of a *regular expression* with a *substring*:

**Example:**

```c
char* output_string = regex_replace("Mr Black is changing his 6 strings on his Brown guitar", "black|brown", "Blue", REG_GLOBAL | REG_ICASE);
```

- This will match all upper case and lower case words `"black"` or `"brown"` in the *input text string* and replace it with the word "Blue". It returns a pointer to a character array string with the modified text:

    - `"Mr Blue is changing his 6 strings on his Blue guitar"`.

<br>

**Example with group references:**

- This will replace `"ABC"` with `"CBA"`:

```c
char* output_string = regex_replace("ABC", "(A)(B)(C)", "\\3\\2\\1", REG_DEFAULT);
```

<br>

**Example with multiple replacements:**

```c
int option_flags = REG_GLOBAL | REG_ICASE;
char* input_string = "Mr Black is changing his 6 strings on his Brown guitar";
char* output_string_1;
char* output_string_2;

output_string_1 = regex_replace(input_string, "black|Brown", "Blue", option_flags);
output_string_2 = regex_replace(output_string_1, "guitar", "acoustic guitar", option_flags);
```

- `output_string_2` will be:

    - `"Mr Blue is changing his 6 string on his Blue acoustic guitar"`.

<br>

## File reading and writing

### RegExFile Object

The Object `RegExFile` contains fields for the *regex file i/o data*:
```c
FILE* ptr;                  /* pointer to the FILE object for reading and writing*/
char* name;                 /* filename string */
char* buffer;               /* file buffer */
int status;                 /* status of file reading */
int length;                 /* file-length */
```

- The `RegExFile` object can be connected to the a pointer in the `RegEx` object like `regex_data->file`.

<br>

### Reading from a file

The contents of a file can be read into the buffer of the [RegExFile](#regexfile-object) object.

The [Maximum value limiters](#maximum-value-limiters) must be changed in order to work with larger files:

```c
/* file length is 30 KB = 30720 bytes */
MAX_TEXT_LENGTH = 30720;
/* there more than 4000 matches */
MAX_NUM_MATCHES = 4096;
/* print the first 1024 characters */
MAX_PRINT_TEXT_LENGTH = 1024;

RegExFile regex_file = regex_readfile("example-file.txt");

if (regex_file->status > 0)
{
    RegEx regex_data = regex_compile("[a-zA-Z]+ [0-9]+", REG_GLOBAL | REG_ICASE);
    regex_exec(regex_file->content, regex_data);

    if (regex_data->return_code == REGEX_MATCH_SUCCESS)
    {
        regex_data->file = regex_file;
        regex_print(regex_data, REGEX_PRINT_LIST);
        regex_writefile(regex_data, REGEX_PRINT_TABLE | REGEX_PRINT_NOTEXT | REGEX_PRINT_NOSTATS, "output_as_table.txt");
    }

    regex_closefile(regex_file);
    regex_close(regex_data);
}
```

- Read the file into the [RegExFile](#regexfile-object) object. Check the status of `fread()`, then compile the *regeular expression* and execute it on the file content. If the matching is successful, print the matched results as a list and export it to a file as table without the text and *regular expression* statistics.

- The [RegExFile](#regexfile-object) object **must not** be connected to the [RegEx](#regex-object), but it's useful for functions like `regex_print()` to print related i/o data.

- The [RegExFile](#regexfile-object) must also be closed with `regex_closefile()` to free the allocated memory.

- The [RegExFile](#regexfile-object) must be set to `NULL` before `regex_closefile()`, if `regex_readfile()` is not called, to avoid a runtime error.


> The program can handle large text files with more than 1.000.000 lines and over 100.000 matches. It is tested with larger files over 10 MB up to 100 MB. But very large text files with more than 10.000 matches can take a long time to process (more than 10 minutes).

<br>

### Maximum value limiters

The maximum value limiters can be changed to larger size values:
```c
/* Memory limiters */
unsigned int MAX_TEXT_LENGTH = 8192;
unsigned int MAX_PATTERN_LENGTH = 1024;
unsigned int MAX_NUM_MATCHES = 1024;
unsigned int MAX_PRINT_TEXT_LENGTH = 512;
unsigned int MAX_FILENAME_LENGTH = 256;
```
- These are the default values. They can easily be changed to other sizes, if needed.

<br>

### Writing to a file

<br>

#### regex_writefile()

The [regex_writefile()](#regex_writefile) function is quite similar to the [regex_print()](#regex_print) function. It can be used in the same way for printing the content into a formated text file using the [Print layout](#print-layout).

<br>

**Examples:**

```c
regex_writefile(regex_data, REGEX_PRINT_PLAIN, "output_words.txt");
```

- Just print the results into a text file.

<br>

```c
regex_writefile(regex_data, REGEX_PRINT_TABLE | REGEX_PRINT_NOTEXT | REGEX_PRINT_NOSTATS, "output_as_table.txt");
```

- Print a table without the text or any additional information.

<br>

```c
regex_writefile(regex_data, REGEX_PRINT_JSON, "output_reg_file.json");
```

- Print the results into a JSON file.

<br>

#### regex_writefile_string()

The [regex_writefile_string()](#regex_writefile_string) function can be used to print a basic string into a file.

<br>

**Example:**

```c
regex_writefile_string(output_string, "replaced_words.txt");
```

<br>

### Acknowledgements

* Regex (edition 0.12) - Kathryn A. Hargreaves, Karl Berry
You can read a compiled version of their documentation in the subfolder: `documentation/Regex - edition 0.12a - 1992.pdf`

* The makers of *Sublime Text* for a very nice text editor software.

* List Of English Words - https://github.com/dwyl/english-words

<br>
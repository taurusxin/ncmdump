# ifdef _WIN32
/*
 * Win32-specific argv initialization that splits OS-supplied UNICODE
 * command line string to array of UTF8-encoded strings.
 */
void win32_utf8argv(int *argc, char **argv[]);
# endif
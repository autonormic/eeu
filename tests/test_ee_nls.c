#include <nl_types.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
check_message(nl_catd catalog, int id, const char *fallback,
    const char *expected)
{
	const char *actual;

	actual = catgets(catalog, 1, id, (char *)fallback);
	if (strcmp(actual, expected) != 0) {
		fprintf(stderr, "message %d: expected [%s], got [%s]\n",
		    id, expected, actual);
		return (1);
	}
	return (0);
}

int
main(int argc, char **argv)
{
	nl_catd catalog;
	int errors;

	if (argc != 2) {
		fprintf(stderr, "usage: %s catalog-name\n", argv[0]);
		return (2);
	}
	if (setlocale(LC_ALL, "") == NULL) {
		fputs("cannot activate the requested locale\n", stderr);
		return (2);
	}
	catalog = catopen(argv[1], NL_CAT_LOCALE);
	if (catalog == (nl_catd)-1) {
		perror("catopen");
		return (2);
	}

	errors = 0;
	errors += check_message(catalog, 27, "main menu", "Главное меню");
	errors += check_message(catalog, 47,
	    "help    : get this info                 file    : print file name          ",
	    "help    : справка                          file    : имя файла");
	errors += check_message(catalog, 55,
	    "  eeu [+#] [-i] [-e] [-h] [file(s)]                                           ",
	    "  eeu [+#] [-i] [-e] [-h] [файл(ы)]");
	/* Command words and configuration keywords intentionally remain English. */
	{
		static const struct { int id; const char *word; } commands[] = {
			{ 116, "HELP" }, { 117, "WRITE" }, { 118, "READ" },
			{ 119, "LINE" }, { 120, "FILE" }, { 121, "CHARACTER" },
			{ 122, "REDRAW" }, { 123, "RESEQUENCE" }, { 124, "AUTHOR" },
			{ 125, "VERSION" }, { 126, "CASE" }, { 127, "NOCASE" },
			{ 128, "EXPAND" }, { 129, "NOEXPAND" }, { 130, "EXIT" },
			{ 131, "QUIT" }, { 132, "INFO" }, { 133, "NOINFO" },
			{ 134, "MARGINS" }, { 135, "NOMARGINS" },
			{ 136, "AUTOFORMAT" }, { 137, "NOAUTOFORMAT" },
			{ 138, "ECHO" }, { 139, "PRINTCOMMAND" },
			{ 140, "RIGHTMARGIN" }, { 141, "HIGHLIGHT" },
			{ 142, "NOHIGHLIGHT" }, { 143, "EIGHTBIT" },
			{ 144, "NOEIGHTBIT" }, { 159, "EMACS" },
			{ 160, "NOEMACS" }
		};
		size_t i;

		for (i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
			errors += check_message(catalog, commands[i].id,
			    commands[i].word, commands[i].word);
	}
	errors += check_message(catalog, 95, "y", "y");
	errors += check_message(catalog, 110,
	    "<!echo 'list of unrecognized words'; echo -=-=-=-=-=-",
	    "<!echo 'list of unrecognized words'; echo -=-=-=-=-=-");
	errors += check_message(catalog, 186,
	    "line %d col %d lines from top %d ",
	    "строка %d, столбец %d, строк от начала %d ");
	errors += check_message(catalog, 44,
	    "^[ (escape) menu        ESC-Enter: exit eeu                                ",
	    "^[ меню (Escape)     ESC-Enter: выход из eeu");
	errors += check_message(catalog, 61,
	    "^c command        ^k delete char    ^f undelete char      ESC-Enter: exit eeu ",
	    "^c команда    ^k удалить знак ^f вернуть знак    ESC-Enter: выход из eeu");
	errors += check_message(catalog, 163,
	    "eeu configuration saved in file %s",
	    "настройки eeu сохранены в файле %s");
	errors += check_message(catalog, 165, "save eeu configuration",
	    "сохранить настройки eeu");
	errors += check_message(catalog, 168, "eeu configuration not saved",
	    "настройки eeu не сохранены");
	errors += check_message(catalog, 169,
	    "file name must be specified when running reeu",
	    "при запуске reeu необходимо указать имя файла");
	errors += check_message(catalog, 187, "written by Hugh Mahon",
	    "автор: Hugh Mahon");
	errors += check_message(catalog, 188,
	    "eeu's standard input and output must be a terminal\n",
	    "eeu должен быть запущен в терминале (stdin и stdout)\n");

	catclose(catalog);
	if (errors != 0)
		return (1);
	puts("Russian message catalog tests passed");
	return (0);
}

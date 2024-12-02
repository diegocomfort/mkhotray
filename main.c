#define _POSIX_C_SOURCE 200809L	// strdup(3)
#define _GNU_SOURCE			// get_current_dir_name(3)

#include <stdio.h>
#include <sysexits.h>
#include <unistd.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>
#include <regex.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

struct arguments
{
	// All members are unique and allocated
	char *name;
	char *parent_dir;
	char *dir;
	char *ident;
	char *lib;
};

struct replacement
{
	const char *original;
	const char *new;
};

int parse_args(struct arguments *args, int argc, char **argv);
error_t parse_opt(int key, char *arg, struct argp_state *state);
int set_and_check_all_options(struct arguments *args);
bool is_valid_identifier(char *ident);
void free_args(struct arguments *args);
int create_raylib_hot_reload_template(struct arguments *args);
const char *get_template_share_dir(void);
int copy_template_dir(const char *template_dir, const char *new_dir);
void create_replacement_strings(char **ident_upper, char **lib,
				char **lib_upper, struct arguments *args);
void uppercaseify(char *str);
void open_and_replace(const char *file_name, const char *new_name,
		      struct replacement *replacements);
char *read_entire_file(const char *file_path, size_t *length);
char* replace_strings(const char *text, const char *old, const char *new);

int main(int argc, char **argv)
{
	struct arguments args = { 0 };
	int error = parse_args(&args, argc, argv);
	if (error)
	{
		return error;
	}

	error = create_raylib_hot_reload_template(&args);
	free_args(&args);
	return error;
}

int parse_args(struct arguments *args, int argc, char **argv)
{
	const char *doc = "Make a hot-reload template for Raylib.";
	const char *args_doc = "<project name>";
	static struct argp_option options[] = {
		{
			.name = "parent-dir",
			.key = 'p',
			.arg = "<project parent directory name>",
			.flags = 0,
			.doc = "The project's parent directory. Default: ./",
			.group = 0
		},
		{
			.name = "directory-name",
			.key = 'd',
			.arg = "<project directory name>",
			.flags = 0,
			.doc = "The project directory's name. "
			"Default: <project name>",
			.group = 0
		},
		{
			.name = "identifier-name",
			.key = 'i',
			.arg = "<identifier name>",
			.flags = 0,
			.doc = "The identifer to use for functions/variables. "
			"Default: <project name>",
			.group = 0
		},
		{
			.name = "lib-name",
			.key = 'l',
			.arg = "<shared library name>",
			.flags = 0,
			.doc = "The name for the shared library "
			"(lib<lib-name>.so). "
			"Default: <identifier name> (defaults to "
			"<project name> if no <identifier name> "
			"is provided)",
			.group = 0
		},
		{ NULL, '\0', NULL, 0, NULL, 0 },
	};
	struct argp argp;
	argp.options = options;
	argp.parser = parse_opt;
	argp.args_doc = args_doc;
	argp.doc = doc;
	argp.children = NULL;
	argp.help_filter = NULL;
	argp.argp_domain = NULL;

	error_t error = argp_parse(&argp, argc, argv, 0, NULL, args);
	if (error)
	{
		return error;
	}

	return set_and_check_all_options(args);
}

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *args = state->input;

	switch (key)
	{
	case 'p':
		free(args->parent_dir);
		args->parent_dir = strdup(arg);
		break;
	case 'd':
		free(args->dir);
		args->dir = strdup(arg);
		break;
	case 'i':
		free(args->ident);
		args->ident = strdup(arg);
		break;
	case 'l':
		free(args->lib);
		args->lib = strdup(arg);
		break;
	case ARGP_KEY_ARG:	// positional argument(s)
		if (state->arg_num >= 1)
		{
			argp_usage(state);
		}
		args->name = strdup(arg);
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1)
		{
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

int set_and_check_all_options(struct arguments *args)
{
	if (!is_valid_identifier(args->name) &&
	    !is_valid_identifier(args->ident))
	{
		fprintf(stderr,
			"ERORR: no valid identifiers given\n");
		return 1;
	}

	if (args->parent_dir == NULL)
	{
		args->parent_dir = strdup("./");
	}

	if (args->dir == NULL)
	{
		args->dir = strdup(args->name);
	}

	if (!is_valid_identifier(args->ident))
	{
		free(args->ident);
		args->ident = strdup(args->name);
	}

	if (args->lib == NULL)
	{
		args->lib = strdup(args->ident);
	}

	return 0;
}

bool is_valid_identifier(char *ident)
{
	if (ident == NULL)
	{
		return false;
	}

	regex_t regex;
	const char *ident_regex = "^[_a-zA-Z][_a-zA-Z0-9]*$";
	int flags = REG_EXTENDED;
	int error = regcomp(&regex, ident_regex, flags);
	if (error)
	{
		fprintf(stderr, "Failed to compile identifier regex\n");
		return false;
	}

	char *string = ident;
	size_t nmatch = 0;
	regmatch_t *pmatch = NULL;
	int eflags = 0;
	error = regexec(&regex, string, nmatch, pmatch, eflags);
	regfree(&regex);
	return !error;
}

void free_args(struct arguments *args)
{
	free(args->name);
	free(args->parent_dir);
	free(args->dir);
	free(args->ident);
	free(args->lib);
}

int create_raylib_hot_reload_template(struct arguments *args)
{
	char *cwd = get_current_dir_name();
	if (cwd == NULL)
	{
		perror("Unable to get CWD");
		return 1;
	}

	int error = chdir(args->parent_dir);
	if (error)
	{
		fprintf(stderr, "WARNING: Unable to change to project parent "
		       "directory (%s): %s", args->parent_dir, strerror(errno));
	}

	const char *template_dir = get_template_share_dir();
	if (template_dir == NULL)
	{
		return 2;
	}

	const char *new_dir = args->dir;
	error = copy_template_dir(template_dir, new_dir);
	if (error)
	{
		return error;
	}

	error = chdir(args->dir);
	if (error)
	{
		fprintf(stderr, "ERROR: Unable to enter project "
		       "directory (%s): %s", args->parent_dir, strerror(errno));
		return 5;
	}

	char *ident_upper;
	char *lib;
	char *lib_upper;
	create_replacement_strings(&ident_upper, &lib, &lib_upper, args);

	struct replacement replacements[] = {
		// original             , new
		{ "::NAME::"		, args->name },
		{ "::LIB::"		, lib },
		{ "::LIB_UPPER::"	, lib_upper },
		{ "::IDENT::"		, args->ident },
		{ "::IDENT_UPPER::"	, ident_upper },
		{ NULL		, NULL },
	};

	char *header_file_name;
	(void)asprintf(&header_file_name, "src/include/%s.h", args->ident);
	if (header_file_name == NULL)
	{
		fprintf(stderr, "WARNING: failed to rename header file\n");
	}

	open_and_replace("Makefile", NULL, replacements);
	open_and_replace("src/include/header.h", header_file_name,
			 replacements);
	open_and_replace("src/lib/close.c", NULL, replacements);
	open_and_replace("src/lib/init.c", NULL, replacements);
	open_and_replace("src/lib/save-restore.c", NULL, replacements);
	open_and_replace("src/lib/update.c", NULL, replacements);
	open_and_replace("src/main.c", NULL, replacements);

	free(header_file_name);
	free(ident_upper);
	free(lib);
	free(lib_upper);

	chdir(cwd);
	free(cwd);
	return 0;
}

const char *get_template_share_dir(void)
{
	DIR *dir = opendir(PRIMARY_TEMPLATE_DIR);
	if (dir != NULL)
	{
		closedir(dir);
		return PRIMARY_TEMPLATE_DIR;
	}

	dir = opendir(SECONDARY_TEMPLATE_DIR);
	if (dir != NULL)
	{
		closedir(dir);
		return SECONDARY_TEMPLATE_DIR;
	}

	fprintf(stderr, "Unable to open template directories: "
		PRIMARY_TEMPLATE_DIR " AND "
		SECONDARY_TEMPLATE_DIR "\n");
	return NULL;
}

int copy_template_dir(const char *template_dir, const char *new_dir)
{
	char *command;
	(void)asprintf(&command, "/usr/bin/cp -r %s ./%s", template_dir,
		       new_dir);
	if (command == NULL)
	{
		perror("ERROR: Failed to allocated a string");
		return 3;
	}
	int error = system(command);
	if (error)
	{
		fprintf(stderr, "ERROR: Failed to execute `%s`\n", command);
		free(command);
		return 4;
	}
	free(command);
	return 0;
}

void create_replacement_strings(char **ident_upper, char **lib,
				char **lib_upper, struct arguments *args)
{
	*ident_upper = strdup(args->ident);
	if (*ident_upper == NULL)
	{
		fprintf(stderr, "WARNING: can't create ident_upper\n");
		*ident_upper = args->ident;
	}
	else
	{
		uppercaseify(*ident_upper);
	}

	char lib_backup[] = "lib";
	(void)asprintf(lib, "lib%s", args->lib);
	if (*lib == NULL)
	{
		fprintf(stderr, "WARNING: can't create lib\n");
		*lib = lib_backup;
	}

	*lib_upper = strdup(*lib);
	if (*lib_upper == NULL)
	{
		fprintf(stderr, "WARNING: can't create lib_upper\n");
		*lib_upper = *ident_upper;
	}
	else
	{
		uppercaseify(*lib_upper);
	}
}

void uppercaseify(char *str)
{
	for (char *c = str; *c != '\0'; ++c)
	{
		*c = (char)toupper(*c);
	}
}

void open_and_replace(const char *file_name, const char *new_name,
		      struct replacement *replacements)
{
	size_t length;
	char *contents = read_entire_file(file_name, &length);
	if (NULL)
	{
		fprintf(stderr, "WARNING: unable to replace things in %s",
			file_name);
		return;
	}

	for (size_t i = 0; replacements[i].original != NULL; ++i)
	{
		char *new_contents = replace_strings(contents,
						     replacements[i].original,
						     replacements[i].new);

		if (new_contents == NULL)
		{
			fprintf(stderr, "WARNING: unable to replace "
				"%s in %s\n", replacements[i].original,
				file_name);
			continue;
		}

		free(contents);
		contents = new_contents;
	}

	FILE *fp = fopen(file_name, "w");
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open %s: %s", file_name,
			strerror(errno));
	}
	size_t file_size = strlen(contents);
	size_t bytes_written = fwrite(contents, sizeof(char), file_size, fp);
	if (bytes_written != file_size)
	{
		fprintf(stderr, "WARNING: failed to write new contents to %s\n"
			"\tExpected %lu, got %lu\n", file_name, file_size,
			bytes_written);
		perror("\tReason");
	}
	free(contents);
	fclose(fp);


	if (new_name == NULL)
	{
		return;
	}

	int error = rename(file_name, new_name);
	if (error)
	{
		fprintf(stderr, "WARNING: unable to rename %s to %s",
			file_name, new_name);
	}
}

char *read_entire_file(const char *file_path, size_t *length)
{
	*length = 0;
	FILE *file = fopen(file_path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Failed to open %s: %s\n",
			file_path, strerror(errno));
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	*length = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);
	char *buffer = malloc(*length + 1);
	if (buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate space for a buffer"
			" of size %lu: %s\n", (*length + 1), strerror(errno));
		fclose(file);
		return NULL;
	}

	fread(buffer, sizeof(char), *length, file);
	buffer[*length] = '\0';
	fclose(file);

	return buffer;
}

/*
  Replace all occurences of `old` in `text` with `new`
  Modified from [[https://stackoverflow.com/a/779960/23990444]]
  I don't really know what's going on here, but it works
 */
char *replace_strings(const char *restrict text,
		      const char *restrict old,
		      const char *restrict new)
{
	if (text == NULL || old == NULL)
	{
		return NULL;
	}
	size_t old_length = strlen(old);
	if (old_length == 0)
	{
		return NULL;
	}
	if (new == NULL)
	{
		new = "";
	}
	size_t new_length = strlen(new);

	size_t count;
	char *tmp = NULL;
	const char *insert = text;
	for (count = 0; (tmp = strstr(insert, old)); count++)
	{
		insert = tmp + old_length;
	}

	size_t result_length = strlen(text) +
		(new_length - old_length) * count + 1;
	char *result = malloc(result_length);
	tmp = result;
	if (result == NULL)
	{
		return NULL;
	}

	size_t front_length = 0;
	while (count--)
	{
		insert = strstr(text, old);
		front_length = (size_t)(insert - text);
		tmp = strncpy(tmp, text, front_length) + front_length;
		tmp = strcpy(tmp, new) + new_length;
		text += front_length + old_length;
	}
	strcpy(tmp, text);

	return result;
}

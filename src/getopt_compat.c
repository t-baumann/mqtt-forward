/**
 * Minimal getopt_long() implementation for native Win32 builds.
 *
 * Supports the subset of GNU getopt_long() behavior used by mqtt-forward:
 * short options (with or without a required argument) and long options
 * (with or without a required argument, accepted either as "--opt value"
 * or "--opt=value"). Option permutation and optional-argument long options
 * are not implemented since they are not exercised by this code base.
 */
#ifdef _WIN32

#include "getopt_compat.h"
#include <string.h>
#include <stdio.h>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt = 0;

static int char_pos = 1;

static const struct option *find_long_option(const struct option *longopts,
					     const char *name,
					     size_t name_len)
{
	const struct option *opt;

	for (opt = longopts; opt->name; opt++) {
		if (strlen(opt->name) == name_len &&
		    strncmp(opt->name, name, name_len) == 0)
			return opt;
	}

	return NULL;
}

static int handle_long_option(int argc,
			      char *const argv[],
			      const struct option *longopts,
			      int *longindex)
{
	const char *arg;
	const char *name;
	const char *eq;
	size_t name_len;
	const struct option *opt;
	int idx;

	arg = argv[optind] + 2;
	eq = strchr(arg, '=');
	name = arg;
	name_len = eq ? (size_t)(eq - arg) : strlen(arg);

	opt = find_long_option(longopts, name, name_len);
	if (!opt) {
		if (opterr)
			fprintf(stderr, "unrecognized option '--%s'\n", name);
		optind++;
		return '?';
	}

	if (longindex) {
		for (idx = 0; &longopts[idx] != opt; idx++)
			;
		*longindex = idx;
	}

	optind++;
	optarg = NULL;

	if (opt->has_arg == required_argument) {
		if (eq) {
			optarg = (char *)(eq + 1);
		} else if (optind < argc) {
			optarg = argv[optind];
			optind++;
		} else {
			if (opterr)
				fprintf(stderr,
					"option '--%s' requires an argument\n",
					opt->name);
			return '?';
		}
	} else if (opt->has_arg == optional_argument && eq) {
		optarg = (char *)(eq + 1);
	}

	if (opt->flag) {
		*opt->flag = opt->val;
		return 0;
	}

	return opt->val;
}

static int handle_short_option(int argc,
			       char *const argv[],
			       const char *optstring)
{
	char c;
	const char *arg;
	const char *match;

	arg = argv[optind];
	c = arg[char_pos];
	match = strchr(optstring, c);

	if (c == ':' || !match) {
		optopt = c;
		if (opterr)
			fprintf(stderr, "unknown option '-%c'\n", c);
		char_pos++;
		if (arg[char_pos] == '\0') {
			optind++;
			char_pos = 1;
		}
		return '?';
	}

	if (match[1] == ':') {
		if (arg[char_pos + 1] != '\0') {
			optarg = (char *)&arg[char_pos + 1];
			optind++;
		} else {
			optind++;
			if (optind < argc) {
				optarg = argv[optind];
				optind++;
			} else {
				optopt = c;
				if (opterr)
					fprintf(stderr,
						"option '-%c' requires an argument\n",
						c);
				char_pos = 1;
				return '?';
			}
		}
		char_pos = 1;
		return c;
	}

	optarg = NULL;
	char_pos++;
	if (arg[char_pos] == '\0') {
		optind++;
		char_pos = 1;
	}

	return c;
}

int getopt_long(int argc,
		char *const argv[],
		const char *optstring,
		const struct option *longopts,
		int *longindex)
{
	const char *arg;

	optarg = NULL;

	if (optind >= argc)
		return -1;

	arg = argv[optind];

	if (arg[0] != '-' || arg[1] == '\0')
		return -1;

	if (strcmp(arg, "--") == 0) {
		optind++;
		return -1;
	}

	if (arg[1] == '-')
		return handle_long_option(argc, argv, longopts, longindex);

	return handle_short_option(argc, argv, optstring);
}

#endif /* _WIN32 */

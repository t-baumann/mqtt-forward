#ifndef _GETOPT_COMPAT_H_
#define _GETOPT_COMPAT_H_

/**
 * getopt_long() is a POSIX/GNU extension not provided by the native Win32
 * toolchain headers. This header exposes the minimal subset of the API used
 * by mqtt-forward (short options, long options with required/no argument)
 * backed by a small vendored implementation in getopt_compat.c.
 *
 * On non-Windows builds the system <getopt.h> is used as before.
 */
#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif

#define no_argument       0
#define required_argument 1
#define optional_argument 2

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt_long(int argc,
		char *const argv[],
		const char *optstring,
		const struct option *longopts,
		int *longindex);

#ifdef __cplusplus
}
#endif

#else /* !_WIN32 */

#include <getopt.h>

#endif /* _WIN32 */

#endif /* _GETOPT_COMPAT_H_ */

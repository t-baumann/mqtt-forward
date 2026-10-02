#ifndef _COMPAT_WIN32_H_
#define _COMPAT_WIN32_H_

/**
 * Native Win32 compatibility shim.
 *
 * mqtt-forward was originally written against POSIX sockets, pthreads and
 * syslog. This header bridges the POSIX APIs used throughout the code base
 * to their native Win32/Winsock equivalents so the rest of the source can
 * stay largely unmodified. It is only included when building with _WIN32
 * defined (see the WIN32_SOCK/CLOSE_SOCK/etc. usages in session.c and
 * mqtt-forward.c).
 */
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

/* socket_t: native socket handle type (SOCKET on Windows, int on POSIX) */
typedef SOCKET socket_t;
#define INVALID_SOCK INVALID_SOCKET
#define CLOSE_SOCK(s) closesocket(s)
#define SOCK_ERRNO() WSAGetLastError()

/* shutdown() second argument */
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif

/* send() flags: MSG_NOSIGNAL has no Windows equivalent (no SIGPIPE there) */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

/* poll()/pollfd: map directly onto WSAPoll()/WSAPOLLFD */
typedef WSAPOLLFD compat_pollfd_t;
#define poll(fds, nfds, timeout) WSAPoll((fds), (nfds), (timeout))

/* syslog severity levels (only used as plain severity constants here) */
#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

/* inet_aton() is a BSD legacy call not provided by Winsock */
static inline int inet_aton(const char *cp, struct in_addr *addr)
{
	return inet_pton(AF_INET, cp, addr) == 1;
}

/* Socket-related error codes referenced by the port */
#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#else /* !_WIN32 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

typedef int socket_t;
#define INVALID_SOCK (-1)
#define CLOSE_SOCK(s) close(s)
#define SOCK_ERRNO() errno

typedef struct pollfd compat_pollfd_t;

#endif /* _WIN32 */

#endif /* _COMPAT_WIN32_H_ */

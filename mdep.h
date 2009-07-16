/*******************************************************************************
 * Copyright (c) 2007, 2008 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *******************************************************************************/

/*
 * Machine and OS dependend definitions.
 * This module implements host OS abstraction layer that helps make
 * agent code portable between Linux, Windows, VxWorks and potentially other OSes.
 *
 * mdep.h must be included first, before any other header files.
 */

#ifndef D_mdep
#define D_mdep

#if defined(WIN32) || defined(__CYGWIN__)
/* MS Windows NT/XP */

#define _WIN32_WINNT 0x0501

#if defined(__CYGWIN__)
#  define _WIN32_IE 0x0501
#elif defined(__MINGW32__)
#  define _WIN32_IE 0x0501
#elif defined(_MSC_VER)
#  pragma warning(disable:4100) /* unreferenced formal parameter */
#  pragma warning(disable:4996) /* 'strcpy': This function or variable may be unsafe */
#  pragma warning(disable:4152) /* nonstandard extension, function/data pointer conversion in expression */
#  ifdef _DEBUG
#    define _CRTDBG_MAP_ALLOC
#    include <stdlib.h>
#    include <crtdbg.h>
#  endif
#  define _WSPIAPI_H_
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <io.h>

typedef __int64 int64;
typedef unsigned __int64 uns64;

#define FILE_PATH_SIZE MAX_PATH

typedef int socklen_t;

typedef CONTEXT REG_SET;
#define get_regs_SP(x) ((x).Esp)
#define get_regs_BP(x) ((x).Ebp)
#define get_regs_PC(x) ((x).Eip)
#define set_regs_PC(x,y) (x).Eip = (y)

extern unsigned char BREAK_INST[];  /* breakpoint instruction */
#define BREAK_SIZE 1                /* breakpoint instruction size */

#if defined(__CYGWIN__)

#ifndef _LARGEFILE_SOURCE
#error "Need CC command line option: -D_LARGEFILE_SOURCE"
#endif

#ifndef _GNU_SOURCE
#error "Need CC command line option: -D_GNU_SOURCE"
#endif

#include <sys/unistd.h>

typedef struct stat struct_stat;

extern void __stdcall freeaddrinfo(struct addrinfo *);
extern int __stdcall getaddrinfo(const char *, const char *,
                const struct addrinfo *, struct addrinfo **);
extern const char * loc_gai_strerror(int ecode);

#else /* not __CYGWIN__ */

#include <direct.h>

struct timespec {
    time_t  tv_sec;         /* seconds */
    long    tv_nsec;        /* nanoseconds */
};

#define SIGTRAP 5
#define SIGKILL 9
#define SIGSTOP 19

#define ETIMEDOUT 100

#if defined(__MINGW32__)
typedef unsigned int useconds_t;
#elif defined(_MSC_VER)
#define __i386__
typedef unsigned long pid_t;
typedef unsigned long useconds_t;
#endif

#define CLOCK_REALTIME 1
typedef int clockid_t;
extern int clock_gettime(clockid_t clock_id, struct timespec * tp);
extern void usleep(useconds_t useconds);

#define lseek _lseeki64
typedef struct _stati64 struct_stat;
#define stat _stati64
#define lstat _stati64
#define fstat _fstati64
extern int truncate(const char * path, int64 size);
extern int ftruncate(int f, int64 size);
#define utimbuf _utimbuf
#define utime _utime
#define futime _futime
#define snprintf _snprintf

#define loc_gai_strerror gai_strerror

extern int getuid(void);
extern int geteuid(void);
extern int getgid(void);
extern int getegid(void);

#endif /* __CYGWIN__ */

#define MSG_MORE 0

extern const char * inet_ntop(int af, const void * src, char * dst, socklen_t size);
extern int inet_pton(int af, const char * src, void * dst);

/*
 * readdir() emulation
 */
#if !defined(__GNUC__)
struct DIR {
  long hdl;
  struct _finddatai64_t blk;
  char path[FILE_PATH_SIZE];
};

struct dirent {
  char d_name[FILE_PATH_SIZE];
  int64 d_size;
  time_t d_atime;
  time_t d_ctime;
  time_t d_wtime;
};

typedef struct DIR DIR;

extern DIR * opendir(const char * path);
extern int closedir(DIR * dir);
extern struct dirent * readdir(DIR * dir);
#endif /* !__GNUC__ */

/*
 * PThreads emulation.
 */
#if defined(__CYGWIN__)
#  include <cygwin/types.h>
#else
typedef void * pthread_t;
typedef void * pthread_attr_t;
typedef void * pthread_mutex_t;
typedef void * pthread_cond_t;
typedef void * pthread_mutexattr_t;
typedef void * pthread_condattr_t;
#endif

extern int pthread_attr_init(pthread_attr_t * attr);
extern int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutexattr_t * attr);
extern int pthread_cond_init(pthread_cond_t * cond, const pthread_condattr_t * attr);
extern int pthread_cond_destroy(pthread_cond_t * cond);

extern int pthread_cond_signal(pthread_cond_t * cond);
extern int pthread_cond_broadcast(pthread_cond_t * cond);
extern int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
extern int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex,
                                  const struct timespec * abstime);
extern int pthread_mutex_lock(pthread_mutex_t * mutex);
extern int pthread_mutex_unlock(pthread_mutex_t * mutex);
extern pthread_t pthread_self(void);
extern int pthread_create(pthread_t * thread, const pthread_attr_t * attr,
                          void * (*start_routine)(void *), void * arg);
extern int pthread_join(pthread_t thread, void **value_ptr);

/*
 * Windows socket functions don't set errno as expected.
 * Wrappers are provided to workaround the problem.
 * TODO: more socket function wrappers are needed for better error reports on Windows
 */
#define socket(af, type, protocol) wsa_socket(af, type, protocol)
#define bind(socket, addr, addr_size) wsa_bind(socket, addr, addr_size)
#define listen(socket, size) wsa_listen(socket, size)
#define recv(socket, buf, size, flags) wsa_recv(socket, buf, size, flags)
#define recvfrom(socket, buf, size, flags, addr, addr_size) wsa_recvfrom(socket, buf, size, flags, addr, addr_size)
#define send(socket, buf, size, flags) wsa_send(socket, buf, size, flags)
#define sendto(socket, buf, size, flags, dest_addr, dest_size) wsa_sendto(socket, buf, size, flags, dest_addr, dest_size)

extern int wsa_socket(int af, int type, int protocol);
extern int wsa_bind(int socket, const struct sockaddr * addr, int addr_size);
extern int wsa_listen(int socket, int size);
extern int wsa_recv(int socket, void * buf, size_t size, int flags);
extern int wsa_recvfrom(int socket, void * buf, size_t size, int flags,
                    struct sockaddr * addr, socklen_t * addr_size);
extern int wsa_send(int socket, const void * buf, size_t size, int flags);
extern int wsa_sendto(int socket, const void * buf, size_t size, int flags,
                  const struct sockaddr * dest_addr, socklen_t dest_size);

extern char * canonicalize_file_name(const char * path);

#define O_LARGEFILE 0

#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo

#elif defined(_WRS_KERNEL)
/* VxWork kernel module */

#define INET

#include <vxWorks.h>
#include <inetLib.h>
#include <regs.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <selectLib.h>
#include <wrn/coreip/sockLib.h>
#include <wrn/coreip/hostLib.h>
#if _WRS_VXWORKS_MAJOR > 6 || _WRS_VXWORKS_MAJOR == 6 && _WRS_VXWORKS_MINOR >= 7
#  include <private/taskLibP.h>
#  define kernelVersion() vxWorksVersion
#endif

#define environ taskIdCurrent->ppEnviron

#define get_regs_PC(x) (*(int *)((int)&(x) + PC_OFFSET))
#define set_regs_PC(x,y) *(int *)((int)&(x) + PC_OFFSET) = (int)(y)

#define closesocket close

typedef long long int64;
typedef unsigned long long uns64;
typedef unsigned long useconds_t;

#define FILE_PATH_SIZE PATH_MAX
#define O_BINARY 0
#define O_LARGEFILE 0
#define lstat stat
typedef struct stat struct_stat;
#define ifr_netmask ifr_addr
#define SA_LEN(addr) ((addr)->sa_len)
#define MSG_MORE 0

extern int truncate(char * path, int64 size);
extern char * canonicalize_file_name(const char * path);

extern void usleep(useconds_t useconds);

extern int getuid(void);
extern int geteuid(void);
extern int getgid(void);
extern int getegid(void);

extern void loc_freeaddrinfo(struct addrinfo * ai);
extern int loc_getaddrinfo(const char * nodename, const char * servname,
       const struct addrinfo * hints, struct addrinfo ** res);
extern const char * loc_gai_strerror(int ecode);

#elif defined(__APPLE__)
#ifndef _LARGEFILE_SOURCE
#error "Need CC command line option: -D_LARGEFILE_SOURCE"
#endif

#ifndef _GNU_SOURCE
#error "Need CC command line option: -D_GNU_SOURCE"
#endif

#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <limits.h>

#include <mach/thread_status.h>
typedef x86_thread_state32_t REG_SET;

#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo
#define loc_gai_strerror gai_strerror

#define O_BINARY 0
#define O_LARGEFILE 0
#define FILE_PATH_SIZE PATH_MAX
#define closesocket close
#define ifr_netmask ifr_addr
#define canonicalize_file_name(path)    realpath(path, NULL)

typedef int64_t int64;
typedef uint64_t uns64;
typedef struct stat struct_stat;

#define get_regs_SP(x) ((x).__esp)
#define get_regs_BP(x) ((x).__ebp)
#define get_regs_PC(x) ((x).__eip)
#define set_regs_PC(x,y) (x).__eip = (unsigned long)(y)

#ifndef SA_LEN
# ifdef HAVE_SOCKADDR_SA_LEN
#  define SA_LEN(addr) ((addr)->sa_len)
# else /* HAVE_SOCKADDR_SA_LEN */
#  ifdef HAVE_STRUCT_SOCKADDR_STORAGE
static size_t get_sa_len(const struct sockaddr *addr) {
    switch (addr->sa_family) {
#   ifdef AF_UNIX
        case AF_UNIX: return sizeof(struct sockaddr_un);
#   endif
#   ifdef AF_INET
        case AF_INET: return (sizeof (struct sockaddr_in));
#   endif
#   ifdef AF_INET6
        case AF_INET6: return (sizeof (struct sockaddr_in6));
#   endif
        default: return (sizeof (struct sockaddr));
    }
}
#   define SA_LEN(addr)   (get_sa_len(addr))
#  else /* HAVE_SOCKADDR_STORAGE */
#   define SA_LEN(addr)   (sizeof (struct sockaddr))
#  endif /* HAVE_SOCKADDR_STORAGE */
# endif /* HAVE_SOCKADDR_SA_LEN */
#endif /* SA_LEN */

extern unsigned char BREAK_INST[];  /* breakpoint instruction */
#define BREAK_SIZE 1                /* breakpoint instruction size */

#define CLOCK_REALTIME 1
typedef int clockid_t;
extern int clock_gettime(clockid_t clock_id, struct timespec * tp);

extern char **environ;

/* Mac OS X */
#else
/* Linux, UNIX */

#ifndef _LARGEFILE_SOURCE
#error "Need CC command line option: -D_LARGEFILE_SOURCE"
#endif

#ifndef _GNU_SOURCE
#error "Need CC command line option: -D_GNU_SOURCE"
#endif

#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <limits.h>

#include <sys/user.h>
typedef struct user_regs_struct REG_SET;

#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo
#define loc_gai_strerror gai_strerror

#define O_BINARY 0

extern int tkill(pid_t pid, int signal);

#define FILE_PATH_SIZE PATH_MAX

#define closesocket close

typedef __int64_t int64;
typedef __uint64_t uns64;
typedef struct stat struct_stat;

#define get_regs_SP(x) ((x).esp)
#define get_regs_BP(x) ((x).ebp)
#define get_regs_PC(x) ((x).eip)
#define set_regs_PC(x,y) (x).eip = (unsigned long)(y)

extern unsigned char BREAK_INST[];  /* breakpoint instruction */
#define BREAK_SIZE 1                /* breakpoint instruction size */

#ifndef SA_LEN
# ifdef HAVE_SOCKADDR_SA_LEN
#  define SA_LEN(addr) ((addr)->sa_len)
# else /* HAVE_SOCKADDR_SA_LEN */
#  ifdef HAVE_STRUCT_SOCKADDR_STORAGE
static size_t get_sa_len(const struct sockaddr *addr) {
    switch (addr->sa_family) {
#   ifdef AF_UNIX
        case AF_UNIX: return sizeof(struct sockaddr_un);
#   endif
#   ifdef AF_INET
        case AF_INET: return (sizeof (struct sockaddr_in));
#   endif
#   ifdef AF_INET6
        case AF_INET6: return (sizeof (struct sockaddr_in6));
#   endif
        default: return (sizeof (struct sockaddr));
    }
}
#   define SA_LEN(addr)   (get_sa_len(addr))
#  else /* HAVE_SOCKADDR_STORAGE */
#   define SA_LEN(addr)   (sizeof (struct sockaddr))
#  endif /* HAVE_SOCKADDR_STORAGE */
# endif /* HAVE_SOCKADDR_SA_LEN */
#endif /* SA_LEN */

#endif

extern pthread_attr_t pthread_create_attr;

/* Return Operating System name */
extern char * get_os_name(void);

/* Get user home directory path */
extern char * get_user_home(void);

/* Switch to running in the background, rather than under the direct control of a user */
extern void become_daemon(void);

/* Return 1 if running in the background, return 0 othewise */
extern int is_daemon(void);

/* Initialize mdep module */
extern void ini_mdep(void);

#endif

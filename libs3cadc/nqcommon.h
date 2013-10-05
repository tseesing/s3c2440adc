/*
 * This file licensed under the GNU General Public License.
 * http://www.gnu.org/licenses/gpl.txt
 */


#ifndef NQ_COMMON_H__
#define NQ_COMMON_H__


#define NQMIN(a, b) ((a) < (b) ? (a) : (b))
#define NQMAX(a, b) ((a) > (b) ? (a) : (b))

/*
#define TRIPLE_MAX(a, b, c) \
    (((((a) > (b)) ? (a) : (b)) > (c) ) ? (((a) > (b)) ? (a) : (b)) : (c) )
 */
#define NQTRIPLE_MAX(a, b, c) NQMAX(NQMAX((a), (b)), (c))
#define NQTRIPLE_MIN(a, b, c) NQMIN(NQMIN((a), (b)), (c))

#define NQTRIPLE_MID(a, b, c) \
    ((a) > (b) ? ((b) > (c) ? (b) : ((a) < (c) ? (a) : (c))) : ((a) > (c) ? (a) : ((b) < (c) ? (b) : (c))))


#ifndef MAXLINE
#define MAXLINE 1024
#endif


#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

int nq_set_fd_nonblocking(int fd);



#ifdef NQDEBUG

#define nq_errmsg(fmt, args...) \
    do {\
        fprintf(stderr, "Error:  %s:%d " fmt "\n", __FILE__, __LINE__, ##args); \
    } while(0)

#define nq_errmsg_exit(fmt, args...) \
    do {\
        fprintf(stderr, "Error: %s:%d " fmt "\n", __FILE__, __LINE__, ##args); \
        exit(1); \
    } while(0)

#define nq_warnmsg(fmt, args...) \
    do {\
        fprintf(stderr, "Warning: %s:%d " fmt "\n", __FILE__, __LINE__, ##args); \
    } while(0)

#define nq_normsg(fmt, args...) \
    do {\
        fprintf(stdout, fmt "\n", ##args); \
    } while(0)

#define nq_debug(fmt, args...) \
    do {\
        fprintf(stdout, "Debug: %s:%d " fmt "\n", __FILE__, __LINE__, ##args); \
    } while(0)


#else

#include <stdarg.h>

void nq_errmsg_exit(char *fmt, ...);
void nq_warnmsg(char *fmt, ...);
void nq_errmsg(char *fmt, ...);
void nq_normsg(char *fmt, ...);

#define nq_debug(fmt, args...) \
    do {\
    } while(0)

#endif /* NQDEBUG */


off_t nq_get_file_length(char * path);

int nq_regex_match(char *s, char *pattern, int cflags, int eflags, size_t nmatch, regmatch_t *offset);


#endif /* NQ_COMMON_H__ */

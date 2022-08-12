#ifndef FT_PING_H
# define FT_PING_H

# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <signal.h>

/**
 * @brief arguments struct
 * Store command line arguments
 */
typedef struct s_args {
    char	*hostname;
    int		verbose;
    int		sweepincrsize;
} t_args;

t_args  *parse_clo(int len, char *clo_args[]);

#endif

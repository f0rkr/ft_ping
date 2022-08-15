#include "ft_ping.h"

/**
 * @brief Parsing command line
 * parse command line argumments 
 * and store valid arguments in t_args struct
 */
t_args  *parse_clo(int len, char *clo_args[]) {
    t_args *args;
    int i;

    i = 1;
    args = (t_args *) malloc(sizeof(t_args) * 1);
    if (args == NULL)
        return (args);
    while (i < len)
    {
        if (strcmp(clo_args[i], "-h") == 0)
            args->sweepincrsize = 1;
        else if (strcmp(clo_args[i], "-v") == 0)
            args->verbose = 1;
        else
            args->hostname = strdup(clo_args[i]);
        i++;
    }
    return (args);
}

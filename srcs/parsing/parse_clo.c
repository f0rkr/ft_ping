#include "ft_ping.h"

/** @Brief Init Args
 * Initialize args struct
 *
 * @param none
 * @return t_args*
 */
t_args  *init_args( void )
{
    t_args *args;

    args = (t_args *) malloc(sizeof(t_args) * 1);
    if (args == NULL)
        return (args);
    args->help = 0;
    args->verbose = 0;
    args->hostname = NULL;
    args->sweepincrsize = 0;
    args->invalid_option = 0;
    return (args);
}

/**
 * @brief Parsing command line
 * parse command line argumments 
 * and store valid arguments in t_args struct
 */
t_args  *parse_clo(int len, char *clo_args[]) {
    t_args *args;
    int i;

    i = 1;
    args = init_args();
    if (args == NULL)
        return (args);

    // TO-DO: Parsing only valid options and break if any invalid_option appears
    while (i < len && clo_args[i][0] == '-') {
        if (strcmp(clo_args[i], "-?") == 0)
            args->help = 1;
        else if (strcmp(clo_args[i], "-v") == 0)
            args->verbose = 1;
        else
        {
            printf("ft_ping: invalid option -- %s\n", &clo_args[i][1]);
            args->invalid_option = 1;
            break;
        }
        i++;
    }
    if (i + 1 < len || i == len || args->help || args->invalid_option)
    {
        free(args);
        args = NULL;
        return (NULL);
    }
    args->hostname = strdup(clo_args[i]);
    return (args);
}

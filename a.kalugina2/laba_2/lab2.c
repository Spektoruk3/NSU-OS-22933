#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *arr[];

main()
{
    time_t moment;
    setenv("TZ", "AMERICA/LOS_ANGELES", 1);
    tzset();

    struct tm *swamp;
    (void) time( &moment );
    
    printf("%s", ctime( &moment ) );
    swamp = localtime(&moment);
    
    printf("%d/%d/%02d %d:%02d %s\n",
        swamp->tm_year, swamp->tm_hour,
        swamp->tm_mon + 1, swamp->tm_mday,
        swamp->tm_min, arr[swamp->tm_isdst]);
    
    exit(0);
}

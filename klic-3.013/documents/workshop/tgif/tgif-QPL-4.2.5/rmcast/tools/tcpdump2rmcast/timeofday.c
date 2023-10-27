#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main()
{
    struct timeval time_of_day;
    struct tm time;
    
    gettimeofday(&time_of_day, 0);
   
    fprintf(stderr,"%ld:%ld\n", 
            time_of_day.tv_sec, time_of_day.tv_usec );
    
    fprintf(stderr,"%s", ctime((const time_t*)&time_of_day));            
    
    tm = *((const time_t*)&time_of_day);

    time_of_day. tv_sec =     ( (tm.tm_hour * 60 + tm.tm_min) * 60 ) + tm.tm_sec;
        
        
    return 1;
}    



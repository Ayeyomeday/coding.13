/*
Question 1:
The gettimeofday() function measures time in microseconds. Since memory accesses happen 
in nanoseconds, a single access is too fast to measure accurately. We can repeat the 
memory accesses many times and divide the total time by the number of accesses to get an average.

Question 2:
The program takes the number of pages and the number of trials as its two inputs. 
It allocates an array and accesses one integer from each page repeatedly. It then 
measures the total time and calculates the average time per access.

Question 5:
To prevent compiler optimization from removing the loop, we can use volatile to make sure 
the memory accesses actually happen. We can also print a value from the array after the loop.

Question 6:
We can use the Linux taskset command to keep the program running on one CPU core. If the program 
switches between cores, it may encounter different TLB contents, causing additional misses and 
less accurate timing results.

Question 7:
Yes, not initializing the array can affect the timing because the OS may need to allocate and 
initialize memory during the first accesses. We can initialize the array before starting the 
timer to avoid extra costs.

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pages> <trials>\n", argv[0]);
        return 1;
    }

    int pages = atoi(argv[1]);
    int trials = atoi(argv[2]);
    if (pages <= 0 || trials <= 0) {
        printf("Pages and trials must be positive.\n");
        return 1;
    }

    long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0) {
        printf("Could not determine page size.\n");
        return 1;
    }

    size_t jump = (size_t)pageSize / sizeof(int);
    size_t total = (size_t)pages * jump;
    volatile int *array = calloc(total, sizeof(int));

    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Page Initialization
    for (size_t i = 0; i < total; i += jump) {
        array[i] = 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int t = 0; t < trials; t++) {
        for (size_t i = 0; i < total; i += jump) {
            array[i] += 1;
        }
    }

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
    double accesses = (double)pages * trials;
    double average = (elapsed * 1000.0) / accesses;

    printf("Pages: %d\n", pages);
    printf("Trials: %d\n", trials);
    printf("Total time: %.2f microseconds\n", elapsed);
    printf("Average time per access: %.2f nanoseconds\n", average);
    printf("Check: %d\n", array[0]);
    free((void *)array);

    return 0;
}
#if 0
#define START_TIMER clock_t t0; t0 = clock();
#define END_TIMER printf("Time elapsed doing work: %lf\n", (clock() - t0) / (double) CLOCKS_PER_SEC);
#else
#define START_TIMER struct timeval t0, t1; gettimeofday(&t0, 0);
#define END_TIMER gettimeofday(&t1, 0); printf("Time elapsed: %.02lf ms\n", (t1.tv_sec-t0.tv_sec)*1000.0 + (t1.tv_usec-t0.tv_usec)/1000.0);
#endif

typedef enum {
    OPTION_1 = 0x44,
    OPTION_2,
    OPTION_3,
    OPTION_4,
    OPTION_5,
    OPTION_6
} MessageType;

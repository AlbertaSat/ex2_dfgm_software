#ifndef DFGM_FILTER_H
#define DFGM_FILTER_H

#include <time.h>

struct SECOND {
    time_t time;
    double X[100];
    double Y[100];
    double Z[100];
    char flag;
    double Xfilt;
    double Yfilt;
    double Zfilt;
};

typedef struct __attribute__((packed)) {
    time_t time;
    uint32_t X;
    uint32_t Y;
    uint32_t Z;
} dfgm_data_sample_t;

// Functions from the original filter code
//void read_second(struct SECOND *ptr); // Unused
//void print_result(struct SECOND *ptr); // Unused
//void print_raw(struct SECOND *ptr); // Unused
//void print_mean(struct SECOND *ptr); // Unused
void shift_sptr(void);
void apply_filter(void);

// New functions for binary testing
void save_second(struct SECOND *second, char * filename1Hz);
void convert_100Hz_to_1Hz(char * filename100Hz, char * filename1Hz);

#endif /* DFGM_FILTER_H_ */

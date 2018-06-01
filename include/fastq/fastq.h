// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on May. 3, 2018
//
// This file is the header file of reading fastq file.

#ifndef FASTQ_FASTQ_H_
#define FASTQ_FASTQ_H_

// C libraries
#include <stdlib.h>
#include <zlib.h>

// Project's other libraries
#include "fastq/quality_system.h"

#define MAX_READ_CHECK 4000000

typedef struct Read {
    int id_length;
    int seq_length;
    int desc_length;
    int qual_length;
    int barcode_length;

    char *id;
    char *seq;
    char *desc;
    char *qual;
    char *barcode;
    char *barcode_qual;

    qual_sys_t const *qual_sys;
} read_t;

typedef struct Fastq {
    char const *file_name;
    // int file_name_length;
    gzFile file;

    int max_read_length;
    qual_sys_t const *qual_sys;
} fastq_t;

/* Fastq I/O control */
fastq_t *fastq_open(char const *file_name, qual_sys_t const *qual_sys);
void fastq_close(fastq_t *fastq);
void fastq_seek(fastq_t *fastq, int nth_read);  // Skip (nth_read - 1) reads
void fastq_reload(fastq_t *fastq);

/* Fastq quality system */
void fastq_check(fastq_t *fastq, int verbose_level);
void convert_qual_sys(read_t *read, qual_sys_t const *to_qual_sys);

/* Read */
read_t *init_read(void);
read_t *make_read(char const *id, char const *seq, char const *desc, 
                  char const *qual, char const *barcode, qual_sys_t const *qual_sys);
void copy_read(read_t const *src, read_t *dst);
int get_read(fastq_t *fastq, read_t *read);
void clear_read(read_t *read);
void destroy_read(read_t *read);

#endif  // FASTQ_FASTQ_H_

// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on Apr. 26, 2018
//
// This file is the header file for quality system.

#ifndef FASTQ_QUALITY_SYSTEM_H_
#define FASTQ_QUALITY_SYSTEM__H_

typedef struct Qual_sys {
    char name[16];
    char zero_qual_char;
    char min_qual_char;
    char max_qual_char;
    int qual_length;
} qual_sys_t;

extern qual_sys_t const *g_k_sanger;
extern qual_sys_t const *g_k_solexa;
extern qual_sys_t const *g_k_illumina_1_3;
extern qual_sys_t const *g_k_illumina_1_5;
extern qual_sys_t const *g_k_illumina_1_8;

extern qual_sys_t const *g_k_qual_sys_pool[5];

#endif  // FASTQ_QUALITY_SYSTEM__H_

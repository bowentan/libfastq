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

typedef struct qual_sys_s {
    char name[16];
    char zero_qual_char;
    char min_qual_char;
    char max_qual_char;
} qual_sys_t;

#define SANGER {"Sanger", '!', '!', 'I'}
#define SOLEXA {"Solexa", '@', ';', 'h'}
#define ILLUMINA_1_3 {"Illumina 1.3+", '@', '@', 'h'}
#define ILLUMINA_1_5 {"Illumina 1.5+", '@', 'B', 'i'}
#define ILLUMINA_1_8 {"Illumina 1.8+", '!', '!', 'J'}

extern qual_sys_t const g_k_sanger;
extern qual_sys_t const g_k_solexa;
extern qual_sys_t const g_k_illumina_1_3;
extern qual_sys_t const g_k_illumina_1_5;
extern qual_sys_t const g_k_illumina_1_8;

extern qual_sys_t const g_k_qual_sys_pool[5];

// extern qual_sys_t const g_k_sanger;
// extern qual_sys_t const g_k_solexa;
// extern qual_sys_t const g_k_illumina_1_3;
// extern qual_sys_t const g_k_illumina_1_5;
// extern qual_sys_t const g_k_illumina_1_8;
// 
// extern qual_sys_t const g_k_qual_sys_pool[5];

#endif  // FASTQ_QUALITY_SYSTEM__H_

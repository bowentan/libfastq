// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on Apr. 26, 2018
//
// This file is the source for quality system.

// Self include
#include "fastq/quality_system.h"

#define SANGER {"Sanger", '!', '!', 'I', 'I' - '!' + 1}
#define SOLEXA {"Solexa", '@', ';', 'h', 'h' - ';' + 1}
#define ILLUMINA_1_3 {"Illumina 1.3+", '@', '@', 'h', 'h' - '@' + 1}
#define ILLUMINA_1_5 {"Illumina 1.5+", '@', 'B', 'i', 'i' - 'B' + 1}
#define ILLUMINA_1_8 {"Illumina 1.8+", '!', '!', 'J', 'J' - '!' + 1}

static qual_sys_t const _k_sanger = SANGER;
static qual_sys_t const _k_solexa = SOLEXA;
static qual_sys_t const _k_illumina_1_3 = ILLUMINA_1_3;
static qual_sys_t const _k_illumina_1_5 = ILLUMINA_1_5;
static qual_sys_t const _k_illumina_1_8 = ILLUMINA_1_8;

qual_sys_t const *g_k_sanger = &_k_sanger;
qual_sys_t const *g_k_solexa = &_k_solexa;
qual_sys_t const *g_k_illumina_1_3 = &_k_illumina_1_3;
qual_sys_t const *g_k_illumina_1_5 = &_k_illumina_1_5;
qual_sys_t const *g_k_illumina_1_8 = &_k_illumina_1_8;

qual_sys_t const *g_k_qual_sys_pool[5] = {
    &_k_sanger,
    &_k_solexa,
    &_k_illumina_1_3,
    &_k_illumina_1_5,
    &_k_illumina_1_8
};

// g_k_qual_sys_pool[0] = g_k_sanger;

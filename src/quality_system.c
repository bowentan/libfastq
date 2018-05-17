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

qual_sys_t const g_k_sanger = SANGER;
qual_sys_t const g_k_solexa = SOLEXA;
qual_sys_t const g_k_illumina_1_3 = ILLUMINA_1_3;
qual_sys_t const g_k_illumina_1_5 = ILLUMINA_1_5;
qual_sys_t const g_k_illumina_1_8 = ILLUMINA_1_8;

qual_sys_t const g_k_qual_sys_pool[5] = {SANGER, SOLEXA, ILLUMINA_1_3, ILLUMINA_1_5, ILLUMINA_1_8};

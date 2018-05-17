// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on Apr. 26, 2018
//
// This file is the header file of barcodes and white list.

#ifndef FASTQ_BARCODE_H_
#define FASTQ_BARCODE_H_

// Project's other libraries
#include "fastq/fastq.h"

typedef struct barcode_s {
    char* codes;
    int length;

    int count;  // The observed number in fastq
    int dummy_count;  // = count + 1, used for calculating frequency
    double frequency;
} barcode_t;

typedef struct white_list_s {
    // int file_name_len;
    char const* file_name;
    int barcode_length;

    int size;
    barcode_t** barcodes;  // Array of barcode_t

    int observed_count;  // The number of barcodes that are observed in fastq.
    int dummy_observed_count;  // For calculate frequencies, each count will be added by 1.
} white_list_t;

/* Barcode */
barcode_t* init_barcode(void);
barcode_t* init_barcode_with_size(int size);
barcode_t* init_barcode_with_code(char const* codes);
void destroy_barcode(barcode_t* barcode);

/* White list */
static int _compare_barcode(void const* a, void const* b);
static void _add_barcode(white_list_t* wl, barcode_t* barcode);
static int _check_duplicates(white_list_t* wl);

// Read 
white_list_t* load_white_list(char const* file_name);
white_list_t* load_white_list_count(char const* file_name);
void destroy_white_list(white_list_t* wl);

// Write
void write_white_list_count(white_list_t* wl, char const* file_name);

// Distribution and interaction with fastq
void extract_barcode(read_t* read, int length);
void count_barcode(white_list_t* wl, fastq_t* fastq, int barcode_length);
void calculate_barcode_freq(white_list_t* wl);

#endif  // FASTQ_BARCODE_H_

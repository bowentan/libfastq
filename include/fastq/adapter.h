// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Jun. 01, 2018
// Lastest revised on Jun. 01, 2018
//
// This file is the header file of adapter processor.

#ifndef FASTQ_ADAPTER_H_
#define FASTQ_ADAPTER_H_

typedef struct Adapter {
    char *read_id;
    char *adapter_id;
    int adapter_length;
    int adapter_start;
    int adapter_end;
    int align_length;
    int n_mismatch;

    int read_length;
    int read_start;
    int read_end;
} adapter_t;

typedef struct AdapterList {
    char const *file_name;

    int size;
    adapter_t **adapters;
} adapter_list_t;

adapter_t *init_adapter();
void destroy_adapter(adapter_t *adapter);

void destroy_adapter_list(adapter_list_t *list);

adapter_list_t *load_adapter(char const *file_name);

adapter_t *find_adapter_by_read_id(adapter_list_t *list, char const *read_id);

#endif  // FASTQ_ADAPTER_H_

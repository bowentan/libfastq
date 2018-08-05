// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on Jun. 05, 2018
//
// This file is the source file of barcodes and white list.

// Self include
#include "fastq/barcode.h"

// C libraries
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include <stdio.h>

// Project's other libraries
#include "fastq/log.h"

#define BUFFER_SIZE 8196

static int _compare_barcode(void const *a, void const *b) {
    return strcmp((*((barcode_t **)a))->codes, (*((barcode_t **)b))->codes);
}

static void _add_barcode(white_list_t *wl, barcode_t *barcode) {
    wl->size++;
    wl->barcodes = realloc(wl->barcodes, wl->size * sizeof(barcode_t *));
    wl->barcodes[wl->size - 1] = barcode;
}

static int _check_duplicates(white_list_t *wl) {
    for (int i = 0; i < wl->size - 1; i++) {
        if (strcmp(wl->barcodes[i]->codes, wl->barcodes[i + 1]->codes) == 0) {
            return i;
        }
    }
    return -1;
}


barcode_t *init_barcode(void) {
    barcode_t *barcode = malloc(sizeof(barcode_t));
    barcode->length = 0;
    barcode->codes = NULL;
    barcode->count = 0;
    barcode->frequency = 0;
    return barcode;
}

barcode_t *init_barcode_with_size(int size) {
    barcode_t *barcode = malloc(sizeof(barcode_t));
    barcode->length = size;
    barcode->codes = malloc((size + 1) * sizeof(char));
    barcode->count = 0;
    barcode->frequency = 0;
    return barcode;
}

barcode_t *init_barcode_with_code(char const *codes) {
    barcode_t *barcode = malloc(sizeof(barcode_t));
    barcode->length = strlen(codes);
    barcode->codes = malloc((barcode->length + 1) * sizeof(char));
    strcpy(barcode->codes, codes);
    barcode->count = 0;
    barcode->frequency = 0;
    return barcode;
}

void destroy_barcode(barcode_t *barcode) {
    free(barcode->codes);
    barcode->codes = NULL;
    free(barcode);
    barcode = NULL;
}

white_list_t *load_white_list(char const *file_name) {
    // int file_name_len = strlen(file_name);
    
    white_list_t *wl = malloc(sizeof(white_list_t));
    wl->file_name = file_name;
    // wl->file_name = malloc(file_name_len);
    // strcpy(wl->file_name, file_name);
    wl->size = 0;
    wl->barcodes = malloc(wl->size * sizeof(barcode_t *));

    gzFile wl_file = gzopen(file_name, "rb");
    if (!wl_file) {
        fprintf(stderr, "%sCannot open white list file %s.\n", 
                LOG_IO_ERROR, file_name);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bc_len;
    while (gzgets(wl_file, buffer, BUFFER_SIZE) != 0) {
        bc_len = strlen(buffer) - 1;  // Excluding trailing '\n'
        char bc[bc_len + 1];
        strncpy(bc, buffer, bc_len);
        bc[bc_len] = '\0';
        barcode_t *barcode = init_barcode_with_code(bc);
        _add_barcode(wl, barcode);
    }
    gzclose(wl_file);

    wl->barcode_length = wl->barcodes[0]->length;
    wl->observed_count = 0;
    wl->dummy_observed_count = 0;

    qsort(wl->barcodes, wl->size, sizeof(barcode_t *), _compare_barcode);
    int dup_idx = _check_duplicates(wl);
    if (dup_idx >= 0) {
        fprintf(stderr, "%sDuplicate barcode \"%s\" is found.\n", 
                LOG_BARCODE_ERROR, wl->barcodes[dup_idx]->codes);
        exit(1);
    }
    return wl;
}

white_list_t *load_white_list_count(char const *file_name) {
    // int file_name_len = strlen(file_name);
    
    white_list_t *wl = malloc(sizeof(white_list_t));
    wl->file_name = file_name;
    // wl->file_name = malloc(file_name_len);
    // strcpy(wl->file_name, file_name);
    wl->size = 0;
    wl->barcodes = malloc(wl->size * sizeof(barcode_t *));

    gzFile wl_file = gzopen(file_name, "rb");
    if (!wl_file) {
        fprintf(stderr, "%sCannot open white list file %s.\n", 
                LOG_IO_ERROR, file_name);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bc_len;
    int count;
    char *token;
    while (gzgets(wl_file, buffer, BUFFER_SIZE) != 0) {
        token = strtok(buffer, "\t");
        bc_len = strlen(token);
        barcode_t *barcode = init_barcode_with_size(bc_len);
        barcode->length = bc_len;
        strcpy(barcode->codes, token);

        token = strtok(NULL, "\n");
        count = atoi(token);
        barcode->count = count;
        wl->observed_count += count;
        wl->dummy_observed_count += count + 1;

        _add_barcode(wl, barcode);
    }
    gzclose(wl_file);

    wl->barcode_length = wl->barcodes[0]->length;

    qsort(wl->barcodes, wl->size, sizeof(barcode_t *), _compare_barcode);
    int dup_idx = _check_duplicates(wl);
    if (dup_idx >= 0) {
        fprintf(stderr, "%sDuplicate barcode \"%s\" is found.\n", 
                LOG_BARCODE_ERROR, wl->barcodes[dup_idx]->codes);
        exit(1);
    }
    return wl;
}

void destroy_white_list(white_list_t *wl) {
    // free(wl->file_name);
    // wl->file_name = NULL;
    for (int i = 0; i < wl->size; i++) {
        destroy_barcode(wl->barcodes[i]);
    }
    free(wl->barcodes);
    wl->barcodes = NULL;
    free(wl);
    wl = NULL;

}

void write_white_list_count(white_list_t *wl, char const *file_name) {
    gzFile wl_out = gzopen(file_name, "wb");
    for (int i = 0; i < wl->size; i++) {
        gzprintf(wl_out, "%s\t%d\n", wl->barcodes[i]->codes, wl->barcodes[i]->count);
    }
    gzclose(wl_out);
}

void extract_barcode(read_t *read, int length) {
    free(read->barcode);
    read->barcode = NULL;
    free(read->barcode_qual);
    read->barcode_qual = NULL;

    read->barcode_length = length;
    read->barcode = malloc((length + 1) * sizeof(char));
    read->barcode_qual = malloc((length + 1) * sizeof(char));

    strncpy(read->barcode, read->seq, length);
    read->barcode[length] = '\0';
    strncpy(read->barcode_qual, read->qual, length);
    read->barcode_qual[length] = '\0';

    // char *tmp_copy = malloc((read->seq_length + 1) * sizeof(char));
    // strcpy(tmp_copy, read->seq);
    // read->seq_length -= length;
    // free(read->seq);
    // read->seq = NULL;
    // read->seq = malloc((read->seq_length + 1) * sizeof(char));
    // strncpy(read->seq, tmp_copy + length, read->seq_length);
    // read->seq[read->seq_length] = '\0';

    // strcpy(tmp_copy, read->qual);
    // read->qual_length -= length;
    // free(read->qual);
    // read->qual = NULL;
    // read->qual = malloc((read->qual_length + 1) * sizeof(char));
    // strncpy(read->qual, tmp_copy + length, read->qual_length);
    // read->qual[read->seq_length] = '\0';

    // free(tmp_copy);
    // tmp_copy = NULL;
}

void count_barcode(white_list_t *wl, fastq_t *fastq, int barcode_length) {
    read_t *r = init_read();
    barcode_t **found_barcode;
    barcode_t *tmp_barcode = init_barcode_with_size(barcode_length);
    while (get_read(fastq, r) >= 0) {
        extract_barcode(r, barcode_length);
        strcpy(tmp_barcode->codes, r->barcode);
        found_barcode = bsearch(&tmp_barcode, wl->barcodes, wl->size, sizeof(barcode_t *), _compare_barcode);
        if (found_barcode != NULL) {
            (*found_barcode)->count++;
            wl->observed_count++;
        }
    }
    wl->dummy_observed_count = wl->observed_count;
    destroy_barcode(tmp_barcode);
    destroy_read(r);
}

void calculate_barcode_freq(white_list_t *wl) {
    for (int i = 0; i < wl->size; i++) {
        wl->barcodes[i]->dummy_count = wl->barcodes[i]->count + 1;
        wl->dummy_observed_count += wl->barcodes[i]->dummy_count;
    }
    for (int i = 0; i < wl->size; i++) {
        wl->barcodes[i]->frequency = wl->barcodes[i]->dummy_count * 1.0 / wl->dummy_observed_count;
    }
}

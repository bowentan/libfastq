// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Jun. 01, 2018
// Lastest revised on Jun. 19, 2018
//
// This file is the source file of adapter processor.

#include "fastq/adapter.h"

// C libraries
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include <string.h>

// Project's other libraries
#include "fastq/log.h"

#define BUFFER_SIZE 8192

static int _adapter_compare(void const *a, void const *b) {
    return strcmp((*(adapter_t **)a)->read_id, (*(adapter_t **)b)->read_id);
}

static void _destroy_adapter(adapter_t *adapter) {
    free(adapter->read_id);
    adapter->read_id = NULL;
    free(adapter->adapter_id);
    adapter->adapter_id = NULL;
    free(adapter);
    adapter = NULL;
}

adapter_t *init_adapter() {
    adapter_t *adapter = malloc(sizeof(adapter_t));
    adapter->read_id = NULL;
    adapter->adapter_id = NULL;
    adapter->adapter_length = 0;
    adapter->adapter_start = 0;
    adapter->adapter_end = 0;
    adapter->align_length = 0;
    adapter->n_mismatch = 0;
    adapter->read_length = 0;
    adapter->read_start = 0;
    adapter->read_end = 0;
    return adapter;
}

void destroy_adapter(adapter_t *adapter) {
    free(adapter->read_id);
    adapter->read_id = NULL;
    free(adapter->adapter_id);
    adapter->adapter_id = NULL;
    free(adapter);
    adapter = NULL;
}

void destroy_adapter_list(adapter_list_t *list) {
    for (int i = 0; i < list->size; i++) {
        _destroy_adapter(list->adapters[i]);
    }
    free(list->adapters);
    list->adapters = NULL;
    free(list);
    list = NULL;
}

adapter_list_t *load_adapter(char const *file_name) {
    adapter_list_t *list = malloc(sizeof(adapter_list_t));
    list->file_name = file_name;
    list->size = 0;
    list->adapters = malloc(list->size * sizeof(adapter_t *));

    gzFile adapter_file = gzopen(file_name, "rb");
    if (!adapter_file) {
        fprintf(stderr, "%sCannot open adapter file %s.\n",
                LOG_IO_ERROR, file_name);
        exit(1);
    }
    char line[BUFFER_SIZE];
    char *token;
    gzgets(adapter_file, line, BUFFER_SIZE);  // Read header
    while (gzgets(adapter_file, line, BUFFER_SIZE) > 0) {
        list->size++;
        list->adapters = realloc(list->adapters, list->size * sizeof(adapter_t *));
        list->adapters[list->size - 1] = malloc(sizeof(adapter_t));
        adapter_t *adapter = list->adapters[list->size - 1];

        // Read id
        token = strtok(line, "\t ");
        adapter->read_id = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(adapter->read_id, token);
        
        // Read length
        token = strtok(NULL, "\t ");
        adapter->read_length = atoi(token);

        // Read start
        token = strtok(NULL, "\t ");
        adapter->read_start = atoi(token);

        // Read end
        token = strtok(NULL, "\t ");
        adapter->read_end = atoi(token);

        // Adapter id
        token = strtok(NULL, "\t ");
        adapter->adapter_id = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(adapter->adapter_id, token);

        // Adapter length
        token = strtok(NULL, "\t ");
        adapter->adapter_length = atoi(token);

        // Adapter start
        token = strtok(NULL, "\t ");
        adapter->adapter_start = atoi(token);

        // Adapter end
        token = strtok(NULL, "\t ");
        adapter->adapter_end = atoi(token);

        // Align length
        token = strtok(NULL, "\t ");
        adapter->align_length = atoi(token);

        // Mismatch
        token = strtok(NULL, "\t ");
        adapter->n_mismatch = atoi(token);
    }
    gzclose(adapter_file);

    qsort(list->adapters, list->size, sizeof(adapter_t *), _adapter_compare);
    return list;
}

adapter_t *find_adapter_by_read_id(adapter_list_t *list, char const *read_id) {
    adapter_t **found_adapter;
    adapter_t *temp_adapter = init_adapter();
    temp_adapter->read_id = malloc((strlen(read_id) + 1) * sizeof(char));
    strcpy(temp_adapter->read_id, read_id);

    found_adapter = (adapter_t **)bsearch(&temp_adapter, list->adapters, list->size, sizeof(adapter_t *), _adapter_compare);
    destroy_adapter(temp_adapter);
    if (found_adapter != NULL) {
        return *found_adapter;
    }
    return NULL;
}

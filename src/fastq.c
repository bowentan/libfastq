// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on Apr. 26, 2018
// Lastest revised on May. 3, 2018
//
// This file is the source file of reading fastq file.

// Self include
#include "fastq/fastq.h"

// C libraries
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Project's other libraries
#include "fastq/log.h"

static int _qual_compare(void const *a, void const *b) {
    return *(char *)a - *(char *)b;
}


fastq_t *fastq_open(char const *file_name, qual_sys_t const *qual_sys) {
    // int file_name_len = strlen(file_name);

    fastq_t *fastq = malloc(sizeof(fastq_t));
    fastq->file_name = file_name;
    // fastq->file_name_length = file_name_len;
    // fastq->file_name = malloc(file_name_len);
    // strcpy(fastq->file_name, file_name);
    fastq->file = gzopen(file_name, "rb");

    if (!fastq->file) {
        fprintf(stderr, "%sCannot open fastq file %s\n", 
                LOG_IO_ERROR, file_name);
        exit(1);
    }

    fastq->max_read_length = 0;
    fastq->qual_sys = qual_sys;

    return fastq;
}

void fastq_close(fastq_t *fastq) {
    // free(fastq->file_name);
    fastq->file_name = NULL;
    gzclose(fastq->file);
    free(fastq);
    fastq = NULL;
}

void fastq_seek(fastq_t *fastq, int nth_read) {
    read_t *r = init_read();
    for (int i = 0; i < nth_read - 1; i++) {
        get_read(fastq, r);
    }
    destroy_read(r);
}

void fastq_reload(fastq_t *fastq) {
    gzseek(fastq->file, 0, SEEK_SET);
}

void fastq_check(fastq_t *fastq, int verbose_level) {
    int n = 0;
    char min = 'i';
    char max = '!';
    read_t *r = init_read();
    while (n <= MAX_READ_CHECK && get_read(fastq, r) >= 0) {
        n++;
        char tmp_qual[r->seq_length + 1];
        strcpy(tmp_qual, r->qual);
        qsort(tmp_qual, r->seq_length, sizeof(char), _qual_compare);
        if (tmp_qual[0] < min) {
            min = tmp_qual[0];
        }
        if (tmp_qual[r->seq_length - 1] > max) {
            max = tmp_qual[r->seq_length - 1];
        }
        if (r->seq_length > fastq->max_read_length) {
            fastq->max_read_length = r->seq_length;
        }
    }
    destroy_read(r);
    
    // TODO (Bowen Tan): here I set the quality system to be more likely the one where
    // the corresponding quality characters represent higher quality values, although 
    // multiple quality systems are possible in this quality range.
    if (min < g_k_illumina_1_8->min_qual_char) {
        if (max < g_k_illumina_1_8->max_qual_char) {
            fastq->qual_sys = g_k_sanger;
        } else {
            fastq->qual_sys = g_k_illumina_1_8;
        }
    } else if (min < g_k_illumina_1_3->min_qual_char) {
        if (max > g_k_illumina_1_8->max_qual_char) {
            fastq->qual_sys = g_k_solexa;
        } else if (max > g_k_sanger->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_8;  // Prefer higher quality
        } else {
            fastq->qual_sys = g_k_sanger;  // Prefer higher quality
        }
    } else if (min < g_k_illumina_1_5->min_qual_char) {
        if (max > g_k_illumina_1_8->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_3;  // Prefer Illumina 1.3 to Solexa
        } else if (max > g_k_sanger->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_8;
        } else {
            fastq->qual_sys = g_k_sanger;
        }
    } else {
        if (max > g_k_illumina_1_3->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_5;
        } else if (max > g_k_illumina_1_8->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_3;  // Prefer Illumina 1.3 to Solexa and Illumina 1.5
        } else if (max > g_k_sanger->max_qual_char) {
            fastq->qual_sys = g_k_illumina_1_8;
        } else {
            fastq->qual_sys = g_k_sanger;
        }
    }

    fastq_reload(fastq);
    
    if (verbose_level > 0) {
        fprintf(stdout, "%sBy checking the first %d reads, the maximum read length is %d,"
                        "the mininum quality character is '%c' and "
                        "the maximum quality character is '%c'. Thus the quality system of the given "
                        "fastq(s) is inferred as %s. If incorrect, please specify in options with '%s'.\n",
                LOG_CHECK_INFO, n, fastq->max_read_length, min, max, fastq->qual_sys->name, "-s");
    }
}

void convert_qual_sys(read_t *read, qual_sys_t const *to_qual_sys) {
    // TODO (Bowen Tan): Difference between quality systems is measured by the difference
    // between minimum quality characters. The problem here is that ranges of all quality
    // systems are inconsistent, therefore I forced the value below the minimum quality
    // character of the target quality system to be the MINIMUM quality character after the
    // conversion. The value above the maximum quality character is processed similarly.
    // Also, note the Illumina 1.3+ system, its 0 and 1 are not used and 2 is a special code.
    // Please refer the official documentation of fastq format for detailed information.

    int qs_zero_diff = to_qual_sys->zero_qual_char - read->qual_sys->zero_qual_char;
    for (int i = 0; i < read->seq_length; i++) {
        char after_convert = read->qual[i] + qs_zero_diff;
        if (after_convert < to_qual_sys->min_qual_char) {
            after_convert = to_qual_sys->min_qual_char;
        }
        if (after_convert > to_qual_sys->max_qual_char) {
            after_convert = to_qual_sys->max_qual_char;
        }
        read->qual[i] = after_convert;
    }
}

/* Read */
read_t *init_read(void) {
    read_t *r = malloc(sizeof(read_t));
    r->id_length = 0;
    r->seq_length = 0;
    r->desc_length = 0;
    r->qual_length = 0;
    r->barcode_length = 0;

    r->id = NULL;
    r->seq = NULL;
    r->desc = NULL;
    r->qual = NULL;
    r->barcode = NULL;
    r->barcode_qual = NULL;

    r->qual_sys = NULL;
    return r;
}

read_t *make_read(char const *id, char const *seq, char const *desc,
                  char const *qual, char const *barcode, qual_sys_t const *qual_sys) {
    read_t* r = malloc(sizeof(read_t));
    r->id_length = strlen(id);
    r->seq_length = strlen(seq);
    r->desc_length = strlen(desc);
    r->qual_length = strlen(qual);

    if (r->seq_length != r->qual_length) {
        fprintf(stderr, "%sThe lengths of sequence and quality are inconsistent.\n",
                LOG_READ_ERROR);
    }

    r->id = malloc((r->id_length + 1) * sizeof(char));
    r->seq = malloc((r->seq_length + 1) * sizeof(char));
    r->desc = malloc((r->desc_length + 1) * sizeof(char));
    r->qual = malloc((r->qual_length + 1) * sizeof(char));

    strcpy(r->id, id);
    strcpy(r->seq, seq);
    strcpy(r->desc, desc);
    strcpy(r->qual, qual);

    if (barcode != NULL) {
        r->barcode_length = strlen(barcode);
        r->barcode = malloc((r->barcode_length + 1) * sizeof(char));
        strcpy(r->barcode, barcode);
    } else {
        r->barcode_length = 0;
        r->barcode = NULL;
    }

    r->qual_sys = qual_sys;
}

void copy_read(read_t const *src, read_t *dst) {
    clear_read(dst);

    dst->id_length = src->id_length;
    dst->seq_length = src->seq_length;
    dst->desc_length = src->desc_length;
    dst->qual_length = src->qual_length;

    dst->id = malloc((dst->id_length + 1) * sizeof(char));
    dst->seq = malloc((dst->seq_length + 1) * sizeof(char));
    dst->desc = malloc((dst->desc_length + 1) * sizeof(char));
    dst->qual = malloc((dst->qual_length + 1) * sizeof(char));

    strcpy(dst->id, src->id);
    strcpy(dst->seq, src->seq);
    strcpy(dst->desc, src->desc);
    strcpy(dst->qual, src->qual);

    if (src->barcode != NULL) {
        dst->barcode_length = src->barcode_length;
        dst->barcode = malloc((dst->barcode_length + 1) * sizeof(char));
        strcpy(dst->barcode, src->barcode);
    } else {
        dst->barcode_length = 0;
        dst->barcode = NULL;
    }

    dst->qual_sys = src->qual_sys;
}

int get_read(fastq_t *fastq, read_t *read) {
    clear_read(read);
    int buffer_size = 8196;
    char buffer[buffer_size];
    int size;

    gzgets(fastq->file, buffer, buffer_size);
    size = strlen(buffer) - 1;
    read->id_length = size;        // Exclude the trailing '\n'
    read->id = malloc(size + 1);
    strncpy(read->id, buffer, size);
    read->id[size] = '\0';

    gzgets(fastq->file, buffer, buffer_size);
    size = strlen(buffer) - 1;
    read->seq_length = size;
    read->seq = malloc(size + 1);
    strncpy(read->seq, buffer, size);
    read->seq[size] = '\0';

    gzgets(fastq->file, buffer, buffer_size);
    size = strlen(buffer) - 1;
    read->desc_length = size;
    read->desc = malloc(size + 1);
    strncpy(read->desc, buffer, size);
    read->desc[size] = '\0';

    gzgets(fastq->file, buffer, buffer_size);
    size = strlen(buffer) - 1;
    read->qual_length = size;
    read->qual = malloc(size + 1);
    strncpy(read->qual, buffer, size);
    read->qual[size] = '\0';

    read->qual_sys = fastq->qual_sys;

    if (read->seq_length != read->qual_length) {
        fprintf(stderr, "%sThe lengths of sequence and quality are inconsistent.\n",
                LOG_READ_ERROR);
        exit(1);
    }

    if (gzeof(fastq->file)) {
        return -1;
    }

    return 0;
}

void clear_read(read_t *read) {
    free(read->id);
    free(read->seq);
    free(read->desc);
    free(read->qual);
    free(read->barcode);
    free(read->barcode_qual);
    read->id = NULL;
    read->seq = NULL;
    read->desc = NULL;
    read->qual = NULL;
    read->barcode = NULL;
    read->barcode_qual = NULL;
    read->id_length = 0;
    read->seq_length = 0;
    read->desc_length = 0;
    read->qual_length = 0;
    read->barcode_length = 0;
}

void destroy_read(read_t *read) {
    clear_read(read);
    free(read);
    read = NULL;
}

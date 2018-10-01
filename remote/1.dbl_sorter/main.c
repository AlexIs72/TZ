#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>

// 50 Mb
#define BUF_SIZE			500000
//00 
// BUF_SIZE/8
#define NUM_PLAIN_ITEMS		BUF_SIZE/8	
//6250000

size_t page_size;
size_t block_size;
uint32_t num_items;


int usage(const char *prg, int exit_code) {
	printf("Usage: %s <input_file> <out_file>\n", prg);
	return exit_code;
}

uint32_t reorganize_input_data(int file_in, size_t size, int out);
//double* sort_data(double *buf_in, double *buf_out, size_t left, size_t right);
int sort_data(int input_data, int out_data, uint32_t total_count, uint32_t chunk_items);


int main(int argc, char *argv[]) {
    const char *tmp_file = "input_data_raw";
    const char *buf_file = "data_buf";
    uint32_t total_count = 0/*, portion, index*/;
    FILE *f_out;
    int fd_in, fd_tmp, fd_buf;
	int input_data, out_data;
    struct stat st;
    

	if(argc < 3) {
		return usage(argv[0], -1);
	}

    if (stat(argv[1], &st) == -1) {
        printf("File is not exists: %s\n", argv[1]);
        return usage(argv[0], -1);
    }

	page_size = sysconf(_SC_PAGE_SIZE);
	block_size = (BUF_SIZE/page_size)*page_size;
	num_items = (NUM_PLAIN_ITEMS/page_size)*page_size;


    fd_in = open(argv[1], O_RDWR);

    if(fd_in < 0) {
        printf("Unable to open input file (%s) for reading: %s\n", argv[1], strerror(errno));
        return usage(argv[0], -1);
    }

    fd_tmp = open(tmp_file, O_RDWR|O_CREAT|O_TRUNC);
    if(fd_tmp < 0) {
        printf("Unable to open output file (%s) for writing: %s\n", tmp_file, strerror(errno));
        return -1;
    }
    fchmod(fd_tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    fd_buf = open(buf_file, O_RDWR|O_CREAT|O_TRUNC);                                                            
    if(fd_buf < 0) {                                                                                            
        printf("Unable to open output file (%s) for writing: %s\n", buf_file, strerror(errno));                 
        return -1;                                                                                              
    }                                                                                                           
    fchmod(fd_buf, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    

    f_out = fopen(argv[2], "w");
    if(!f_out) {        
        printf("Unable to open output file (%s) for reading: %s\n", argv[2], strerror(errno));
        return usage(argv[0], -1);
    }

    printf("reorganize data ... \n");

    total_count = reorganize_input_data(fd_in, st.st_size, fd_tmp);
    
    printf("total = %u\n", total_count);

	if(total_count > 0) {
		input_data = fd_tmp;
		out_data = fd_buf;
		uint32_t chunk_items = /*NUM_PLAIN_ITEMS*/num_items;		

		while(1) {
			if(sort_data(input_data, out_data, total_count, chunk_items)) {
				int tmp = input_data;
				input_data = out_data;
				out_data = tmp;
				chunk_items *= 2;
			} else {
				break;
			}
		}
	}

#if 0    
    if(total_count > 0) {
        size_t i;
        size_t data_size = total_count * sizeof(double);
        double *buf_data = NULL;
        double *in_data = (double*)mmap(NULL, data_size, PROT_READ|PROT_WRITE,
                                        MAP_SHARED, fd_tmp, 0);
        if(in_data == MAP_FAILED) {
            printf("Unable to map input file: %s\n", strerror(errno));                                                     
            return -1;
        }

        buf_data = (double*)mmap(NULL, data_size, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, fd_buf, 0);

        if(buf_data == MAP_FAILED) {
            printf("Unable to map buf file: %s\n", strerror(errno));
            return -1;
        }

        ftruncate(fd_buf, data_size);
        lseek(fd_buf, SEEK_SET, 0);

        printf("sorting ...\n");
        sort_data(in_data, buf_data, 0, total_count);    
       
                                                                                                         
        printf("write result ...\n");
        for(i=0; i<total_count; i++) {                                                                                
            fprintf(f_out, "%1.15e\n", buf_data[i]);                                                                         
        }                                                                                                       


        munmap(in_data, data_size);
        munmap(buf_data, data_size);
    }
#endif
    close(fd_buf);
    close(fd_tmp);
    close(fd_in);
    fclose(f_out);    

//	unlink(tmp_file);
//	unlink(buf_file);

	return 0;
}
/*
void *map_region(int fd_in, off_t offset, size_t size) {
	void *in_data = (double*)mmap(NULL, size, PROT_READ,
                                        MAP_PRIVATE, fd_in, offset);

//printf("Try to map file with id: %d, offset: %lu, size: %lu. Reason: %s\n", fd_in, offset, size, strerror(errno));
    if(in_data == MAP_FAILED) {
    	printf("Unable to map file with id: %d, offset: %lu, size: %lu. Reason: %s\n", fd_in, offset, size, strerror(errno));
        return NULL;
    }

	return in_data;
}

void unmap_region(void *ptr, size_t size) {
	munmap(ptr, size);
}
*/

int compare(const void *a, const void *b) {
    const double *left, *right;

    left = (const double*)a;
    right = (const double*)b;

    if (*left < *right) {
        return -1;
    } else if (*left > *right) {
        return 1;
    }

    return 0;
}


uint32_t reorganize_input_data(int fd_in, size_t size, int fd_out) {
    double tmp_dbl;
	int num;
	uint32_t idx = 0;
    uint32_t total = 0;
//	uin32_t	num_items = 0;
	size_t chunk_size = 0;
//	size_t block_size;
	size_t offset = 0;
	int fix_pos = 0;
	size_t rest = size;
//	uint8_t *tmp_buf, 
	uint8_t *p;
	uint8_t *buf_in; // = calloc(BUF_SIZE+2, sizeof(uint8_t));
	double *buf_out; // = calloc(NUM_PLAIN_ITEMS+1, sizeof(double));
	
//	block_size = (BUF_SIZE/page_size)*page_size;

	buf_out = calloc(num_items+1, sizeof(double));
	buf_in = calloc(block_size+2+sizeof(double), sizeof(uint8_t));

	while(offset < size) {
		size_t chunk_rest = 0;

		chunk_size = (rest > block_size ? block_size : rest);
	
/*		tmp_buf = map_region(fileno(in), offset, chunk_size);
		if(!tmp_buf) {
			return 0;
		}

		p = memcpy(buf_in, tmp_buf, chunk_size);
*/
//printf("fix_pos = %d\n", -fix_pos);
		if(fix_pos != 0) {
			lseek(fd_in, -fix_pos, SEEK_CUR);
		}
		if(read(fd_in, buf_in, chunk_size) != (int)chunk_size) {
			printf("Unable to read input file: %s\n", strerror(errno));
			return 0;
		}

//		unmap_region(tmp_buf, chunk_size);	

//		idx = 0;
		p = buf_in;
		chunk_rest = chunk_size;
		fix_pos = 0;
		while(*p != 0 && *p != 0x0A) {
			int len = 0;
//			uint8_t *p2;
//printf("rest = %lu; p = %.25s\n", chunk_rest, p);
//			p = 
			tmp_dbl = 0;
//			p2 = memchr(p, 0x0A, chunk_rest);
//printf("p = %p; p2 = %p\n", p, p2);
			if(memchr(p, 0x0A, chunk_rest) == NULL) {
//printf("break: rest = %lu\n", chunk_rest);
				fix_pos = chunk_rest;
				break;
			}
			num = sscanf((char *)p, "%lf%n", &tmp_dbl, &len);	
//printf("num = %d; len = %d\n", num, len);
			if(num > 0) {
            	if(!isnan(tmp_dbl)) {
					buf_out[idx++] = tmp_dbl;
//                	write(out, &tmp_dbl, sizeof(double));
                	total++;
//printf("%d from %d\n", idx, NUM_PLAIN_ITEMS);
					if(idx >= num_items/*NUM_PLAIN_ITEMS*/) {
						qsort(buf_out, idx, sizeof(double), compare);
/*
{
	int i;
	for(i=0; i<idx; i++) {
printf("%1.15e\n", buf_out[i]);
	}
}
*/
						write(fd_out, buf_out, idx*sizeof(double));
						idx = 0;
					}
            	} else {
//puts("ups");
				}
				p += len+1;
				chunk_rest -= len+1;
        	} else {
				// find eol
				break;
			}
//			idx++;
//printf("num = %d, dbl = %1.15e\n", num, tmp_dbl);
//printf("[%06d]dbl = %1.15e\n", idx, tmp_dbl);

		}
//printf("---->\n");
		rest -= chunk_size;
		offset += chunk_size;
//		offset -= fix_pos;
	}


//    (void)sizeof(out);
/*
    while(!feof(in)) {
        if(fscanf(in, "%lfu", &tmp_dbl) > 0) {
            if(!isnan(tmp_dbl)) {
                write(out, &tmp_dbl, sizeof(double));
                total++;
            } 
        } 
    }
*/
	qsort(buf_out, idx, sizeof(double), compare);
/*
{
	int i;
	for(i=0; i<idx; i++) {
printf("%1.15e\n", buf_out[i]);
	}
}
*/
	write(fd_out, buf_out, idx*sizeof(double));


	free(buf_in);
	free(buf_out);

    lseek(fd_out, SEEK_SET, 0);    
    return total;
}

int read_buf(int fd, void *buf, off_t offset, size_t size) {
	lseek(fd, offset, SEEK_SET);
	return read(fd, buf, size);
}

int sort_data(int input_data, int out_data, uint32_t total_count, uint32_t chunk_items) {
//	size_t page_size = sysconf(_SC_PAGE_SIZE);
//	size_t block_size;
//	uint32_t num_items;
//	uint8_t *buf_in; // = calloc(BUF_SIZE+2, sizeof(uint8_t));
	uint32_t	part_items = num_items/2;
	uint32_t	part_size = part_items*sizeof(double);
	uint32_t 	left_offset = 0, right_offset = chunk_items*sizeof(double);
	uint32_t	left_idx = 0, right_idx = 0;
	uint32_t	right_rest = chunk_items*sizeof(double), left_rest = chunk_items*sizeof(double); //part_items*sizeof(double);
	uint32_t	target_idx = 0;
	int size;
    double *buf_in, *buf_out; //= calloc(NUM_PLAIN_ITEMS+1, sizeof(double));
	double *left_data, *right_data;
//	int num;
//	uint32_t left_idx, right_idx;

//	left_idx = right_idx = 0;

	lseek(out_data, 0, SEEK_SET);

    buf_in = calloc(num_items+2, sizeof(double));
    buf_out = calloc(num_items+2, sizeof(double));

printf("input_data = %d, out_data = %d, total_count = %u, chunk_items = %u, part_items = %u\n", 
			input_data, out_data, total_count, chunk_items, part_items);	

printf("0. left_offset = %u\n", left_offset);
printf("0. right_offset = %u\n", right_offset);
	left_data = buf_in;
	right_data = buf_in+part_items;
	size = read_buf(input_data, left_data, left_offset, part_size);
	left_rest -= size;
	left_offset += size;
	size = read_buf(input_data, right_data, right_offset, part_size);
	right_rest -= size;
	right_offset += size;

printf("1. left_offset = %u, left_rest = %u\n", left_offset, left_rest);
printf("1. right_offset = %u, right_rest = %u\n", right_offset, right_rest);

	while(1) {
		if(left_idx < part_items && right_idx < part_items) {
			if(left_data[left_idx] < right_data[right_idx]) {
				buf_out[target_idx++] = left_data[left_idx++];
			} else {
				buf_out[target_idx++] = right_data[right_idx++];
			}
		} else if(left_idx < part_items) {
			buf_out[target_idx++] = left_data[left_idx++];
		} else {
			buf_out[target_idx++] = right_data[right_idx++];
		}

		if(target_idx >= num_items) {
			write(out_data, buf_out, target_idx*sizeof(double));
			target_idx = 0;
		}

		if(left_idx >= part_items) {
			if(left_rest > 0) {
				size_t rest = (left_rest > part_size ? part_size : left_rest);
				size = read_buf(input_data, left_data, left_offset, rest);			
				left_rest -= size;
				left_offset += size;
			} else {
//				int i=0;
//				for(i=right_idx; i<part_items;i++) 
//				write()				
				// write tail
				memcpy(buf_out + target_idx, right_data + right_idx, (part_items - right_idx)*sizeof(double));
				target_idx += (part_items - right_idx);
				break;
			}
printf("2. left_offset = %u, left_rest = %u\n", left_offset, left_rest);
printf("2. target_idx = %u\n", target_idx);
		} else if(right_idx >= part_items) {
			if(right_rest > 0) {
				size_t rest = (right_rest > part_size ? part_size : right_rest);
				size = read_buf(input_data, right_data, right_offset, rest);			
				right_rest -= size;
				right_offset += size;
			} else {
				// write tail
				memcpy(buf_out + target_idx, left_data + left_idx, (part_items - left_idx)*sizeof(double));
				target_idx += (part_items - left_idx);
				break;
			}
printf("2. right_offset = %u, right_rest = %u\n", right_offset, right_rest);
printf("2. target_idx = %u\n", target_idx);
		}
	}

	write(out_data, buf_out, target_idx*sizeof(double));

	free(buf_in);
	free(buf_out);
printf("exit\n");
	if(chunk_items > total_count) {
		return 0;
	}

	return 0;
}

/*

https://ru.wikipedia.org/wiki/Сортировка_слиянием

*/
#if 0
double* sort_data(double *buf_in, double *buf_out, size_t left, size_t right) {
    size_t i, l_curr, r_curr;
    double *target = NULL;
    double *left_buf, *right_buf;
    size_t middle = (right+left)/2;

    if(left == right) {
        buf_out[left] = buf_in[left];
        return buf_out;
    }

    left_buf = sort_data(buf_in, buf_out, left, middle);
    right_buf = sort_data(buf_in, buf_out, middle+1, right);

    target = (left_buf == buf_in ? buf_out : buf_in);

    l_curr = left, r_curr = middle + 1;
    for (i = left; i <= right; i++) {
        if (l_curr <= middle && r_curr <= right) {
            if (left_buf[l_curr] < right_buf[r_curr]) {
                target[i] = left_buf[l_curr];
                l_curr++;
            } else {
                target[i] = right_buf[r_curr];
                r_curr++;
            }
        } else if (l_curr <= middle) {
            target[i] = left_buf[l_curr];
            l_curr++;
        } else {
            target[i] = right_buf[r_curr];
            r_curr++;
        }
    }

    return target;
}

#endif
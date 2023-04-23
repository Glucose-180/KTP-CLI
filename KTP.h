#ifndef __KTP_H__
#define __KTP_H__

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stdint.h>

#define SIZE 8	/* size of the board */
#define NDIR 8	/* number of directions */

#ifdef LINUX
#define PAUSE getchar()
#else
#define PAUSE system("pause")
#endif

typedef struct dir_t {
	short int dx;
	short int dy;
} dir_t;

typedef struct step_t {
	short int cur_x;			/* current location */
	short int cur_y;
	short int next[NDIR];		/* Next directions, which should be sorted */
	short int index_of_last;	/* How did the horse come to this location? */
} step_t;

extern const dir_t dir[NDIR];
extern step_t path[SIZE * SIZE];	/* path stack */

extern char debug_en;
extern char backstage_en;
extern const DWORD Cmd_buf_size;
extern char* pbuf_cmd;

int ktp_solve(const short int X0, const short int Y0);
void path_print(FILE* fp);
void display(const int I);

int smem_init(void);
int smem_cmd_write_str(const char* str);
int smem_dat_write_int(const int32_t* dat, const DWORD Size);
void smem_free(void);

#endif
#include "KTP.h"

char debug_en = 0;
/* run in backstage mode, get input and put output in shared memory */
char backstage_en = 0;

int main(int argc, char* argv[])
{
	int x0, y0;
	int rt = 0;
	FILE* fp;
	clock_t time_start;
	double time_used;
	int32_t path_buf[SIZE * SIZE];
	int i;

	fp = stdout;
	while (--argc)
	{
		++argv;
		if (strcmp(*argv, "-d") == 0)
			debug_en = 1;
		else if (strcmp(*argv, "-d2") == 0)
			debug_en = 2;
		else if (strcmp(*argv, "-b") == 0)
		{
			backstage_en = 1;
			debug_en = 0;
		}
		else if (strcmp(*argv, "-q") == 0)
			use_wdf = 1;
		else
		{
			fp = fopen(*argv, "w");
			if (fp == NULL)
			{
				fprintf(stderr, "**Error: cannot open file \'%s\'\n", *argv);
				fp = stdout;
			}
		}
	}
	
	if (backstage_en == 0)
	{
		while (printf("Enter location to start: ") && scanf("%d%d%*c", &x0, &y0) == 2)
		{
			time_start = clock();
			switch ((rt = ktp_solve(x0, y0)))
			{
			case 0:	/* done */
				path_print(fp);
				break;
			case -1:
				fprintf(stderr, "**Error: invalid input\n");
				break;
			case -2:
				fprintf(stderr, "**Error: calculation crashed\n");
				break;
			case 1:
				fprintf(stderr, "**Error: no solution\n");
				break;
			default:
				fprintf(stderr, "**Error: number is %d\n", rt);
				break;
			}
			time_used = (double)(clock() - time_start) / CLOCKS_PER_SEC;
			printf("Time used: %g s\n", time_used);
		}
		if (fp != stdout)
			fclose(fp);
	}
	else
	{
		if ((rt = smem_init()) != 0)
		{
			fprintf(stderr, "**Error: %d: failed to create shared memory\n", rt);
			PAUSE;
			return rt;
		}
		while (1)
		{
			printf("Waiting for input data...\n");
			while (*pbuf_cmd == '\0')
				Sleep(250);
			if (strncmp(pbuf_cmd, "Quit", 4) == 0)
			{
				smem_free();
				printf("Quit\n");
				return rt = 0;
			}
			else if (sscanf(pbuf_cmd, "%*s%d%d", &x0, &y0) != 2)
			{
				path_buf[0] = -2;
				smem_dat_write_int(path_buf, 1);
				fprintf(stderr, "**Error: invalid command: %s\n", pbuf_cmd);
				smem_cmd_write_str("Error");
			}
			else
			{
				printf("Start location: (%d,%d)\n", x0, y0);
				time_start = clock();
				rt = ktp_solve(x0, y0);
				time_used = (double)(clock() - time_start) / CLOCKS_PER_SEC;
				printf("Time used: %g s\n", time_used);
				if (rt == 0)
				{
					for (i = 0; i < SIZE * SIZE; ++i)
						path_buf[i] = path[i].cur_x * 100 + path[i].cur_y;
					/* format: xxyy in decimal */
					smem_dat_write_int(path_buf, SIZE * SIZE);
					smem_cmd_write_str("Done");
					printf("Done\n");
				}
				else
				{
					path_buf[0] = rt;
					smem_dat_write_int(path_buf, 1);
					smem_cmd_write_str("Error");
					fprintf(stderr, "**Error: number is %d\n", rt);
				}
			}
			Sleep(3000);	/* The command will be kept 3 seconds */
			*pbuf_cmd = '\0';
		}
	}
	return rt;
}
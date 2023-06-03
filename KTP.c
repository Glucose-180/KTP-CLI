#include "KTP.h"

const dir_t dir[NDIR] = { {1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2} };
step_t path[SIZE * SIZE];	/* path stack */
static char board[SIZE][SIZE];
static int tsp;				/* stack pointer */
int use_wdf = 0;	/* Warnsdorff's Algorithm or normally greedy */

static inline int is_reachable(const short int X, const short int Y)
{
	return (X >= 0 && X < SIZE&& Y >= 0 && Y < SIZE&& board[X][Y] == 0);
}

int ktp_solve(const short int X0, const short int Y0)
{
	short int x, y;	/* current location */
	int i;
	void dir_sort(short int* dir, const short int X, const short int Y);

	/* initialize */
	{
		memset(board, 0, SIZE * SIZE);	/* clear board */
		tsp = 0;	/* clear stack */
		if (is_reachable(x = X0, y = Y0) != 0)
			board[X0][Y0] = 1;
		else
			return -1;	/* invalid input */
	}
	/* push (X0, Y0) into stack as the first step */
	{
		path[tsp].cur_x = X0;
		path[tsp].cur_y = Y0;
		dir_sort(path[tsp].next, X0, Y0);
		path[tsp].index_of_last = NDIR - 1;
		++tsp;
	}
	i = 0;
	while (1)
	{
		if (i < NDIR && (path[tsp - 1].next[i] < NDIR || path[tsp - 1].next[i] < 10 + NDIR && tsp == SIZE * SIZE - 1))
		{
			/* push */
			path[tsp].cur_x = (x += dir[path[tsp - 1].next[i] % 10].dx);
			path[tsp].cur_y = (y += dir[path[tsp - 1].next[i] % 10].dy);
			dir_sort(path[tsp].next, x, y);
			path[tsp].index_of_last = i;
			++tsp;
			board[x][y] = 1;

			if (debug_en != 0)
				display(0);

			if (tsp >= SIZE * SIZE)
				break;	/* Done! */
			else
				i = 0;
		}
		else
		{
			if (tsp > 1)
			{	/* backtrace: pop */
				--tsp;
				i = path[tsp].index_of_last + 1;
				board[x][y] = 0;
				x = path[tsp - 1].cur_x;
				y = path[tsp - 1].cur_y;

				if (debug_en != 0)
					display(i);
			}
			else
			{
				break;	/* Ouch! No solution. */
			}
		}
	}
	if (tsp >= SIZE * SIZE)
		return 0;
	else
		return 1;	/* No solution */
}

void path_print(FILE* fp)
{
	short int i;

	for (i = 0; i < SIZE * SIZE; ++i)
		fprintf(fp, "%hd\t%hd\t%hd\n", i, path[i].cur_x, path[i].cur_y);
	fflush(fp);
}

void dir_sort(short int* next_dir, const short int X, const short int Y)
{
	short int i, j, g, x, y, temp;
	short int count_of_reachable[NDIR] = { 0 };

	for (i = 0; i < NDIR; ++i)
		next_dir[i] = i;
	for (i = 0; i < NDIR; ++i)
	{
		/* calculate the data of the point on the i-th direction of (X, Y) */
		x = X + dir[i].dx;
		y = Y + dir[i].dy;
		if (!is_reachable(x, y))
			count_of_reachable[i] = -1;
		else
			for (j = 0; j < NDIR; ++j)
			{
				if (is_reachable(x + dir[j].dx, y + dir[j].dy))
					++count_of_reachable[i];
			}
	}

	/* do shellsort */
	if (use_wdf != 0)
	{	/* Warnsdorff's Algorithm */
		for (g = NDIR / 2; g > 0; g /= 2)
			for (i = g; i < NDIR; ++i)
				for (j = i - g; j >= 0 && !(
					(count_of_reachable[next_dir[j]] > 0 &&
						count_of_reachable[next_dir[j]] <= count_of_reachable[next_dir[j + g]])
					||
					count_of_reachable[next_dir[j + g]] <= 0 &&
					count_of_reachable[next_dir[j]] > count_of_reachable[next_dir[j + g]]);
					j -= g)
				{
					temp = next_dir[j];
					next_dir[j] = next_dir[j + g];
					next_dir[j + g] = temp;
				}
	}
	else
	{	/* Normally greedy */
		for (g = NDIR / 2; g > 0; g /= 2)
			for (i = g; i < NDIR; ++i)
				for (j = i - g; j >= 0 && count_of_reachable[next_dir[j]] < count_of_reachable[next_dir[j + g]]; j -= g)
				{
					temp = next_dir[j];
					next_dir[j] = next_dir[j + g];
					next_dir[j + g] = temp;
				}
	}

	/* set the decade of next_dir[i] according to its count_of_reachable value */
	for (i = 0; i < NDIR; ++i)
	{
		if (count_of_reachable[next_dir[i]] < 0)
			next_dir[i] += 20;
		else if (count_of_reachable[next_dir[i]] == 0)
			next_dir[i] += 10;
	}
}

/* For debug */
void display(const int I)
{
	short int i, j;
	char c;

	printf("-------\n$tsp = %d\t$i = %d\n\n", tsp, I);
	/* display board */
	for (i = 0; i < SIZE; ++i)
	{
		for (j = 0; j < SIZE; ++j)
		{
			if (i == path[tsp - 1].cur_x && j == path[tsp - 1].cur_y)
				putchar('@');
			else
				putchar(board[i][j] + '0');
			putchar(' ');
		}
		putchar('\n');
	}
	printf("\n$path[tsp-1] = {\n\tcur_x: %hd;\n\tcur_y: %hd;\n\tindex_of_last: %hd;\n\tnext:", path[tsp - 1].cur_x, path[tsp - 1].cur_y, path[tsp - 1].index_of_last);
	for (j = 0; j < NDIR; ++j)
	{
		printf(" %hd", path[tsp - 1].next[j]);
	}
	printf(";\n}\n<Enter>: next step \'c\': continue\n");
	if ((c = getchar()) == 'c' || c == 'C')
		debug_en = 0;
}
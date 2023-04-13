/* smem.c: used for shared memory */
#include "KTP.h"

/* Size of command buffer */
const DWORD Cmd_buf_size = 64;
/* Size of data buffer */
const DWORD Dat_buf_size = SIZE * SIZE * sizeof(int);

#ifdef _CRT_SECURE_NO_WARNINGS
LPCWSTR Smem_name_cmd = L"ktp_cmd_g";
LPCWSTR Smem_name_dat = L"ktp_dat_g";
#else
LPCSTR Smem_name_cmd = "ktp_cmd_g";
LPCSTR Smem_name_dat = "ktp_dat_g";
#endif

static HANDLE hMapFile_cmd, hMapFile_dat;
char* pbuf_cmd;
static int* pbuf_dat;

/* initialize shared memory */
int smem_init()
{
	hMapFile_cmd = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		Cmd_buf_size,
		Smem_name_cmd
	);
	hMapFile_dat = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		Dat_buf_size,
		Smem_name_dat
	);
	if (hMapFile_cmd == NULL || hMapFile_dat == NULL)
		return 1;	/* failed */
	pbuf_cmd = (char *)MapViewOfFile(
		hMapFile_cmd,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		Cmd_buf_size
	);
	pbuf_dat = (int *)MapViewOfFile(
		hMapFile_dat,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		Dat_buf_size
	);
	if (pbuf_cmd == NULL || pbuf_dat == NULL)
		return 2;
	memset(pbuf_cmd, 0, Cmd_buf_size);
	return 0;	/* succeeded */
}

int smem_cmd_write_str(const char* str)
{
	if (pbuf_cmd == NULL)
		return 1;
	else
	{
		strncpy(pbuf_cmd, str, Cmd_buf_size);
		return 0;
	}
}

int smem_dat_write_int(const int* dat, const DWORD Size)
{
	DWORD i;

	if (pbuf_dat == NULL)
		return 1;
	else
	{
		if (Size > Dat_buf_size)
			return 2;
		else
		{
			for (i = 0; i < Size; ++i)
				pbuf_dat[i] = dat[i];
			return 0;
		}
	}
}

void smem_free()
{
	if (pbuf_cmd != NULL)
		UnmapViewOfFile(pbuf_cmd);
	if (pbuf_dat != NULL)
		UnmapViewOfFile(pbuf_dat);
	if (hMapFile_cmd != NULL)
		CloseHandle(hMapFile_cmd);
	if (hMapFile_dat != NULL)
		CloseHandle(hMapFile_dat);
}
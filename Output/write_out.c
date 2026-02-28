#include <stdio.h>

void write_str(char w_buffer[])
{
    FILE* fp = fopen("output.txt", "w");
	if (fp == NULL)
	{
		printf("Error - open output.txt\n");
		fclose(fp);
	}

	fputs(w_buffer, fp);

	fclose(fp);
}
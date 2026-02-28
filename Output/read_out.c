#include <stdio.h>

void read_str()
{
    FILE* in = fopen("output.txt", "r");
	if (in == NULL)
	{
		printf("Error - open output.txt\n");
		fclose(in);
	}

	fseek(in, 0,  SEEK_END);
	long length = ftell(in);

	for (long i = 1; i <= length; i++)
	{
		fseek(in, -i, SEEK_END);
		putchar(fgetc(in));
	}

    printf("\n\n");
	fclose(in);
}
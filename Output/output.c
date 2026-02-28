#include <stdio.h>
#include "interface.h"

int main()
{
	char w_buffer[] = "String from file";

	write_str(w_buffer);

	read_str();

	return 0;
}
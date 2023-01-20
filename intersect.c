#include <stdio.h>
#include <stdbool.h>

enum return_codes {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3
};



int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s [FILE]...\n", argv[0]);
		return(INVOCATION_ERROR);
	} 

	// Validates that given files are able to be opened; sourced from ws.c
	bool close_flag = false;
	for (int i = 1; i < argc; ++i) {
		FILE *fo = fopen(argv[i], "r");
		if (!fo) {	// If file could not be opened for reading
			close_flag = true;	// Close after all files checked
			fprintf(stderr, "%s could not be opened",
				argv[i]);
			perror(" \b");	// Backspace to format perror string
		} else {
			fclose(fo);
		}
	}
	if (close_flag == true) {
		return (INVOCATION_ERROR);
	}

	return(SUCCESS);
}

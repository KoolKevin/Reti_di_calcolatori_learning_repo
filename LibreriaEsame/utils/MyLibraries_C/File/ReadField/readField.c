#include"readField.h"


int readField(char buffer[], char sep, FILE* f) {
	int i = 0;
	char ch = fgetc(f);

	while (ch != sep && ch != 10 && ch != EOF) {
		buffer[i] = ch;
		i++;
		ch = fgetc(f);
	}

	/*if (ch == EOF)
		return 0;*/

	buffer[i] = '\0';

	return ch;
}
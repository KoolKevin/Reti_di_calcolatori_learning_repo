#include <stdio.h>

main()
{
	//test con getchar()/putchar()
	//int c;
	//while((c = getchar()) != EOF)
	//	putchar(c);

	//test con gets()/puts()
	//char s[11];
	//char s2[11];
	//gets(s2);
	//while(gets(s))
	//	puts(s);
	//printf("s:%s\ts2:%s", s, s2);


	//test di consumo dello stream in caso di errore con scanf()
	int num1, ok;
	char c;
	while ((ok = scanf("%d", &num1)) != EOF /* finefile */)
	{
		printf("numero inserito: %i\n", num1);
		if (ok != 1) { // errore di formato
			do { c = getchar(); printf("%c ", c); } while (c != '\n');
		}
		printf("Inserisci valore intero, EOF per terminare: ");
	}
}
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TIPO int	//cambia questo se vuoi ordinare array di altri tipi

//utility
int compare(TIPO s1, TIPO s2);
void stampaVettore(TIPO v[], int dim);
void scambia(TIPO* a, TIPO* b);



//naiveSort
int trovaPosMax(TIPO v[], int n);
void naiveSort(TIPO v[], int dim);

//bubbleSort
void bubbleSort(TIPO v[], int dim);

//insertSort
void insOrd(TIPO v[], int pos);
void insertSort(TIPO v[], int dim);

//mergeSort
void merge(TIPO v[], int i1, int i2, int fine, TIPO vout[]);
void mergeSort(TIPO v[], int first, int last, TIPO vout[]);

//quickSort
void quickSortR(TIPO a[], int iniz, int fine);
void quickSort(TIPO a[], int dim);

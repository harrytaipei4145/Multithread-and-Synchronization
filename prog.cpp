#include "bmpReader.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <process.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight;
int FILTER_SIZE;
int *filter_arr;
int *filter_gx, *filter_gy;
char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};
char *outputSharp_name[5] = {
	"Sharp1.bmp",
	"Sharp2.bmp",
	"Sharp3.bmp",
	"Sharp4.bmp",
	"Sharp5.bmp"
};

unsigned char *pic_sobel, *pic_in, *pic_gx, *pic_gy, *pic_final;

unsigned char gx_sobelFilter(int w, int h, int color)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>imgWidth || b>imgHeight) continue;

		tmp += filter_gx[j*ws + i] * pic_in[3 * (b*imgWidth + a) + color];
	};
	
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	
	return (unsigned char)tmp;

}

unsigned char gy_sobelFilter(int w, int h, int color)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>imgWidth || b>imgHeight) continue;

		tmp += filter_gy[j*ws + i] * pic_in[3 * (b*imgWidth + a) + color];
	};
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;

}

unsigned char sobelFilter(int w, int h, int color)
{
	int tmp = 0;
	int k = 5;
	tmp = pic_gx[3 * (h*imgWidth + w) + color] + pic_gy[3 * (h*imgWidth + w) + color];
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char finalImage(int w, int h, int color)
{
	int tmp = 0;
	int k = 5;
	tmp = pic_sobel[3 * (h*imgWidth + w) + color] + pic_in[3 * (h*imgWidth + w) + color];
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}


HANDLE full, empty;

void * gx_sobel1(void *arg) {
	for (int j = 0; j<imgHeight; j++) {
		WaitForSingleObject(empty, INFINITE);
		for (int i = 0; i<imgWidth; i++){

			
			pic_gx[3 * (j*imgWidth + i) + MYRED] = gx_sobelFilter(i, j, MYRED);
			
		
		}
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}
void * gx_sobel2(void *arg) {
	for (int j = 0; j<imgHeight; j++) {
		WaitForSingleObject(empty, INFINITE);
		for (int i = 0; i<imgWidth; i++){
		
			pic_gx[3 * (j*imgWidth + i) + MYGREEN] = gx_sobelFilter(i, j, MYGREEN);
			
		}
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}
void * gx_sobel3(void *arg) {
	for (int j = 0; j<imgHeight; j++) {
		WaitForSingleObject(empty, INFINITE);
		for (int i = 0; i<imgWidth; i++){
			
			
			pic_gx[3 * (j*imgWidth + i) + MYBLUE] = gx_sobelFilter(i, j, MYBLUE);
		
		}
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}
void * gy_sobel1(void *arg) {
	for (int j = 0; j < imgHeight; j++){
		WaitForSingleObject(full, INFINITE);
		for (int i = 0; i < imgWidth; i++){
			
			pic_gy[3 * (j*imgWidth + i) + MYRED] = gy_sobelFilter(i, j, MYRED);
			
		}
		ReleaseSemaphore(empty, 1, NULL);
	};
	return 0;
}
void * gy_sobel2(void *arg) {
	for (int j = 0; j < imgHeight; j++){
		WaitForSingleObject(full, INFINITE);
		for (int i = 0; i < imgWidth; i++){

			pic_gy[3 * (j*imgWidth + i) + MYGREEN] = gy_sobelFilter(i, j, MYGREEN);
	
		}
		ReleaseSemaphore(empty, 1, NULL);
	}
	return 0;
}
void * gy_sobel3(void *arg) {
	for (int j = 0; j < imgHeight; j++){
		WaitForSingleObject(full, INFINITE);
		for (int i = 0; i < imgWidth; i++){

		
			pic_gy[3 * (j*imgWidth + i) + MYBLUE] = gy_sobelFilter(i, j, MYBLUE);
			
		
		}
		ReleaseSemaphore(empty, 1, NULL);
	}
	return 0;
}


void * sobel(void *arg) {
	for (int j = 0; j < imgHeight; j++){
		WaitForSingleObject(empty, INFINITE);
	
		for (int i = 0; i < imgWidth; i++){
			
			pic_sobel[3 * (j*imgWidth + i) + MYBLUE] = sobelFilter(i, j, MYBLUE);
			pic_sobel[3 * (j*imgWidth + i) + MYGREEN] = sobelFilter(i, j, MYGREEN);
			pic_sobel[3 * (j*imgWidth + i) + MYRED] = sobelFilter(i, j, MYRED);
			
		}
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}



void * final(void *arg) {
	for (int j = 0; j < imgHeight; j++){
		
		WaitForSingleObject(full, INFINITE);
		for (int i = 0; i < imgWidth; i++){
			
			pic_final[3 * (j*imgWidth + i) + MYRED] = finalImage(i, j, MYRED);
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = finalImage(i, j, MYGREEN);
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = finalImage(i, j, MYBLUE);
			

		}
		ReleaseSemaphore(empty, 1, NULL);
	}
	return 0;
}




int main()
{
	BmpReader* bmpReader = new BmpReader();
	printf("Processing the image...\nIt takes a long time. Please wait until 'Time elapse' printed out.\n");

	// timing parameter
	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER start_tick;
	LARGE_INTEGER end_tick;
	double elapsed;

	int *tid_arg;
	tid_arg = (int*)malloc(sizeof(int));
	*tid_arg = 1;
	DWORD tid;
	// read mask file
	FILE* mask;
	mask = fopen("mask.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	
	filter_gx = new int[FILTER_SIZE];
	filter_gy = new int[FILTER_SIZE];
	
	for (int i = 0; i < FILTER_SIZE; i++)

     fscanf(mask, "%d", &filter_gx[i]);
	 
		

	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gy[i]);

	

	// timing function 1
	QueryPerformanceFrequency(&ticksPerSecond);
	QueryPerformanceCounter(&start_tick);

	
	for (int k = 0; k < 5; k++)
	{
		
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_sobel = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		pic_gx = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		pic_gy = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		/* write your code here, between timing function 1&2 */
		
		full = CreateSemaphore(NULL, 0,10, NULL);
		empty = CreateSemaphore(NULL, 10, 10, NULL);
		//apply the gx_sobel filter to the image 
		HANDLE Thread[6];
		Thread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gx_sobel1, (void*)tid_arg, 0, &tid);
		Thread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gx_sobel2, (void*)tid_arg, 0, &tid);
		Thread[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gx_sobel3, (void*)tid_arg, 0, &tid);
		Thread[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gy_sobel1, (void*)tid_arg, 0, &tid);
		Thread[4] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gy_sobel2, (void*)tid_arg, 0, &tid);
		Thread[5] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gy_sobel3, (void*)tid_arg, 0, &tid);
		WaitForMultipleObjects(6, Thread, TRUE, INFINITE);
		Thread[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sobel, (void*)tid_arg, 0, &tid);
		Thread[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)final, (void*)tid_arg, 0, &tid);
		WaitForMultipleObjects(2, Thread, TRUE, INFINITE);
		//apply the gy_sobel filter to the image 
		

		//apply the sobel filter to the image 
		

		//add the result of sobel filter on the original image 
		



		// write output BMP file
		bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_sobel);
		bmpReader->WriteBMP(outputSharp_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_sobel);
		free(pic_final);
		free(pic_gx);
		free(pic_gy);

	}

	// timing function 2
	QueryPerformanceCounter(&end_tick);
	elapsed = ((double)(end_tick.QuadPart - start_tick.QuadPart) / ticksPerSecond.QuadPart);


	printf("Time elapse = %lf\n", elapsed);

	system("pause");
	return 0;
}



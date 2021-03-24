#include <stdio.h>
#include <math.h>
#include <assert.h>

#define N 1024


int main(int argc, char const *argv[])
{
	/*  High Pass Filter based on RBJ Cookbook linked above
		Analog Transfer Function for this filter: H(s) = s^2 / (s^2 + s/Q + 1) */

	// These floating point values are used by the filter code below
	float Fs = 44000.0f;      	// sample rate in samples per second
	float Pi = 3.141592f;   	// the value of Pi

	// These floating point values implement the specific filter type
	float f0 = 500;                // cut-off (or center) frequency in Hz
	float Q = 1.5;                 // filter Q
	float w0 = 2 * M_PI * f0 / Fs;
	float alpha = sin(w0) / (2 * Q);
	float a0 = 1 + alpha;
	float a1 = -2 * cos(w0);
	float a2 = 1 - alpha;
	float b0 = (1 + cos(w0)) / 2;
	float b1 = -(1 + cos(w0));
	float b2 = (1 + cos(w0)) / 2;

	// The Buffer[] array holds the incoming samples, PrevSample[] holds intermediate results
	float Buffer[N] = {0};         		// this array holds 1024 elements numbered 0 through 1023
	float PrevSample[3] = {0};        	// this array holds 3 elements numbered 0 through 2

	/* These integer (whole number) variables are used below to process 1,024 iterations at a time*/
	int I = 0;
	FILE *fin = fopen("demo.raw", "rb");
	FILE *fout = fopen("demo_out.raw", "wb");

	if (fin && fout) {
		while(fread(Buffer,sizeof(float),N,fin) == N){
			/*TO DO: 1. check if the main buffer Buffer[I] contains less than the full 1024 samples /the last time it runs/
				The code below executes repeatedly as long as the value of I is less than N
				Since I was initialized to 0 above, and N was set to 1024, this code executes 1,024 times */

			I = 0;

			while (I < N) {     					// this is the beginning of the code that loops 1,024 times
				PrevSample[2] = PrevSample[1];   	// Slide the samples over one position
				PrevSample[1] = PrevSample[0];
				PrevSample[0] = Buffer[I];
				
				if ((Buffer[I] > 1.0f) || (Buffer[I] < -1.0f) || (Buffer[I] == 1.0f))
					printf("%d: %f\n", I, Buffer[I]);
				//printf("%f\n", a0+a1+a2+b0+b1+b2);

				// TO DO:/DONE?/ 2. avoid the error when the code first run when I counter has value less than 1 and 2
#if 1
				if (I < 2){
					Buffer[I] = (b0 / a0 * PrevSample[0]) +
								(b1 / a0 * PrevSample[1]) +
								(b2 / a0 * PrevSample[2]) -
								(a1 / a0 * PrevSample[1]) -
								(a2 / a0 * PrevSample[2]);
				}
				else {
					// What is the meaning of the formula ???
					Buffer[I] = (b0 / a0 * PrevSample[0]) +
								(b1 / a0 * PrevSample[1]) +
								(b2 / a0 * PrevSample[2]) -
								(a1 / a0 * Buffer[I-1]) -
								(a2 / a0 * Buffer[I-2]);
				}
#else
		Buffer[I] = Buffer[I]*0.2;
#endif
				I++;			//  increment the counter I by adding 1
			}					//	this is the end of the code loop
//assert(0); 
			fwrite(Buffer, sizeof(float), N, fout);
		}

		fclose(fin);
		fclose(fout);
	}
	
	return 0;
}


/* NOTE: 1. Another simplification is omitting code to check if the main buffer contains less than the full 1,024 samples,
 		 which is likely the last time it runs. 
 		 2. Additional code is also needed to avoid an error when the code first runs. 
 		 When the while counter "I" has a value less than 1 or 2, the statements Buffer[I-1] and Buffer[I-2]
 		 reference elements in the array that don't exist */

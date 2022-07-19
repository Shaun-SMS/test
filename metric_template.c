// Hull monitoring algorithm program

// added a new sms logo
// add another image
// add a new line

// test Git GitHub branch

// Libraries to include
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <string.h>
	#include <time.h>
	#include <stdbool.h>

// Defining Constants
	#define TOTAL_SAMPLES_INT 100
	#define SAMPLE_FREQ 10					// 10 Hz sampling frequency, time interval 0.1s
	#define LOWER_FREQ 0						// 0 kHz
	#define UPPER_FREQ 600e3				// 600 kHz

// Declaring Variables

// Functions
	void main();
	void load_data();
	void metric_print(float);

	// Variables
	FILE *fftfile_in;
	FILE *fileout;

	int loop_idx,i;

	float LBSG_y[TOTAL_SAMPLES_INT], time_x[TOTAL_SAMPLES_INT];

	char path[250],pathinput[250],filename_structure[250],fftfilename_in[200];
	char pathin[250],pathout[250];
  char metricnamesfilename[250],fileoutfilename[250];
  char datainchar[20];

void main(int argc, char* argv[])
{
  // Let's track the time:
  float elapsed_time_start = clock();
	// Arguments to Main:
																					//  0: Program name
	sprintf(pathinput,"%s",argv[1]);				//  1: Path
	int s   = atoi(argv[2]);								//  2: Stage
	int d   = atoi(argv[3]);								//  3: Detachment
	int rs  = atoi(argv[4]);								//  4: Repetition Start
	int re  = atoi(argv[5]);								//  5: Repetition End
	int qs  = atoi(argv[6]);								//  6: Flow Rate Start
	int qe  = atoi(argv[7]);								//  7: Flow Rate End
  int vs  = atoi(argv[8]);								//  8: Variable Reference Start
	int ve  = atoi(argv[9]);								//  9: Variable Reference End
  int ts	= atoi(argv[10]);								// 10: Transducer Start
  int te	= atoi(argv[11]);								// 11: Transducer End
	int ce  = atoi(argv[12]);								// 12: Chunk End
	int pid = atoi(argv[13]);								// 13: Process ID

	int analysis_start = 1;
  float average_time_sum = 0;
  float average_time;
  int analysis = analysis_start;
  int relative_analysis = 1;
  int number_of_analyses = ((ve+1)-vs)*1*((re+1)-rs)*((te+1)-ts)*((qe+1)-qs)*(ce+1);

	for(int t=ts;t<te+1;t++){
		sprintf(pathout,"%s\\Metrics",pathinput);
		sprintf(fileoutfilename,"%s\\Metrics_T%d_%d.dat",pathout,t,pid);
		fileout = fopen(fileoutfilename,"a+");

			for(int v=vs;v<ve+1;v++){
        for(int r=rs;r<re+1;r++){
					for(int q=qs;q<qe+1;q=q+10){
						for(int c=0;c<ce+1;c++){
	      			float analysis_time_start = clock();
	      			printf("\nAnalysis %d of %d: V%d Q%d T%d R%d Ch%d\n",analysis,number_of_analyses,v,q,t,r,c);
							float metrics_time_start = clock();

							load_data(s,d,t,v,r,analysis,q,c);
							fprintf(fileout,"\n");

							float metrics_time_end = clock();
		          float analysis_time_end = clock();
		          printf("Analysis Time: %0.2fs\tElapsed Time: %0.2fs\n",(analysis_time_end-analysis_time_start)/CLK_TCK,(clock()-elapsed_time_start)/CLK_TCK);

		    			if(analysis_start==1)
		    			{
		    				average_time_sum += (analysis_time_end-analysis_time_start)/CLK_TCK;
		        		average_time = average_time_sum/analysis;
		        		float etr = (number_of_analyses-analysis)*average_time;
		        		printf("Average Analysis Time: %0.2f s\tETR: %0.2f seconds\n",average_time,etr);
		    			}

							analysis++;
							printf("------------------------------");
						} // <\Chunk>
					}	// <\Transducer>
				}	// <\Flow Rate>
			}	// <\Repetition>
	  }	// <\Var Ref>
		fclose(fileout);

	float elapsed_time = clock()-elapsed_time_start;
    printf("\n  Total Execution Time: %0.2f seconds\n",(elapsed_time)/CLK_TCK);
    printf("Average Execution Time: %0.2f seconds\n",(elapsed_time/CLK_TCK)/ve);
}
//  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
//  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //


///////////////////////////////////////////////////////////////////////////////
//  | Function: Load Data
///////////////////////////////////////////////////////////////////////////////
//  |     Desc: This function will load the relevant data into memory
///////////////////////////////////////////////////////////////////////////////
void load_data(int s, int d, int t, int v, int r, int analysis, int q, int c)
{
	// Let's create the filenames and open the files:
	sprintf(filename_structure,"Data_2MHz_20ms_S%d_D%d_V%d_Q%d_R%d_T%d",s,d,v,q,r,t);   // Filename Structure for data recorded via sms2m.m
  sprintf(fftfilename_in,"%s\\%s_FfssnCh%d_c_fwp.dat",pathinput,filename_structure,c);
  fftfile_in = fopen(fftfilename_in,"r");
  char output_linestart[100];
  sprintf(output_linestart,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",analysis,s,d,v,q,r,t,c);
	fprintf(fileout,"%s",output_linestart);
	printf("Opening: %s\n",fftfilename_in);

	// Let's load the FFT Data file into memory:
	float section_start_time = clock(); // Start time for Load function.

	// <LOAD FREQ DATA LOOP>
		//Let's set up and initialize some metric parameters
		float magnitude_sum = 0; 						// METRIC LINE: M01 SOM
		float peak_frequency = 0;						// METRIC LINE: M02 WPF, M03 PCR, M13 PF
		float max_magnitude = 0;						// METRIC LINE: M04 PAR, M09 PM
		float peak_freq_1 = 0;							// METRIC LINE: M05 PFR
		float peak_freq_2 = 0;							// METRIC LINE: M05 PFR
		float max_mag_1 = 0;								// METRIC LINE: M06 PMR, M11 PMD
		float max_mag_2 = 0;								// METRIC LINE: M06 PMR, M11 PMD
		float mag_sum_1 = 0;								// METRIC LINE: M07 PSR, M12 PSD
		float mag_sum_2 = 0;								// METRIC LINE: M07 PSR, M12 PSD
		float product_sum = 0;							// METRIC LINE: M08 FC
		float mag_cum_1 = 0;								// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
		float mag_cum_2 = 0;								// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
		float cum_1_ratio = 0;							// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
		float cum_2_ratio = 0;							// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
		float q1_freq = 0;									// METRIC LINE: M16 FQF
		float q2_freq = 0;									// METRIC LINE: M17 SQF
		float q3_freq = 0;									// METRIC LINE: M18 TQF

    for(i=0;i<TOTAL_SAMPLES_INT;i++)
    {		// <FIRST FREQ DATA LOOP>
	  fscanf(fftfile_in,"%s",datainchar);
	  LBSG_y[i] = atof(datainchar);
	  time_x[i] = i/SAMPLE_FREQ;

		if(time_x[i]>=LOWER_FREQ && time_x[i]<=UPPER_FREQ)
		{
			magnitude_sum += LBSG_y[i];								// METRIC LINE: M01 SOM
			product_sum += LBSG_y[i]*time_x[i];					// METRIC LINE: M08 FC
			if(LBSG_y[i]>max_magnitude){
				peak_frequency = time_x[i] / 1000;				// METRIC LINE: M02 WPF, M03 PCR, M13 PF
				max_magnitude = LBSG_y[i];								// METRIC LINE: M04 PAR, M09 PM
			}else{}

			// Spectrum Summations
			if(time_x[i]>=LOWER_FREQ && time_x[i]<=((UPPER_FREQ+LOWER_FREQ)/2)){
				mag_sum_1 += LBSG_y[i];   								//	METRIC LINE: M07 PSR, M12 PSD
				if(LBSG_y[i]>max_mag_1){
					peak_freq_1 = time_x[i];								// METRIC LINE: M05 PFR
					max_mag_1 = LBSG_y[i];}								// METRIC LINE: M06 PMR, M11 PMD
			}else if(time_x[i]>=((UPPER_FREQ+LOWER_FREQ)/2) && time_x[i]<=UPPER_FREQ){
				mag_sum_2 += LBSG_y[i];									//	METRIC LINE: M07 PSR, M12 PSD
				if(LBSG_y[i]>max_mag_2){
					peak_freq_2 = time_x[i];								// METRIC LINE: M05 PFR
					max_mag_2 = LBSG_y[i];}}}							// METRIC LINE: M06 PMR, M11 PMD

		// </LOAD FREQ DATA LOOP>
        if(fmod(i,1000)==0)
        {
            float percentage;
            percentage = ((100*i)/TOTAL_SAMPLES_INT);
            printf("\rFFT Data Load Progress: %0.1f %%",percentage);
        }
    }		// </FIRST LOAD LOOP END>
		printf("\tLoaded in %0.2f seconds\n",(clock()-section_start_time)/CLK_TCK);

		for(i=0;i<TOTAL_SAMPLES_INT;i++)
	  {		// <SECOND FREQ DATA LOOP>
			if(time_x[i]>=LOWER_FREQ && time_x[i]<=UPPER_FREQ){
				mag_cum_1 = mag_cum_2;										// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
				mag_cum_2 += LBSG_y[i];										// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
				cum_1_ratio = mag_cum_1/magnitude_sum;		// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
				cum_2_ratio = mag_cum_2/magnitude_sum;		// METRIC LINE: M16 FQF, M17 SQF, M18 TQF
				if(cum_1_ratio<0.25){
					if(cum_2_ratio>=0.25){
						q1_freq = time_x[i] / 1000;						// METRIC LINE: M16 FQF
				}}else if(cum_1_ratio<0.50){
				 	if(cum_2_ratio>=0.50){
						q2_freq = time_x[i] / 1000;						// METRIC LINE: M17 SQF
				}}else if(cum_1_ratio<0.75){
				 	if(cum_2_ratio>=0.75){
						q3_freq = time_x[i] / 1000;}}}					// METRIC LINE: M18 TQF
		} 	// </SECOND LOAD LOOP END>		<END OF LOAD FREQ DATA>

		// METRICS: METRIC CALCULATIONS AFTER LOOP
		float total_magnitude = 0;								// METRIC LINE: M01 SOM initialisation
		float weighted_peak_frequency = 0;				// METRIC LINE: M02 WPF initialisation
		float peak_to_centroid_ratio = 0;					// METRIC LINE: M03 PCR initialisation
		float avg_magnitude = 0;									// METRIC LINE: M04 PAR, M10 AM initialisation
		float peak_to_average_ratio = 0;					// METRIC LINE: M04 PAR initialisation
		float peak_frequency_ratio = 0;						// METRIC LINE: M05 PFR initialisation
		float peak_magnitude_ratio = 0;						// METRIC LINE: M06 PMR initialisation
		float peak_spectrum_ratio = 0;						// METRIC LINE: M07 PSR initialisation
		float frequency_centroid = 0;							// METRIC LINE: M08 FC initialisation
		float peak_magnitude_diff = 0;						// METRIC LINE: M11 PMD initialisation
		float peak_spectrum_diff = 0;							// METRIC LINE: M12 PSD initialisation
		float peak_product_ratio = 0;							// METRIC LINE: M14 PPR initialisation
		float peak_interactive_ratio = 0;					// METRIC LINE: M15 PIR initialisation
		float iq_range = 0;												// METRIC LINE: M19 IQR
		float quartile_ratio1 = 0;								// METRIC LINE: M20 QR1
		float quartile_ratio2 = 0;								// METRIC LINE: M21 QR2
		float quartile_ratio3 = 0;								// METRIC LINE: M22 QR3

		total_magnitude = magnitude_sum / 10000; 																		// METRIC LINE: M01 Calculation (Sum of Magnitude)
		frequency_centroid = product_sum / magnitude_sum / 1000;										// METRIC LINE: M08 Calculation (Frequency Centroid)
		weighted_peak_frequency = sqrt(peak_frequency * frequency_centroid);				// METRIC LINE: M02 Calculation (Weighted Peak Frequency)
		peak_to_centroid_ratio = peak_frequency / frequency_centroid;								// METRIC LINE: M03 Calculation (Peak to Centroid Ratio)
		avg_magnitude = magnitude_sum / ((UPPER_FREQ-LOWER_FREQ)/SAMPLE_FREQ);			// METRIC LINE: M10 Calculation (Average Magnitude)
		peak_to_average_ratio = max_magnitude / avg_magnitude;											// METRIC LINE: M04 Calculation (Peak to Average Ratio)
		peak_magnitude_ratio = max_mag_1 / max_mag_2;   														// METRIC LINE: M06 Calculation (Peak Magnitude Ratio)
		peak_spectrum_ratio = mag_sum_1 / mag_sum_2;																// METRIC LINE: M07 Calculation (Peak Spectrum Ratio)
		peak_product_ratio = (peak_freq_1*max_mag_1)/(peak_freq_2*max_mag_2);   		// METRIC LINE: M14 Calculation (Peak Product Ratio)
		peak_interactive_ratio = (peak_freq_1*max_mag_2)/(peak_freq_2*max_mag_1);		// METRIC LINE: M15 Calculation (Peak Interactive Ratio)
		iq_range = q3_freq - q1_freq;																								// METRIC LINE: M19 Calculation (Interquartile Range)
		quartile_ratio1 = q1_freq / q2_freq;																				// METRIC LINE: M20 Calculation (Quartile Ratio 1)
		quartile_ratio2 = q1_freq / q3_freq;																				// METRIC LINE: M21 Calculation (Quartile Ratio 2)
		quartile_ratio3 = q2_freq / q3_freq;																				// METRIC LINE: M22 Calculation (Quartile Ratio 3)

		if(max_mag_1>=max_mag_2){
			peak_frequency_ratio = peak_freq_1 / peak_freq_2;   											// METRIC LINE: M05 Calculation (Peak Frequency Ratio)
			peak_magnitude_diff = max_mag_1 - max_mag_2;															// METRIC LINE: M11 Calculation (Peak Magnitude Difference)
		}else if(max_mag_1<max_mag_2){
			peak_frequency_ratio = peak_freq_2 / peak_freq_1;   											// METRIC LINE: M05 Calculation (Peak Frequency Ratio)
			peak_magnitude_diff = max_mag_2 - max_mag_1;}															// METRIC LINE: M11 Calculation (Peak Magnitude Difference)

		if(mag_sum_1>=mag_sum_2){
			peak_spectrum_diff = mag_sum_1 - mag_sum_2;																// METRIC LINE: M12 Calculation (Peak Spectrum Difference)
		}else if(mag_sum_1<mag_sum_2){
			peak_spectrum_diff = mag_sum_2 - mag_sum_1;}															// METRIC LINE: M12 Calculation (Peak Spectrum Difference)

		// METRICS: Print the Frequency Domain Metrics
		metric_print(roundf(total_magnitude*1000)/1000); 							/* PRINT METRIC: M01 Sum of Magnitude (SOM) */
		metric_print(lround(weighted_peak_frequency)); 								/* PRINT METRIC: M02 Weighted Peak Frequency (WPF) */
		metric_print(roundf(peak_to_centroid_ratio*1000)/1000); 			/* PRINT METRIC: M03 Peak to Centroid Ratio (PCR) */
		metric_print(roundf(peak_to_average_ratio*1000)/1000); 				/* PRINT METRIC: M04 Peak to Average Ratio (PAR) */
		metric_print(roundf(peak_frequency_ratio*1000)/1000); 				/* PRINT METRIC: M05 Peak Frequency Ratio (PFR) */
		metric_print(roundf(peak_magnitude_ratio*1000)/1000); 			 	/* PRINT METRIC: M06 Peak Magnitude Ratio (PMR) */
		metric_print(roundf(peak_spectrum_ratio*1000)/1000); 					/* PRINT METRIC: M07 Peak Spectrum Ratio (PSR) */
		metric_print(lround(frequency_centroid)); 										/* PRINT METRIC: M08 Frequency Centroid (FC) */
		metric_print(roundf(max_magnitude*1000)/1000); 								/* PRINT METRIC: M09 Peak Magnitude (PM) */
		metric_print(roundf(avg_magnitude*1000)/1000); 								/* PRINT METRIC: M10 Average Magnitude (AM) */
		metric_print(roundf(peak_magnitude_diff*1000)/1000); 					/* PRINT METRIC: M11 Peak Magnitude Difference (PMD) */
		metric_print(roundf((peak_spectrum_diff/10000)*1000)/1000); 	/* PRINT METRIC: M12 Peak Spectrum Difference (PSD) */
		metric_print(lround(peak_frequency)); 												/* PRINT METRIC: M13 Peak Frequency (PF) */
		metric_print(roundf(peak_product_ratio*1000)/1000); 			 		/* PRINT METRIC: M14 Peak Product Ratio (PPR) */
		metric_print(roundf(peak_interactive_ratio*1000)/1000); 			/* PRINT METRIC: M15 Peak Interactive Ratio (PIR) */
		metric_print(lround(q1_freq)); 																/* PRINT METRIC: M16 First Quartile Frequency (FQF) */
		metric_print(lround(q2_freq)); 																/* PRINT METRIC: M17 Second Quartile Frequency (SQF) */
		metric_print(lround(q3_freq)); 																/* PRINT METRIC: M18 Third Quartile Frequency (TQF) */
		metric_print(lround(iq_range)); 															/* PRINT METRIC: M19 Interquartile Range (IQR) */
		metric_print(roundf(quartile_ratio1*1000)/1000); 							/* PRINT METRIC: M20 Quartile Ratio 1 (QR1) */
		metric_print(roundf(quartile_ratio1*1000)/1000); 							/* PRINT METRIC: M21 Quartile Ratio 2 (QR2) */
		metric_print(roundf(quartile_ratio1*1000)/1000); 							/* PRINT METRIC: M22 Quartile Ratio 3 (QR3) */

		printf("\n");
		//printf("%d\n",TOTAL_SAMPLES_INT);
}
//  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
//  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //


///////////////////////////////////////////////////////////////////////////////
 //  | Function: metric_print
 ///////////////////////////////////////////////////////////////////////////////
 //  |     Desc: This function is intended to print the metrics to file
 //  |
 //	|
 ///////////////////////////////////////////////////////////////////////////////
 void metric_print(float value)
 {
	 fprintf(fileout,"\t%f",value);
 }
 //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
   //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //
 //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //  //

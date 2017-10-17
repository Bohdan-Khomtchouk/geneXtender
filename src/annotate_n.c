/* Copyright (C) 2016-2017 Bohdan Khomtchouk */

/*

EXPLANATION:
This program takes in a variant list and goes through a single vcf file to find the matching variants within the vcf file.  It outputs the variants in the vcf file to a separate out vcf file that the user specifies.

COMPILE CODE USING THIS COMMAND:
gcc -o ex test

USAGE assuming compiled with above command:
./ex <variant bed file> <gtf bed file>

Example USAGE:
./ex test1.var test1.vcf

ASSUMPTIONS:
1.  Both the variant file and the gtf file are properly sorted in the same manner.
2.  The line within the gtf file does not exceed 10000 characters
3.  The user has write permissions to the executing directory
4.  There is no header within either of the files.  The structure of the file is simply chr \t position \t position.

*/

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <Rcpp.h>

int countlines2(char ** fname) {
	int ch = 0;
	int lines = 0;
	FILE * fp = fopen(*fname, "r");
	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (ch == '\n')
		{
			lines++;
		}
	}
	return lines;
}

void annotate_n(char **f1, char **f2, char ** Rlist, int ** n_near)
{
  char varstr[50000];
  char vcfstr[50000];
  char varstr1[50000];
  char vcfstr1[50000];
  char col1[100];
  char col2[100];
  
  
  char * pvarcol1;
  char * pvarcol2;
  char * pvarcol3;
  char * pvcfcol1;
  char * pvcfcol2;
  char * pvcfcol3;
  char * pvcfcol4;
  char * pvcfcol5;
  char * end_str;
  char * end_str2;
  FILE * vcffp = NULL;
  FILE * varfp = NULL;
  long i;
  long j;
  
  long k;
  long l;
  long l_1;
  long n_1;
  long j_1;
  long m;
  long n;
  long v1;
  long v2;
  long v3;
  long v4;
  int zero_count;
  int pflag;
  int wflag;
  
  wflag = 1;
  zero_count = 0;
  pflag = 0;
  /* open up all the files and make sure they are open, else return value 1 */
  varfp = fopen(*f1, "r");
  if (varfp == NULL)
  {
    Rprintf("Problem opening variant file\n");
  }
  vcffp = fopen(*f2, "r");
  if (vcffp == NULL)
  {
    Rprintf("Problem opening vcf file\n");
  }
  
  // Get the first lines of both files and see what's up.
  
  fgets(varstr, 50000, varfp);
  strcpy(varstr1, varstr);
  pvarcol1 = strtok_r(varstr, "\t\n", &end_str);
  k = strtol(pvarcol1, NULL, 0);                //Chromosome
  pvarcol2 = strtok_r(NULL, "\t\n", &end_str);
  i = strtol(pvarcol2, NULL, 0);                //Start-Peak
  pvarcol3 = strtok_r(NULL, "\t\n", &end_str);
  m = strtol(pvarcol3, NULL, 0);                //End-Peak
  
  
  fgets(vcfstr, 10000, vcffp);
  strcpy(vcfstr1, vcfstr);
  pvcfcol1 = strtok_r(vcfstr, "\t", &end_str2);
  n = strtol(pvarcol1, NULL, 0);              //Chromosome
  pvcfcol2 = strtok_r(NULL, "\t", &end_str2);
  j = strtol(pvcfcol2, NULL, 0);              //Start-Gene
  pvcfcol3 = strtok_r(NULL, "\t", &end_str2);
  l = strtol(pvcfcol3, NULL, 0);              //End-Gene
  pvcfcol4 = strtok_r(NULL, "\t", &end_str2);
  pvcfcol5 = strtok_r(NULL, "\t\n", &end_str2);
  
  
  strcpy(col1, pvcfcol4);
  strcpy(col2, pvcfcol5);
  
  n_1 = n;      //Start first Chromosome
  j_1 = j;      //Start first Gene
  l_1 = l;      //End first Gene
  
  v1 = i - j;   //Start-Peak - Start-Gene
  v2 = i - l;   //Start-Peak - End-Gene
  v3 = m - j;   //End-Peak - Start-Gene
  v4 = m - l;   //End-Peak - End-Gene
  
  /*===============================================R/C code====================================================*/
  
  int counter = 0;
  char Buffer[100];
  int skip_counter = 0;
  
  /*============================================================================================================*/
  
  
  while (wflag > 0)
  {
    int numchars = 0;
    memset(Buffer, ' ', 100);
    
    // check chromosomes.  If gtf is on next chromosome then move var file appropriately.
    
    if (k < n) { //If Peaks-Chrome < Gene-Chrome
      skip_counter++;
      if (skip_counter == **n_near) {
        /*===============================================R/C code====================================================*/
        numchars = snprintf(Rlist[counter], sizeof(Buffer), "%s\t%s\t%s\t%ld\t%ld\t%ld\t%s\t%s\t%ld", pvarcol1, pvarcol2, pvarcol3, n_1, j_1, l_1, col1, col2, labs((i - l_1)) + 1);
        if (numchars >= sizeof(Buffer))
          Rf_error("annotate() doesn't handle output lines longer than %d characters", sizeof(Buffer));
        counter++; //Plus counter, Write?
        /*============================================================================================================*/
        skip_counter = 0;
      }
      
      
      
      pflag = 1;  //increase GTF CHROME
    }
    else if (k > n) {
      pflag = 2;  //Increase Peaks CHROME
      
    }
    else {
      if ((v1 > 0) && (v2 > 0) && (v3 > 0) && (v4 > 0)) {
        // if the peak in the gtf is all positive then we need to update the previous pointer to the current gtf peak        
        strcpy(col1, pvcfcol4);
        strcpy(col2, pvcfcol5);
        n_1 = n;
        j_1 = j;
        l_1 = l;
        // set pflag to increase the gtf file
        pflag = 2;
      }
      else if ((v1 < 0) && (v2 < 0) && (v3 < 0) && (v4 < 0)) {
        
        // if all is negative then we need to which which is the closest peak and then move the peak file to the next flag.
        if ((j - m) < (i - l_1)) { //IF start-gene - end-peak < Start-peak - End-gene
          // the previous pointer is closer to the distance so we need to put the distance there
          skip_counter++;
          if (skip_counter == **n_near) {
            /*===============================================R/C code====================================================*/
            numchars = snprintf(Rlist[counter], sizeof(Buffer), "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%ld", pvarcol1, pvarcol2, pvarcol3, pvcfcol1, pvcfcol2, pvcfcol3, pvcfcol4, pvcfcol5, labs((j - m)) + 1);
            if (numchars >= sizeof(Buffer))
              Rf_error("annotate() doesn't handle output lines longer than %d characters", sizeof(Buffer));
            counter++;
            skip_counter = 0;
            /*============================================================================================================*/
          }
        }
        else {
          
          if (n_1 == n) {
            skip_counter++;
            if (skip_counter == **n_near) {
              numchars = snprintf(Rlist[counter], sizeof(Buffer), "%s\t%s\t%s\t%ld\t%ld\t%ld\t%s\t%s\t%ld", pvarcol1, pvarcol2, pvarcol3, n_1, j_1, l_1, col1, col2, labs((i - l_1)) + 1);
              if (numchars >= sizeof(Buffer))
                Rf_error("annotate() doesn't handle output lines longer than %d characters", sizeof(Buffer));
              counter++;
              skip_counter = 0;
              /*============================================================================================================*/
            }
          }
          else {
            skip_counter++;
            if (skip_counter == **n_near) {
              /*===============================================R/C code====================================================*/
              numchars = snprintf(Rlist[counter], sizeof(Buffer), "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%ld", pvarcol1, pvarcol2, pvarcol3, pvcfcol1, pvcfcol2, pvcfcol3, pvcfcol4, pvcfcol5, labs((j - m)) + 1);
              if (numchars >= sizeof(Buffer))
                Rf_error("annotate() doesn't handle output lines longer than %d characters", sizeof(Buffer));
              counter++;
              skip_counter = 0;
              /*============================================================================================================*/
            }
          }
          
          
          
          
          
        }
        // set the pflag to represent a movement in the peak file
        pflag = 1;
      }
      else {
        /* add to the running total of the overlap and move ot the next query */
        zero_count = zero_count + 1;
        skip_counter++;
        if (skip_counter == **n_near) {
          /*===============================================R/C code====================================================*/
          numchars = snprintf(Rlist[counter], sizeof(Buffer), "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t0", pvarcol1, pvarcol2, pvarcol3, pvcfcol1, pvcfcol2, pvcfcol3, pvcfcol4, pvcfcol5);
          if (numchars >= sizeof(Buffer))
            Rf_error("annotate() doesn't handle output lines longer than %d characters", sizeof(Buffer));
          counter++;
          skip_counter = 0;
          /*============================================================================================================*/
        }
        
        
        pflag = 1;
      }
    }
    
    switch (pflag) {
    case 1:
      // increase the gtf file
      if (fgets(varstr, 50000, varfp) != NULL) {
        strcpy(varstr1, varstr);
        pvarcol1 = strtok_r(varstr, "\t\n", &end_str);
        k = strtol(pvarcol1, NULL, 0);
        pvarcol2 = strtok_r(NULL, "\t\n", &end_str);
        i = strtol(pvarcol2, NULL, 0);
        pvarcol3 = strtok_r(NULL, "\t\n", &end_str);
        m = strtol(pvarcol3, NULL, 0);
      }
      else { wflag = 0; };
      break;
    case 2:
      // increase the peak file
      if (fgets(vcfstr, 10000, vcffp) != NULL) {
        strcpy(vcfstr1, vcfstr);
        pvcfcol1 = strtok_r(vcfstr, "\t", &end_str2);
        n = strtol(pvcfcol1, NULL, 0);
        pvcfcol2 = strtok_r(NULL, "\t", &end_str2);
        j = strtol(pvcfcol2, NULL, 0);
        pvcfcol3 = strtok_r(NULL, "\t", &end_str2);
        l = strtol(pvcfcol3, NULL, 0);
        pvcfcol4 = strtok_r(NULL, "\t", &end_str2);
        pvcfcol5 = strtok_r(NULL, "\t\n", &end_str2);
      }
      else { wflag = 0; }
      break;
    }
    
    v1 = i - j;
    v2 = i - l;
    v3 = m - j;
    v4 = m - l;
  }
  fclose(varfp);
  fclose(vcffp);
  
  
  
}


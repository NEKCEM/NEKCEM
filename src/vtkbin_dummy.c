#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*#ifdef NEED_TRAILING_UNDERSCORE
   #define FORTRAN(SUBROUTINE_NAME) SUBROUTINE_NAME##_
#else
  #define FORTRAN(SUBROUTINE_NAME) SUBROUTINE_NAME
#endif
FORTRAN(getfieldname) */

/*
#ifdef NEED_TRAILING_UNDERSCORE
  #define FORTRAN(SUBROUTINE_NAME) SUBROUTINE_NAME##_
#else
  #define FORTRAN(SUBROUTINE_NAME) SUBROUTINE_NAME
#endif
*/

/*
FORTRAN(openfile)
FORTRAN(closefile)
FORTRAN(writeheader)  
FORTRAN(writenodes)   
FORTRAN(write2dcells) 
FORTRAN(write3dcells) 
FORTRAN(writefield)   
*/


FILE *fp = NULL;
char filename[100];
int Little_endian = -1;

void getfieldname_( int i, char *name )
{
   int id = i;

   switch( id )
   {
           case 333:
                   strcpy(name, "Velocity");
                   break;
           case 334:
                   strcpy(name, "SolVeloc");
                   break;
           case 335:
                   strcpy(name, "Pressure");
                   break;
           case 336:
                   strcpy(name, "Density");
                   break;
           case 337:
                   strcpy(name, "Vorticity");
   }
}

void adjust_endian()
{
        int endian_int = 1;
        char* pchar = &endian_int;
//      for(int i = 0 ; i < 4; i++) printf(" -%d ",(int) *(pchar+i));
        if(* (pchar+3)  == 1) Little_endian = 0;
        else Little_endian = 1;
}

void getfilename_(int *id, int *nid )
{
   char ext0[100];
   char ext1[100];

       /*printf( "\n  nid:: %d\n", *nid);*/
       strcpy( filename, "./vtk/binary-NN-p");
       sprintf( ext0, "%.6d-t", *nid);
       strcat( filename, ext0);
       sprintf( ext1, "%.5d", *id);
       strcat( filename, ext1);
       strcat( filename, ".vtk");
       adjust_endian();
}

int swap_int_byte(int *n)
{
if(Little_endian == 1)
{
  unsigned char *cptr,tmp;

  cptr = (unsigned char *)n;
  tmp = cptr[0];
  cptr[0] = cptr[3];
  cptr[3] = tmp;
  tmp = cptr[1];
  cptr[1] = cptr[2];
  cptr[2] = tmp;
}
  return 0;
}

int swap_float_byte(float *n)
{
if(Little_endian == 1)
{
  unsigned char *cptr,tmp;

  cptr = (unsigned char *)n;
  tmp  = cptr[0];
  cptr[0] = cptr[3];
  cptr[3] = tmp    ;
  tmp     = cptr[1];
  cptr[1] = cptr[2];
  cptr[2] = tmp    ;
}
  return 0;
}

#ifdef UPCASE
void OPENFILE(  int *id, int *nid)
#elif  IBM
void openfile(  int *id, int *nid)
#else
void openfile_(  int *id, int *nid)
#endif
{
   getfilename_(id,nid);
   fp = fopen(filename,  "w"); assert(fp);
}
  
#ifdef UPCASE
void CLOSEFILE()
#elif  IBM
void closefile()
#else
void closefile_()
#endif
{
   fclose(fp);
}

#ifdef UPCASE
void WRITEHEADER()
#elif  IBM
void writeheader()
#else
void writeheader_()
#endif
{
   int i ;/* np = 10;*/
   float xyz[3];
   assert( fp );
   
   /*printf("# vtk DataFile Version 2.0 \n"); */
   fprintf(fp, "# vtk DataFile Version 2.0 \n"); 
   fprintf(fp, "Electromagnetic Field  \n");
   fprintf(fp, "BINARY \n");
   fprintf(fp, "DATASET UNSTRUCTURED_GRID \n");
}

#ifdef UPCASE
void WRITENODES(double *xyzCoords, int *numNodes)
#elif  IBM
void writenodes(double *xyzCoords, int *numNodes)
#else
void writenodes_(double *xyzCoords, int *numNodes)
#endif
{
   float coord[3];
   int   i, j;
   fprintf(fp, "POINTS  %d ", *numNodes );
   fprintf(fp, " float  \n");
   for( i = 0; i < *numNodes; i++) {
       coord[0] = (float)xyzCoords[3*i+0];
       coord[1] = (float)xyzCoords[3*i+1];
       coord[2] = (float)xyzCoords[3*i+2];
       swap_float_byte( &coord[0] );
       swap_float_byte( &coord[1] );
       swap_float_byte( &coord[2] );
       fwrite(coord, sizeof(float), 3, fp);
   }
   fprintf( fp, " \n");
}

#ifdef UPCASE
void WRITE2DCELLS( int *eConnect, int *numElems, int *numCells, int *numNodes)
#elif  IBM
void write2dcells( int *eConnect, int *numElems, int *numCells, int *numNodes)
#else
void write2dcells_( int *eConnect, int *numElems, int *numCells, int *numNodes)
#endif
{
   int conn[5];                   
   int i, j;
   int elemType=9;

   fprintf( fp, "CELLS %d  %d \n", *numCells, 5*(*numCells));
   
   for (i = 0; i < *numCells; i++) {
        conn[0] = 4; 
        conn[1] = eConnect[4*i+0];
        conn[2] = eConnect[4*i+1];
        conn[3] = eConnect[4*i+2];
        conn[4] = eConnect[4*i+3];
	for( j = 0; j < 5; j++) swap_int_byte( &conn[j] );
        fwrite(conn, sizeof(int), 5, fp);
   }
   fprintf( fp, "\n");
   fprintf( fp, "CELL_TYPES %d \n", *numCells);

   swap_int_byte(&elemType);

   for( i = 0; i < *numCells; i++) 
    fwrite(&elemType,  sizeof(int), 1, fp);

   fprintf( fp, "\n");
   fprintf( fp, "POINT_DATA %d \n", *numNodes);
}

#ifdef UPCASE
void WRITE3DCELLS( int *eConnect, int *numElems, int *numCells, int *numNodes)
#elif  IBM
void write3dcells( int *eConnect, int *numElems, int *numCells, int *numNodes)
#else
void write3dcells_( int *eConnect, int *numElems, int *numCells, int *numNodes)
#endif
{
   int conn[9];                         
   int i, j;
   int elemType=12;

   fprintf( fp, "CELLS %d  %d \n", *numCells, 9*(*numCells) );
   
   for (i = 0; i < *numCells; i++) {
        conn[0] = 8;
        conn[1] = eConnect[8*i+0];
        conn[2] = eConnect[8*i+1];
        conn[3] = eConnect[8*i+2];
        conn[4] = eConnect[8*i+3];
        conn[5] = eConnect[8*i+4];
        conn[6] = eConnect[8*i+5];
        conn[7] = eConnect[8*i+6];
        conn[8] = eConnect[8*i+7];
	for( j = 0; j < 9; j++) swap_int_byte( &conn[j] );
        fwrite(conn, sizeof(int), 9, fp);
   }
   fprintf( fp, "\n");
   fprintf( fp, "CELL_TYPES %d \n", *numCells );

   swap_int_byte(&elemType);

   for (i = 0; i < *numCells; i++) 
     fwrite(&elemType,  sizeof(int), 1, fp);

   fprintf( fp, "\n");
   fprintf( fp, "POINT_DATA %d \n", *numNodes );
}

#ifdef UPCASE
void WRITEFIELD(int *fldid, double *vals, int *numNodes)
#elif  IBM
void writefield(int *fldid, double *vals, int *numNodes)
#else
void writefield_(int *fldid, double *vals, int *numNodes)
#endif
{
   float fldval[3];
   int   i, j  ;
   char  fldname[100];

   getfieldname_(*fldid, fldname);

   fprintf( fp, "VECTORS %s ", fldname);
   fprintf( fp, " float \n");
   
   for (i = 0; i < *numNodes; i++) {

        fldval[0] = (float)vals[3*i+0];
        fldval[1] = (float)vals[3*i+1];
        fldval[2] = (float)vals[3*i+2];
        swap_float_byte( &fldval[0]);
        swap_float_byte( &fldval[1]);
        swap_float_byte( &fldval[2]);
        fwrite(fldval, sizeof(float), 3, fp);
   }
   fprintf(fp, " \n");
}
                   

#ifdef UPCASE
void OPENFILE4(  int *id, int *nid);
#elif  IBM
void openfile4(  int *id, int *nid);
#else
void openfile4_(  int *id, int *nid){}
#endif


#ifdef UPCASE
void CLOSEFILE4();
#elif  IBM
void closefile4();
#else
void closefile4_(){};
#endif

#ifdef UPCASE
void WRITEHEADER4();
#elif  IBM
void writeheader4();
#else
void writeheader4_(){};
#endif


#ifdef UPCASE
void WRITENODES4(double *xyzCoords, int *numNodes);
#elif  IBM
void writenodes4(double *xyzCoords, int *numNodes);
#else
void writenodes4_(double *xyzCoords, int *numNodes){};
#endif


#ifdef UPCASE
void WRITE2DCELLS4( int *eConnect, int *numElems, int *numCells, int *numNodes);
#elif  IBM
void write2dcells4( int *eConnect, int *numElems, int *numCells, int *numNodes);
#else
void write2dcells4_( int *eConnect, int *numElems, int *numCells, int *numNodes){};
#endif


#ifdef UPCASE
void WRITE3DCELLS4( int *eConnect, int *numElems, int *numCells, int *numNodes);
#elif  IBM
void write3dcells4( int *eConnect, int *numElems, int *numCells, int *numNodes);
#else
void write3dcells4_( int *eConnect, int *numElems, int *numCells, int *numNodes){};
#endif

#ifdef UPCASE
void WRITEFIELD4(int *fldid, double *vals, int *numNodes);
#elif  IBM
void writefield4(int *fldid, double *vals, int *numNodes);
#else
void writefield4_(int *fldid, double *vals, int *numNodes){};
#endif

#ifdef UPCASE
void INITRBIO(int *numgroups)
#elif  IBM
void initrbio(int *numgroups)
#else
void initrbio_(int *numgroups)
#endif
{}

#ifdef UPCASE
void OPENFILE6(  int *id, int *nid)
#elif  IBM
void openfile6(  int *id, int *nid)
#else
void openfile6_(  int *id, int *nid)
#endif
{}

#ifdef UPCASE
void CLOSEFILE6()
#elif  IBM
void closefile6()
#else
void closefile6_()
#endif
{}

#ifdef UPCASE
void WRITEHEADER6()
#elif  IBM
void writeheader6()
#else
void writeheader6_()
#endif
{}

#ifdef UPCASE
void WRITENODES6(double *xyzCoords, int *numNodes)
#elif  IBM
void writenodes6(double *xyzCoords, int *numNodes)
#else
void writenodes6_(double *xyzCoords, int *numNodes)
#endif
{}

#ifdef UPCASE
void WRITE2DCELLS6( int *eConnect, int *numElems, int *numCells, int *numNodes)
#elif  IBM
void write2dcells6( int *eConnect, int *numElems, int *numCells, int *numNodes)
#else
void write2dcells6_( int *eConnect, int *numElems, int *numCells, int *numNodes)
#endif
{}

#ifdef UPCASE
void WRITE3DCELLS6( int *eConnect, int *numElems, int *numCells, int *numNodes)
#elif  IBM
void write3dcells6( int *eConnect, int *numElems, int *numCells, int *numNodes)
#else
void write3dcells6_( int *eConnect, int *numElems, int *numCells, int *numNodes)
#endif
{}

#ifdef UPCASE
void WRITEFIELD6(int *fldid, double *vals, int *numNodes)
#elif  IBM
void writefield6(int *fldid, double *vals, int *numNodes)
#else
void writefield6_(int *fldid, double *vals, int *numNodes)
#endif
{}

#ifdef UPCASE
void SET_ASCII_TRUE(int *numgroups)
#elif  IBM
void set_ascii_true(int *numgroups)
#else
void set_ascii_true_(int *numgroups)
#endif
{}

#ifdef UPCASE
void SET_ASCII_NMM(int *numgroups)
#elif  IBM
void set_ascii_nmm(int *numgroups)
#else
void set_ascii_nmm_(int *numgroups)
#endif
{}

#ifdef UPCASE
void SET_ASCII_NM(int *numgroups)
#elif  IBM
void set_ascii_nm(int *numgroups)
#else
void set_ascii_nm_(int *numgroups)
#endif
{}

#ifdef UPCASE
void PVTK_NMM(  int *id)
#elif  IBM
void pvtk_nmm(  int *id)
#else
void pvtk_nmm_(  int *id)
#endif
{}

#ifdef UPCASE
void STARTTIMING()
#elif  IBM
void starttiming()
#else
void starttiming_()
#endif
{}


#ifdef UPCASE
void ENDTIMING()
#elif  IBM
void endtiming()
#else
void endtiming_()
#endif
{}

#ifdef UPCASE
void WRITEIOTRACE(int *fparam, int* piostep)
#elif  IBM
void writeiotrace(int *fparam, int* piostep)
#else
void writeiotrace_(int *fparam, int* piostep)
#endif
{}

#ifdef UPCASE
void FREE_RBIO_BUFFER ()
#elif  IBM
void free_rbio_buffer ()
#else
void free_rbio_buffer_()
#endif
{}


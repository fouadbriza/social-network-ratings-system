/**********************************************************
 * Implementation of TOF module functions (TOF_model.c)   *
 * SFSD (File & Data Structures) / 2CP / ESI / 2024       *
 **********************************************************/

#include "TOF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// open a TOF file / mode ='N' for a New file and mode ='E' for an Existing file
// returns a pointer to a newly allocated variable of type 't_TOF'
void TOF_open( t_TOF **F, char *fname, char mode )
{
   *F = malloc( sizeof( t_TOF) );
   if ( mode == 'E' || mode == 'e' ) {
        // openning an existing TOF file ...
(*F)->f = fopen( fname, "rb+" );
if ( (*F)->f == NULL ) {
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // loading header part in main memory (in (*F)->h)
fread( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
   else { // mode == 'N' || mode == 'n'
        // creating a new TOF file ...
(*F)->f = fopen( fname, "wb+" );
if ( (*F)->f == NULL ) {
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // initializing the header part in main memory (in (*)->h)
(*F)->h.nBlock = 0;

fwrite( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
}


// close a TOF file :
// the header is first saved at the beginning of the file and the t_TOF variable is freed
void TOF_close( t_TOF *F )
{
   // saving header part in secondary memory (at the begining of the stream F->f)
   fseek(F->f, 0L, SEEK_SET);
   fwrite( &F->h, sizeof(t_header), 1, F->f );
   fclose(F->f);
   free( F );
}


// reading data block number i into variable buf
void TOF_readBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fread( buf, sizeof(t_block), 1, F->f );
}


// writing the contents of the variable buf in data block number i
void TOF_writeBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fwrite( buf, sizeof(t_block), 1, F->f );
}


// header modification
void setHeader( t_TOF *F , long val )
{
     F->h.nBlock = val;


}


// header value
int getHeader( t_TOF *F )
{
    return F->h.nBlock;


}
void Rech(int cle, int *Trouv, int *i, int *j, t_TOF *F)
{
    int bi, bs;
    int inf, sup;
    int stop;
    t_block buf;

    bs = getHeader(F); // last block
    bi = 1;                      // first block
    *Trouv = 0;
    stop = 0;
    *j = 1;

    while(bi <= bs && !(*Trouv) && !stop)
    {
        *i = (bi + bs)/2;
        TOF_readBlock(F,*i,&buf);

        if(cle >= buf.tab[1].student1_id && cle <= buf.tab[buf.nb].student1_id)
        {
            inf = 1; sup = buf.nb;
            while(inf <= sup && !(*Trouv))
            {
                *j = (inf + sup)/2;
                if(cle == buf.tab[*j].student1_id)
                    *Trouv = 1;
                else if(cle < buf.tab[*j].student1_id)
                    sup = *j-1;
                else
                    inf = *j+1;
            }

            if(inf > sup) *j = inf;
            else if(buf.eff[*j]==1) *Trouv=0;
            stop = 1;
        }
        else
        {
            if(cle < buf.tab[1].student1_id) bs = *i -1;
            else bi = *i +1;
        }
    }

    if(bi>bs){ *i=bi; *j=1; }
}


void Insert(t_TOF *F, t_rec e)
{
    int trouv, i, j;
    int reutilisation = 0;
    int continu = 0;
    t_block buf;
    int max_allowed;

    Rech(e.student1_id, &trouv, &i, &j, F);

    if(trouv) return; // already exists

    continu = 1;

    if(i > getHeader(F)) buf.nb=0; // new block
    else TOF_readBlock(F,i,&buf);

    max_allowed = (int)(0.75*MAXTAB);

    while(continu)
    {
        int stop=0;
        while(!stop && j <= buf.nb)
        {
            t_rec x = buf.tab[j];
            buf.tab[j] = e;

            if(buf.eff[j]==1) stop=1;
            else { e = x; j++; }
        }

        if(stop)
        {
            buf.eff[j]=0;
            reutilisation=1;
            TOF_writeBlock(F,i,&buf);
            continu=0;
        }
        else if(buf.nb < max_allowed)
        {
            buf.tab[j] = e;
            buf.eff[j]=0;
            buf.nb++;
            TOF_writeBlock(F,i,&buf);
            continu=0;
        }
        else
        {
            TOF_writeBlock(F,i,&buf);
            i++; j=1;

            if(i <= getHeader(F))
                TOF_readBlock(F,i,&buf);
            else
            {
                buf.nb=0;
                continu=0;
            }
        }
    }

    if(i>getHeader(F))
    {
        buf.tab[1] = e;
        buf.eff[1]=0;
        buf.nb=1;
        TOF_writeBlock(F,i,&buf);
        setHeader(F,i);
    }

    if(!reutilisation)
        setHeader(F, getHeader(F)+1);
}

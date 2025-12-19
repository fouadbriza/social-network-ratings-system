/************************************************************************
 * Interface of the algorithmic model to manipulate a 'TOF' type file   *
 *        - SFSD (File & Data Structures) / 2CP / ESI / 2024 -        *
 ************************************************************************/
 #ifndef TOF_H_INCLUDED




#define TOF_H_INCLUDED
#include <stdio.h>

// maximum block capacity (in number of records)
#define MAXTAB 100

// type of records (adapt to the using context)
typedef struct record{
int student1_id;
int student2_id;
int time;
int rating;
}t_rec;


// type of a data block (and therefore of buffer variables as well)
typedef struct blck {
  t_rec tab[MAXTAB]; // array of records inside a block
  char eff[MAXTAB]; // logical erase indicators ('*' erased / ' ' not erased)
  int nb;
  int link; // number of records inserted in the block
        } t_block;


// the file header (characteristics)
typedef struct hdr {
  int nBlock; // number of blocks in the file (this is also the number of the last block)

} t_header;


// TOF file structure
typedef struct TOFstr {
            FILE *f;    // C stream implementing the file
            t_header h; // the header in main memory
        } t_TOF;


/**** ****************************************** ****/
/**** The functions of the abstract machine      ****/
/**** ****************************************** ****/

// open a TOF file / mode ='N' for a New file and mode ='E' for an Existing file
// returns a pointer to a newly allocated variable of type 't_TOF'
void TOF_open( t_TOF **F, char *fname, char mode );

// close a TOF file :
// the header is first saved at the beginning of the file and the t_TOF variable is freed
void TOF_close( t_TOF *F );

// reading data block number i into variable buf
void TOF_readBlock( t_TOF *F, long i, t_block *buf );

// writing the contents of the variable buf in data block number i
void TOF_writeBlock( t_TOF *F, long i, t_block *buf );

// header modification
void setHeader( t_TOF *F  , long val );

// header value
int getHeader( t_TOF *F  );
void Insert(t_TOF *f, t_rec e,t_TOF *f1);
void seqsearch3(t_TOF *f,t_rec e, int *found, int *i, int *j);
void seqsearch3test(t_TOF *f, t_TOF *f1, int id1, int id2, int *i, int *j);
void recherchee(t_TOF *f,t_TOF *f1,int cle1,int cle2, int *found, int *i, int *j,t_TOF **f2);
void seqsearch3_binary(t_TOF *f, t_rec e,int *found, int *i, int *j);
int cmp3(t_rec a, t_rec b);
void reorganization(t_TOF **f2,t_TOF *f,t_TOF *f1 );
void affichage(t_TOF *f, t_TOF *f1);
void seqsearch(t_TOF *f, t_TOF *f1, int cle1, int cle2, int cle3,int *found, int *i, int *j, int *in_overflow);
void Bulk_Loading(t_TOF *f);
void N_insert(t_TOF *f, t_TOF *f1);
int compare_by_three_fields(const void *a, const void *b);
int count_lines(const char *filename);
void print_friends(t_TOF *f, t_TOF *f1, int id1);
void find_position_by_id1(t_TOF *f, int student1_id,int *found, int *block_index, int *rec_index);
int cmp_id1(int key, t_rec r);
int compareById1forsort(const void *a, const void *b);
#endif // TOF_H_INCLUDED

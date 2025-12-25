/************************************************************************
 * Interface of the algorithmic model to manipulate a 'TOF' type file   *
 *        - SFSD (File & Data Structures) / 2CP / ESI / 2024 -        *
 ************************************************************************/
 #ifndef TOF_H_INCLUDED




#define TOF_H_INCLUDED
#include <stdio.h>

// maximum block capacity (in number of records)
#define MAXTAB 10

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
  int upd[MAXTAB]} t_block;


// the file header (characteristics)
typedef struct hdr {
  int nBlock; // number of blocks in the file (this is also the number of the last block)
  int nBrec;

} t_header;


// TOF file structure
typedef struct TOFstr {
            FILE *f;    // C stream implementing the file
            t_header h; // the header in main memory
        } t_TOF;
        //*****************************************
        /* ================= HEADER ================= */


        typedef struct {
    int nb_blocks;      // number of blocks in file
} l_header;

        typedef struct {
    FILE    *f;          // file stream
    l_header h;          // header in memory
} t_LNOF;

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

int getHeader( t_TOF *F,int n );
void LNOF_open(t_LNOF **F, char *fname, char mode);
void LNOF_close(t_LNOF *F);
void LNOF_readBlock(t_LNOF *F, int i, t_block *buf);
void LNOF_writeBlock(t_LNOF *F, int i, t_block *buf);
int LNOF_getHeader(t_LNOF *F);
void LNOF_setHeader(t_LNOF *F, int val);
int LNOF_allocBlock(t_LNOF *F);
/*
void Insert(t_TOF *f, t_rec e,t_TOF *f1);
void seqsearch3(t_TOF *f,t_rec e, int *found, int *i, int *j);
void seqsearch3test(t_TOF *f, t_TOF *f1, int id1, int id2, int *i, int *j);
void recherchee(t_TOF *f,t_TOF *f1,int cle1,int cle2, int *found, int *i, int *j,t_TOF **f2);
void seqsearch3_binary(t_TOF *f, t_rec e,int *found, int *i, int *j);///////////////////////////////////////////////////////////////////
int cmp3(t_rec a, t_rec b);
void reorganization(t_TOF *f2, t_TOF *f, t_TOF *f1); //menu
void affichage(t_TOF *f, t_TOF *f1); //wa9il mansha9ouhach
void seqsearch(t_TOF *f, t_TOF *f1, int cle1, int cle2, int cle3,int *found, int *i, int *j, int *in_overflow);/////////////////////////
void Bulk_Loading(t_TOF *f);
void N_insert(t_TOF *f, t_TOF *f1);  //in menu
int compare_by_three_fields(const void *a, const void *b);
int count_lines(const char *filename);
void print_friends(t_TOF *f, t_TOF *f1, int id1);  //in menu
void find_position_by_id1(t_TOF *f, int student1_id,int *found, int *block_index, int *rec_index);
int cmp_id1(int key, t_rec r);
int compareById1forsort(const void *a, const void *b);
int cmp3_keys(int id1, int id2, int time, t_rec r);
void seqsearch3_binary_keys(t_TOF *f,int id1, int id2, int time,int *found, int *i, int *j);///////////////////////////////
void update_rating(t_TOF *f, t_TOF *f1); //in menu
int cmp2_keys(int id1, int id2, t_rec r);
void find_first_record(t_TOF *f, int id1, int id2,int *found, int *block_index, int *rec_index);
void print_all_records_id1_id2(t_TOF *f, t_TOF *f1, int id1, int id2);  //in menu
void Inserer(t_TOF **f, t_rec e);
void loading_file(t_TOF *f, t_rec *p, int size);
void display_block(t_TOF *f, long block_index);
void display_file(t_TOF *f);
*/

void Insert(t_TOF *f, t_rec e, t_LNOF *f1);
void seqsearch3(t_TOF *f, t_rec e, int *found, int *i, int *j);
//void seqsearch3test(t_TOF *f, t_LNOF *f1, int id1, int id2, int *i, int *j);
//void recherchee(t_TOF *f, t_LNOF *f1, int cle1, int cle2, int *found, int *i, int *j, t_LNOF **f2);
void seqsearch3_binary(t_TOF *f, t_rec e, int *found, int *i, int *j);
int cmp3(t_rec a, t_rec b);
void reorganization(t_TOF *f2, t_TOF *f, t_LNOF *f1); // menu
void affichage(t_TOF *f, t_LNOF *f1); // wa9il mansha9ouhach
void seqsearch(t_TOF *f, t_LNOF *f1, int cle1, int cle2, int cle3, int *found, int *i, int *j, int *in_overflow);
void Bulk_Loading(t_TOF *f);
void N_insert(t_TOF *f, t_LNOF *f1);  // in menu
int compare_by_three_fields(const void *a, const void *b);
int count_lines(const char *filename);
void print_friends(t_TOF *f, t_LNOF *f1, int id1);  // in menu
void find_position_by_id1(t_TOF *f, int student1_id, int *found, int *block_index, int *rec_index);
int cmp_id1(int key, t_rec r);
int compareById1forsort(const void *a, const void *b);
int cmp3_keys(int id1, int id2, int time, t_rec r);
void seqsearch3_binary_keys(t_TOF *f, int id1, int id2, int time, int *found, int *i, int *j);
void update_rating(t_TOF *f, t_LNOF *f1); // in menu
int cmp2_keys(int id1, int id2, t_rec r);
void find_first_record(t_TOF *f, int id1, int id2, int *found, int *block_index, int *rec_index);
void print_all_records_id1_id2(t_TOF *f, t_LNOF *f1, int id1, int id2);  // in menu
void Inserer(t_TOF **f, t_rec e);
void loading_file(t_TOF *f, t_rec *p, int size);
void display_block(t_TOF *f, long block_index);
void display_file(t_TOF *f);

#endif // TOF_H_INCLUDED

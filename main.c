/**************************************************************************
 * Example of a C program manipulating a TOF file / SFSD 2024 / Hidouci   *
 **************************************************************************/



#include "TOF.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// the TOF file
t_TOF *f = NULL;
t_TOF *f1 = NULL;


t_block buf,buf1;  // the buffer in main memory








int main(){


TOF_open(&f,"fouad_file",'N');
TOF_open(&f1,"LnOF",'N');
if (f == NULL) {
    printf("Error: file not opened!\n");
}




Bulk_Loading(f);

N_insert(f,f1);
affichage(f,f1);
int i, j, found, in_overflow;
//TOF_readBlock(f,1,&buf);
//printf("%d\n",getHeader(f));


//seqsearch(f, f1, buf1.tab[1].student1_id, buf1.tab[1].student2_id, buf1.tab[1].time, &found, &i, &j, &in_overflow);



//printf("%d",buf.tab[74].student1_id);
//printf("%d",buf.nb);
//int found,i,j;

//seqsearch3(buf.tab[5],&found,&i,&j);
//if(found=1){
//printf("%d,%d",i,j);
//TOF_readBlock(f,4,&buf);
//t_rec x=buf.tab[4];
//printf("%d",x.time);
//printf("%d",buf.tab[4].time);
//printf("%d",buf.nb);
/*FILE *fi;
t_rec e;
int number=count_lines("DAKICHE.txt");
fi=fopen("DAKICHE.txt","r");

*/

/*int i,j;
i=0;
j=0;
t_TOF*f2;
int trouve;
TOF_readBlock(f,12,&buf);
recherchee(f,f1,29,4,&trouve,&i,&j,&f2);
printf("%d,%d\n",i,j);
printf("%d",trouve);
*/
/*TOF_readBlock(f,20,&buf);
int i,j,trouve;

t_rec e;
e.student1_id=6;
e.student2_id=28;
e.time=916959600;
e.rating=0;
TOF_readBlock(f,3,&buf);
seqsearch3(f,e,&trouve,&i,&j);
printf("%d,%d,%d\n",trouve,i,j);
printf("%d,%d,%d,%d\n",buf.tab[57].student1_id,buf.tab[57].student2_id,buf.tab[57].time);
printf("%d,%d,%d,%d\n",buf.tab[58].student1_id,buf.tab[58].student2_id,buf.tab[58].time);
*/
//void seqsearch3fouad(t_TOF *f, int id1, int id2, t_TOF *f1,int *found,int *i_block, int *j_rec,int *is_lnof)
int is_lnof;

//seqsearch3fouad(f, 28, 10, f1, &found, &i, &j, &is_lnof);

print_friends(f,f1,2);




TOF_close(f);
TOF_close(f1);




}


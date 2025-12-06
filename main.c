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

t_block buf;  // the buffer in main memory


int count_lines(const char *filename) {
    FILE *fp = fopen(filename, "rb");   // <-- rb = read binary
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    long lines = 0;

    int byte;

    while ((byte = fgetc(fp)) != EOF) {
        if (byte == 0x0A) {     // newline in binary
            lines++;
        }
    }

    fclose(fp);
    return lines;

}
void seqsearch3(t_rec e, int *found, int *i, int *j)
{
    int n = getHeader(f); // nombre de blocs
    *i = 1;                // commencer au premier bloc
    *found = 0;
    int stop = 0;
    t_block buf;

    while (*i <= n && !(*found) && !stop) {
        TOF_readBlock(f, *i, &buf);
        *j = 0;

        // recherche interne dans le bloc
        while (*j < buf.nb && !(*found) && !stop) {
            if (buf.tab[*j].student1_id > e.student1_id ||
               (buf.tab[*j].student1_id == e.student1_id && buf.tab[*j].student2_id > e.student2_id)) {
                // position idéale pour insertion
                stop = 1;
            }
            else if (buf.tab[*j].student1_id == e.student1_id &&
                     buf.tab[*j].student2_id == e.student2_id) {
                *found = 1; // trouvé le couple (cle1, cle2)
            }
            else {
                (*j)++;
            }
        }

        if (!(*found) && !stop) {
            (*i)++;
            *j = 0;
        }
    }

    // si non trouvé et fin de fichier
    if (!(*found) && stop == 0) {
        *i = n;
        TOF_readBlock(f, *i, &buf);
        *j = buf.nb; // insertion à la fin du dernier bloc
    }
}

/*
int compare_by_student1(const void *a, const void *b) {
    t_rec *rec1 = (t_rec*)a;
    t_rec *rec2 = (t_rec*)b;
    return rec1->student1_id - rec2->student1_id;  // ascending order
}
*/
int compare_by_two_fields(const void *a, const void *b) {
    t_rec *rec1 = (t_rec*)a;
    t_rec *rec2 = (t_rec*)b;

    // Primary key: student1_id
    if(rec1->student1_id != rec2->student1_id)
        return rec1->student1_id - rec2->student1_id;

    // Secondary key: student2_id
    return rec1->student2_id - rec2->student2_id;
}
void Bulk_Loading(t_TOF *f){
int number=count_lines("DAKICHE.txt");


    t_rec *p = (t_rec*)malloc((number/2) * sizeof(t_rec));
    if(!p) {
        printf("Memory allocation failed\n");

    }
FILE *fi;
fi=fopen("DAKICHE.txt","r");

if (fi == NULL) {
    perror("Error opening file"); // prints system error message
}
    for(int i = 0; i < number/2; i++) {

        fscanf(fi,"%d %d %d %d", &p[i].student1_id, &p[i].student2_id, &p[i].rating, &p[i].time);

    }
qsort(p, number/2, sizeof(t_rec), compare_by_two_fields);


int i=1;
int j=0;
int k=0;
while(k<(number/2)){
if(j<=0.75*MAXTAB){
buf.tab[j].student1_id=p[k].student1_id;
buf.tab[j].student2_id=p[k].student2_id;
buf.tab[j].rating=p[k].rating;
buf.tab[j].time=p[k].time ;


buf.eff[0]=0;

j++;

}
else{
buf.nb=j-1;
TOF_writeBlock(f,i,&buf);
i++;
buf.tab[0].student1_id=p[k].student1_id;
buf.tab[0].student2_id=p[k].student2_id;
buf.tab[0].rating=p[k].rating;
buf.tab[0].time=p[k].time;


buf.eff[0]=0;

j=1;
}
k++;
}
buf.nb=j-1;
TOF_writeBlock(f,i,&buf);

setHeader(f,i);


}



int main(){


TOF_open(&f,"fouad_file",'N');
if (f == NULL) {
    printf("Error: file not opened!\n");
}
Bulk_Loading(f);
TOF_readBlock(f,1,&buf);
int found,i,j;
seqsearch3(buf.tab[5],&found,&i,&j);
if(found=1){
printf("%d,%d",i,j);
}
//printf("%d",getHeader(f));
//printf("%d",buf.tab[4].time);




TOF_close(f);

}

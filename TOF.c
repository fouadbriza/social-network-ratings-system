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


void Insert(t_TOF *f, t_rec e,t_TOF *f1)
{
    int found, i, j, in_overflow;
    t_block buf, buf1;

    // Vérifier si le record existe déjà
    seqsearch3_binary(f,  e,&found, &i, &j);

    if (found) {
        printf("Record already exists\n");
        return; // éviter les doublons
    }

    // Lire le bloc principal où l'insertion doit se faire
    TOF_readBlock(f, i, &buf);

    // 1️⃣ Bloc principal non plein
    if (buf.nb < MAXTAB) {
        if (j > buf.nb) j = buf.nb; // sécuriser l’index
        for (int k = buf.nb; k > j; k--) {
            buf.tab[k] = buf.tab[k-1]; // décaler
        }
        buf.tab[j] = e;
        buf.nb++;
        TOF_writeBlock(f, i, &buf);
        return;
    }

    // 2️⃣ Bloc principal plein → aller vers overflow
    if (buf.link == 0) {
        // Créer un nouveau bloc overflow
        int k = getHeader(f1) + 1;
        buf.link = k;
        TOF_writeBlock(f, i, &buf);

        buf1.tab[0] = e;
        buf1.nb = 1;
        buf1.link = 0;
        TOF_writeBlock(f1, k, &buf1);
        setHeader(f1, k);
        return;
    }

    // 3️⃣ Parcourir la liste d’overflow pour insérer
    int link = buf.link;
    int last = link;
    while (link != 0) {
        TOF_readBlock(f1, link, &buf1);
        if (buf1.nb < MAXTAB) {
            buf1.tab[buf1.nb] = e;
            buf1.nb++;
            TOF_writeBlock(f1, link, &buf1);
            return;
        }
        last = link;
        link = buf1.link;
    }

    // 4️⃣ Tous les blocs overflow pleins → créer un nouveau bloc
    int index = getHeader(f1) + 1;
    buf1.tab[0] = e;
    buf1.nb = 1;
    buf1.link = 0;
    TOF_writeBlock(f1, index, &buf1);

    // Mettre à jour le lien du dernier bloc overflow
    TOF_readBlock(f1, last, &buf1);
    buf1.link = index;
    TOF_writeBlock(f1, last, &buf1);
    setHeader(f1, index);
}
void seqsearch3(t_TOF *f, t_rec e, int *found, int *i, int *j)
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
            t_rec current = buf.tab[*j];

            // Comparaison lexicographique des trois clés
            if (current.student1_id > e.student1_id ||
                (current.student1_id == e.student1_id && current.student2_id > e.student2_id) ||
                (current.student1_id == e.student1_id && current.student2_id == e.student2_id && current.time > e.time)) {
                // position idéale pour insertion
                stop = 1;
            }
            else if (current.student1_id == e.student1_id &&
                     current.student2_id == e.student2_id &&
                     current.time == e.time) {
                *found = 1; // trouvé le triple clé
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

void seqsearch3test(t_TOF *f, t_TOF *f1, int id1, int id2, int *i, int *j) {
    int n = getHeader(f); // nombre de blocs
    *i = 1;               // commencer au premier bloc
    int found = 0;
    t_block buf, buf1;

    while (*i <= n) {
        TOF_readBlock(f, *i, &buf);
        *j = 0;

        while (*j < buf.nb) {
            if ((buf.tab[*j].student1_id == id1) &&
                (buf.tab[*j].student2_id == id2))
                {
                printf("Student %d rated Student %d : %d\n",
                       buf.tab[*j].student1_id,
                       buf.tab[*j].student2_id,
                       buf.tab[*j].rating);
                found = 1;
            }
            (*j)++;
        }
                        printf("%d and the link is %d:",found,buf.link);


        if (found==1 && buf.link != -1) {
                found =0;


            TOF_readBlock(f1, buf.link, &buf1);//probleme fhad la ligne chouf m3a f1 hata ramzi yefriha
                    printf("shiit__");

            int first = 1;

            while (buf1.link != -1) {
                *j = 0;
                while (first || *j < buf1.nb) {
                    first = 0;
                    if ((buf1.tab[*j].student1_id == id1) &&
                        (buf1.tab[*j].student2_id == id2)) {
                        printf("Student %d rated Student %d : %d\n",
                               buf1.tab[*j].student1_id,
                               buf1.tab[*j].student2_id,
                               buf1.tab[*j].rating);
                    }
                    (*j)++;
                }
                TOF_readBlock(f1, buf1.link, &buf1);
            }
        }

        (*i)++;
        *j = 0;
                        printf("hyyyy__");

    }
}









void recherchee(t_TOF *f, t_TOF *f1, int cle1, int cle2,
                 int *found, int *i, int *j, t_TOF **f2)
{
    int n = getHeader(f);   // nombre de blocs dans TOF
    *found = 0;
    *i = 1;
    *f2 = f;

    t_block buf, buf1;
    int stop = 0;   // booléen pour arrêter la recherche
    int searchDone = 0; // booléen global pour fin de recherche

    while (*i <= n && !searchDone) {
        TOF_readBlock(f, *i, &buf);
        *j = 0;

        // Recherche interne dans le bloc TOF
        while (*j < buf.nb && !(*found) && !stop) {
            if (buf.tab[*j].student1_id > cle1 ||
                (buf.tab[*j].student1_id == cle1 && buf.tab[*j].student2_id > cle2)) {
                stop = 1; // position idéale pour insertion
            }
            else if (buf.tab[*j].student1_id == cle1 &&
                     buf.tab[*j].student2_id == cle2) {
                *found = 1;
                *f2 = f;  // trouvé dans TOF
            }
            else {
                (*j)++;
            }
        }

        // Vérification des blocs liés LnOF pour ce bloc TOF
        int link = buf.link;
        while (link != 0 && !(*found)) {
            TOF_readBlock(f1, link, &buf1);
            *j = 0;

            int localStop = 0;
            while (*j < buf1.nb && !(*found) && !localStop) {
                if (buf1.tab[*j].student1_id > cle1 ||
                    (buf1.tab[*j].student1_id == cle1 && buf1.tab[*j].student2_id > cle2)) {
                    localStop = 1; // insertion dans ce bloc
                }
                else if (buf1.tab[*j].student1_id == cle1 &&
                         buf1.tab[*j].student2_id == cle2) {
                    *found = 1;
                    *f2 = f1;  // trouvé dans LnOF
                    *i = link; // bloc LnOF
                }
                else {
                    (*j)++;
                }
            }

            if (!(*found)) {
                link = buf1.link; // passer au bloc LnOF suivant
            } else {
                link = 0;
            }
        }

        // Passage au bloc TOF suivant si non trouvé et stop non déclenché
        if (!(*found) && !stop) {
            (*i)++;
            *j = 0;
        } else {
            searchDone = 1; // fin de recherche
        }
    }

    // Si non trouvé : position d’insertion
    if (!(*found)) {
        *f2 = f;  // insertion se fera dans TOF
        if (!stop) {
            // insertion à la fin du dernier bloc TOF
            *i = n;
            TOF_readBlock(f, *i, &buf);
            *j = buf.nb;
        }
        // si stop == 1, *i et *j contiennent déjà la position idéale
    }
}
void seqsearch3_binary(t_TOF *f, t_rec e,
                       int *found, int *i, int *j)
{
    int n = getHeader(f);
    *found = 0;

    t_block buf;

    int low = 1, high = n;
    int block_pos = n;

    // BINARY SEARCH ON BLOCKS
    while (low <= high)
    {
        int mid = (low + high) / 2;

        TOF_readBlock(f, mid, &buf);

        if (buf.nb == 0) {
            high = mid - 1;
            continue;
        }

        t_rec first = buf.tab[0];
        t_rec last  = buf.tab[buf.nb - 1];

        int cmpFirst = cmp3(e, first);
        int cmpLast  = cmp3(e, last);

        if (cmpFirst < 0) {
            block_pos = mid;
            high = mid - 1;
        }
        else if (cmpLast > 0) {
            low = mid + 1;
        }
        else {
            block_pos = mid;
            break;
        }
    }

    *i = block_pos;
    TOF_readBlock(f, *i, &buf);

    // ---------------------------------
    // BINARY SEARCH INSIDE THE BLOCK
    // ---------------------------------

    low  = 0;
    high = buf.nb - 1;
    *j   = buf.nb;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        int cmp = cmp3(e, buf.tab[mid]);

        if (cmp == 0) {
            *found = 1;
            *j = mid;
            return;
        }
        else if (cmp < 0) {
            *j = mid;
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }
}
int cmp3(t_rec a, t_rec b)
{
    if (a.student1_id != b.student1_id)
        return (a.student1_id < b.student1_id) ? -1 : 1;

    if (a.student2_id != b.student2_id)
        return (a.student2_id < b.student2_id) ? -1 : 1;

    if (a.time != b.time)
        return (a.time < b.time) ? -1 : 1;

    return 0;
}



/*void reorganization(t_TOF **f2,t_TOF *f,t_TOF *f1 )
{
    t_block buf,buf1;
 int index = 1;
while (index <= getHeader(f)) {
    // 1️⃣ Read main block
    TOF_readBlock(f, index, &buf);

    // Insert all records from main block into f2
    for (int j = 0; j < buf.nb; j++) {
        Inserer(*&f2, buf.tab[j]);
    }

    // 2️⃣ Read overflow blocks linked to this main block
    int link = buf.link;
    while (link != 0) {
        TOF_readBlock(f1, link, &buf1);

        for (int k = 0; k < buf1.nb; k++) {
            Inserer(*&f2, buf1.tab[k]);
        }

        link = buf1.link; // move to next overflow block
    }

    index++;

}
}

*/




void affichage(t_TOF *f, t_TOF *f1)
{
    int n = getHeader(f);
    t_block buf, buf1;

    for (int i = 1; i <= n; i++) {
        TOF_readBlock(f, i, &buf);

        printf("=== Bloc principal %d ===\n", i);
        for (int j = 0; j < buf.nb; j++) {
            printf("[%d,%d,%d,%d]\n",
                   buf.tab[j].student1_id,
                   buf.tab[j].student2_id,
                   buf.tab[j].rating,
                   buf.tab[j].time);
        }

        // Affichage de la liste d'overflow si elle existe
        int linkk = buf.link;
        while (linkk != 0) {
            TOF_readBlock(f1, linkk, &buf1);
            printf("--- Bloc overflow %d lié au bloc principal %d ---\n", linkk, i);
            for (int j = 0; j < buf1.nb; j++) {
                printf("[%d,%d,%d,%d]\n",
                       buf1.tab[j].student1_id,
                       buf1.tab[j].student2_id,
                       buf1.tab[j].rating,
                       buf1.tab[j].time);
            }
            linkk = buf1.link;
        }

        printf("--------------------------------------------------------------\n");
    }
}









void seqsearch(t_TOF *f, t_TOF *f1, int cle1, int cle2, int cle3,
                int *found, int *i, int *j, int *in_overflow)
{
    int n = getHeader(f); // nombre de blocs du fichier principal
    *found = 0;
    *in_overflow = 0;
    t_block buf, buf1;

    for (*i = 1; *i <= n && !(*found); (*i)++) {
        // 1️⃣ Recherche dans le bloc principal
        TOF_readBlock(f, *i, &buf);
        for (*j = 0; *j < buf.nb && !(*found); (*j)++) {
            t_rec current = buf.tab[*j];
            if (current.student1_id == cle1 &&
                current.student2_id == cle2 &&
                current.time == cle3) {
                *found = 1;
                *in_overflow = 0;
                return;
            }
        }

        // 2️⃣ Recherche dans la liste d'overflow liée à ce bloc
        int link = buf.link;
        while (link != 0 && !(*found)) {
            TOF_readBlock(f1, link, &buf1);
            for (int j1 = 0; j1 < buf1.nb && !(*found); j1++) {
                t_rec current = buf1.tab[j1];
                if (current.student1_id == cle1 &&
                    current.student2_id == cle2 &&
                    current.time == cle3) {
                    *found = 1;
                    *in_overflow = 1;
                    *i = link;  // bloc dans le fichier overflow
                    *j = j1;    // index dans ce bloc
                    return;
                }
            }
            link = buf1.link; // passer au bloc suivant de la liste
        }
    }

    // Si pas trouvé
    *i = -1;
    *j = -1;
}






void Bulk_Loading(t_TOF *f){
int number=count_lines("DAKICHE.txt");
t_block buf,buf1;

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
qsort(p, number/2, sizeof(t_rec), compare_by_three_fields);


int i=1;
int j=0;
int k=0;
while(k<(number/2)){
if(j<0.75*MAXTAB){
buf.tab[j].student1_id=p[k].student1_id;
buf.tab[j].student2_id=p[k].student2_id;
buf.tab[j].rating=p[k].rating;
buf.tab[j].time=p[k].time ;


buf.eff[0]=0;

j++;

}
else{
buf.nb=j;
buf.link=0;
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
buf.nb=j;
TOF_writeBlock(f,i,&buf);

setHeader(f,i);


}








void N_insert(t_TOF *f, t_TOF *f1)
{
    printf("GIVE ME NUMBER OF RECORD TO INSERT: ");
    int N;
    scanf("%d", &N);  // FIXED: add &

    int number = count_lines("DAKICHE.txt");
    FILE *fi = fopen("DAKICHE.txt", "r");
    if (fi == NULL) {
        perror("Error opening file");
        return;
    }

    t_rec e;

    // Read and optionally insert first half if needed
    for (int i = 0; i < number / 2; i++) {
        fscanf(fi, "%d %d %d %d", &e.student1_id, &e.student2_id, &e.rating, &e.time); // FIXED
        // If needed, Insert(f, e, f1);
    }

    // Read and insert next N records
    for (int i = number / 2; i < number / 2 + N; i++) {
        fscanf(fi, "%d %d %d %d", &e.student1_id, &e.student2_id, &e.rating, &e.time); // FIXED
        Insert(f, e, f1);
    }

    fclose(fi);
}




int compare_by_three_fields(const void *a, const void *b) {
    const t_rec *rec1 = (const t_rec*)a;
    const t_rec *rec2 = (const t_rec*)b;

    // 1. Primary key: student1_id
    if (rec1->student1_id != rec2->student1_id)
        return rec1->student1_id - rec2->student1_id;

    // 2. Secondary key: student2_id
    if (rec1->student2_id != rec2->student2_id)
        return rec1->student2_id - rec2->student2_id;

    // 3. Tertiary key: time
    // (Oldest first)
    if (rec1->time != rec2->time)
        return rec1->time - rec2->time;

    return 0; // completely equal
}






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




void print_friends(t_TOF *f, t_TOF *f1, int id1)
{
    int found, i, j,u=0;
    t_block buf, buf1;
    int stop = 0;

    t_rec array[50];

    // Find the first record with id1
    find_position_by_id1(f, id1, &found, &i, &j);

    if (!found) {
        printf("No records found for student %d\n", id1);
        return; // Do not check any main or overflow block
    }

    int n = getHeader(f);

    while (!stop && i <= n) {
        // Read main block
        TOF_readBlock(f, i, &buf);

        // Scan main block
        while (j < buf.nb && !stop) {
            if (buf.tab[j].student1_id == id1) {
                array[u]=buf.tab[j];
                u++;
                j++;
            } else if (buf.tab[j].student1_id > id1) {
                stop = 1; // Stop scanning, no more id1
            } else {
                j++;
            }
        }

        // Scan overflow list
        int link = buf.link;
        while (link != 0 && !stop) {
            TOF_readBlock(f1, link, &buf1);

            for (int k = 0; k < buf1.nb && !stop; k++) {
                if (buf1.tab[k].student1_id == id1) {
                        array[u]=buf1.tab[j];
                            u++;
                } else if (buf1.tab[k].student1_id > id1) {
                    stop = 1;
                }
            }

            link = buf1.link;
        }

        // Move to next main block
        j = 0;
        i++;
    }
        qsort(array, u, sizeof(t_rec), compareById1forsort);

printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");

if (u > 0) {
    int prevFriend = array[0].student2_id;

    // Walk through all records
    for (int i = 1; i < u; i++) {
        if (array[i].student2_id != prevFriend) {
            // Print the last record of the previous friend
            t_rec last = array[i-1];
            printf("Student %d is friend of %d, Rating=%d, Time=%d\n",
                   last.student2_id, last.student1_id, last.rating, last.time);

            // Update to new friend group
            prevFriend = array[i].student2_id;
        }
    }

    // Don’t forget to print the last group’s last record
    t_rec last = array[u-1];
    printf("Student %d is friend of %d, Rating=%d, Time=%d\n",
           last.student2_id, last.student1_id, last.rating, last.time);
}

}




void find_position_by_id1(t_TOF *f, int student1_id,
                          int *found, int *block_index, int *rec_index)
{
    int n = getHeader(f);
    t_block buf;

    *found = 0;
    *block_index = -1;
    *rec_index = -1;

    int low = 1, high = n;
    int candidate_block = n; // default: last block
    int candidate_index = 0;

    // ----- Binary search over blocks -----
    while (low <= high) {
        int mid = (low + high) / 2;
        TOF_readBlock(f, mid, &buf);

        if (buf.nb == 0) { high = mid - 1; continue; }

        t_rec first = buf.tab[0];
        t_rec last = buf.tab[buf.nb - 1];

        if (cmp_id1(student1_id, first) < 0) {
            candidate_block = mid;
            high = mid - 1;
        }
        else if (cmp_id1(student1_id, last) > 0) {
            low = mid + 1;
        }
        else {
            candidate_block = mid;
            break; // this block may contain the record
        }
    }

    *block_index = candidate_block;
    TOF_readBlock(f, candidate_block, &buf);

    // ----- Binary search inside the block -----
    int l = 0, r = buf.nb - 1;
    candidate_index = buf.nb; // default: insert at end

    while (l <= r) {
        int m = (l + r) / 2;
        int cmp = cmp_id1(student1_id, buf.tab[m]);

        if (cmp == 0) {
            candidate_index = m;
            *found = 1;
            r = m - 1; // search for first occurrence
        }
        else if (cmp < 0) {
            candidate_index = m;
            r = m - 1;
        }
        else {
            l = m + 1;
        }
    }

    *rec_index = candidate_index;
}


int cmp_id1(int key, t_rec r) {
    if (key < r.student1_id) return -1;
    if (key > r.student1_id) return 1;
    return 0;
}

int compareById1forsort(const void *a, const void *b) {
    const t_rec *recA = (const t_rec*)a;
    const t_rec *recB = (const t_rec*)b;

    if (recA->student1_id < recB->student1_id) return -1;
    if (recA->student1_id > recB->student1_id) return 1;
    return 0;
}


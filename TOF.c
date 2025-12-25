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
int getHeader( t_TOF *F,int n )
{   if (n==1) {
      return F->h.nBlock;
    } else {
    return F->h.nBrec;
    }


}
//MACHINE ABSTRAR OF LNOF


/* ================= OPEN FILE ================= */
void LNOF_open(t_LNOF **F, char *fname, char mode) {
    t_block buf; // type of the block in LNOF

    *F = (t_LNOF*) malloc(sizeof(t_LNOF));
    if (!*F) {
        perror("Memory allocation failed for LNOF");
        exit(EXIT_FAILURE);
    }

    if (mode == 'E' || mode == 'e') {
        // Open an existing file
        (*F)->f = fopen(fname, "rb+");
        if (!(*F)->f) {
            perror("LNOF_open: cannot open file");
            exit(EXIT_FAILURE);
        }
        // Read the header from the file
        fread(&((*F)->h), sizeof(l_header), 1, (*F)->f);
    } else {
        // Create a new file
        (*F)->f = fopen(fname, "wb+");
        if (!(*F)->f) {
            perror("LNOF_open: cannot create file");
            exit(EXIT_FAILURE);
        }
        // Initialize the header
        (*F)->h.nb_blocks = 1; // make sure l_header has this field
        // Write the header to the file
        fwrite(&((*F)->h), sizeof(l_header), 1, (*F)->f);
        // Initialize the first block
        buf.nb = 0;      // number of records in the block
        buf.link = 0;    // no next block
        fwrite(&buf, sizeof(t_block), 1, (*F)->f);
    }
}

/* ================= CLOSE FILE ================= */
void LNOF_close(t_LNOF *F) {
    // save header first
    fseek(F->f, 0L, SEEK_SET);
    fwrite(&F->h, sizeof(l_header), 1, F->f);
    fclose(F->f);
    free(F);
}

/* ================= READ BLOCK ================= */
void LNOF_readBlock(t_LNOF *F, int i, t_block *buf) {
    fseek(F->f, sizeof(l_header) + (i-1) * sizeof(t_block), SEEK_SET);
    fread(buf, sizeof(t_block), 1, F->f);
}

/* ================= WRITE BLOCK ================= */
void LNOF_writeBlock(t_LNOF *F, int i, t_block *buf) {
    fseek(F->f, sizeof(l_header) + (i-1) * sizeof(t_block), SEEK_SET);
    fwrite(buf, sizeof(t_block), 1, F->f);
}

/* ================= GET HEADER ================= */
int LNOF_getHeader(t_LNOF *F) {
    return F->h.nb_blocks;
}

/* ================= SET HEADER ================= */
void  LNOF_setHeader(t_LNOF *F, int val) {
    F->h.nb_blocks = val;
}

/* ================= ALLOCATE BLOCK ================= */
int LNOF_allocBlock(t_LNOF *F) {
    t_block buf;
    long index = F->h.nb_blocks + 1;

    // update header
    F->h.nb_blocks = index;

    return index;
}


void Insert(t_TOF *f, t_rec e, t_LNOF *f1)
{
    int found, i, j;
    t_block buf, buf1;

    // 1️⃣ Vérifier si le record existe déjà dans le TOF principal
    seqsearch3_binary(f, e, &found, &i, &j);
    if (found) {
        printf("Record already exists\n");
        return;
    }

    // 2️⃣ Lire le bloc principal où l'insertion doit se faire
    TOF_readBlock(f, i, &buf);

    // --- Bloc principal non plein ---
    if (buf.nb < MAXTAB) {
        if (j > buf.nb) j = buf.nb;
        for (int k = buf.nb; k > j; k--) {
            buf.tab[k] = buf.tab[k-1];
            buf.upd[k] = 0;
        }
        buf.tab[j] = e;
        buf.nb++;
        TOF_writeBlock(f, i, &buf);
        return;
    }

    // --- Bloc principal plein → aller vers overflow ---
    if (buf.link == 0) {
        // Créer un nouveau bloc overflow via LNOF
        long newIndex = LNOF_allocBlock(f1);  // nouveau bloc
        buf.link = newIndex;
        TOF_writeBlock(f, i, &buf);

        buf1.tab[0] = e;
        buf1.nb = 1;
        buf1.link = 0;
        for(int u=0; u<MAXTAB; u++) buf1.upd[u]=0;
        LNOF_writeBlock(f1, newIndex, &buf1);
        return;
    }

    // --- Parcourir la liste d’overflow pour insérer ---
    long link = buf.link;
    long last = link;
    while (link != 0) {
        LNOF_readBlock(f1, link, &buf1);
        if (buf1.nb < MAXTAB) {
            buf1.tab[buf1.nb] = e;
            buf1.upd[buf1.nb] = 0;
            buf1.nb++;
            LNOF_writeBlock(f1, link, &buf1);
            return;
        }
        last = link;
        link = buf1.link;
    }

    // --- Tous les blocs overflow pleins → créer un nouveau bloc ---
    long newIndex = LNOF_allocBlock(f1);
    buf1.tab[0] = e;
    buf1.nb = 1;
    buf1.link = 0;
    for(int u=0; u<MAXTAB; u++) buf1.upd[u]=0;
    LNOF_writeBlock(f1, newIndex, &buf1);

    // Mettre à jour le lien du dernier bloc overflow
    LNOF_readBlock(f1, last, &buf1);
    buf1.link = newIndex;
    LNOF_writeBlock(f1, last, &buf1);
}
/*void Insert(t_TOF *f, t_rec e, t_LNOF *f1)
{
    int found, i, j;
    t_block buf, buf1;

    // 1️⃣ Vérifier si le record existe déjà dans le TOF principal
    seqsearch3_binary(f, e, &found, &i, &j);
    if (found) {
        printf("Record already exists\n");
        return;
    }

    // 2️⃣ Lire le bloc principal où l'insertion doit se faire
    TOF_readBlock(f, i, &buf);

    // --- Bloc principal non plein ---
    if (buf.nb < MAXTAB) {
        if (j > buf.nb) j = buf.nb;
        for (int k = buf.nb; k > j; k--) {
            buf.tab[k] = buf.tab[k-1];
            buf.upd[k] = 0;
        }
        buf.tab[j] = e;
        buf.nb++;
        TOF_writeBlock(f, i, &buf);
        return;
    }

    // --- Bloc principal plein → aller vers overflow ---
    if (buf.link == 0) {
        // Créer un nouveau bloc overflow via LNOF
        long newIndex = LNOF_allocBlock(f1);  // nouveau bloc
        buf.link = newIndex;
        TOF_writeBlock(f, i, &buf);

        buf1.tab[0] = e;
        buf1.nb = 1;
        buf1.link = 0;
        for(int u=0; u<MAXTAB; u++) buf1.upd[u]=0;
        LNOF_writeBlock(f1, newIndex, &buf1);
        return;
    }

    // --- Parcourir la liste d’overflow pour insérer ---
    long link = buf.link;
    long last = link;
    while (link != 0) {
        LNOF_readBlock(f1, link, &buf1);
        if (buf1.nb < MAXTAB) {
            buf1.tab[buf1.nb] = e;
            buf1.upd[buf1.nb] = 0;
            buf1.nb++;
            LNOF_writeBlock(f1, link, &buf1);
            return;
        }
        last = link;
        link = buf1.link;
    }

    // --- Tous les blocs overflow pleins → créer un nouveau bloc ---
    long newIndex = LNOF_allocBlock(f1);
    buf1.tab[0] = e;
    buf1.nb = 1;
    buf1.link = 0;
    for(int u=0; u<MAXTAB; u++) buf1.upd[u]=0;
    LNOF_writeBlock(f1, newIndex, &buf1);

    // Mettre à jour le lien du dernier bloc overflow
    LNOF_readBlock(f1, last, &buf1);
    buf1.link = newIndex;
    LNOF_writeBlock(f1, last, &buf1);
}
*/
void seqsearch3(t_TOF *f, t_rec e, int *found, int *i, int *j)
{
    int n = getHeader(f,1); // nombre de blocs
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
/*void seqsearch3test(t_TOF *f, t_LNOF *f1, int id1, int id2) {
    int n = getHeader(f); // nombre de blocs TOF
    t_block buf;
    t_block buf1;

    for (int i = 1; i <= n; i++) {
        TOF_readBlock(f, i, &buf);

        // recherche dans le bloc principal
        for (int j = 0; j < buf.nb; j++) {
            if (buf.tab[j].student1_id == id1 && buf.tab[j].student2_id == id2) {
                printf("Main Block %d: Student %d rated Student %d : %d\n",
                       i, buf.tab[j].student1_id, buf.tab[j].student2_id, buf.tab[j].rating);
            }
        }

        // recherche dans les blocs overflow liés
        long link = buf.link;
        while (link != 0) {
            LNOF_readBlock(f1, link, &buf1);
            for (int k = 0; k < buf1.nb; k++) {
                if (buf1.tab[k].student1_id == id1 && buf1.tab[k].student2_id == id2) {
                    printf("Overflow Block %ld: Student %d rated Student %d : %d\n",
                           link, buf1.tab[k].student1_id, buf1.tab[k].student2_id, buf1.tab[k].rating);
                }
            }
            link = buf1.link; // passer au bloc overflow suivant
        }
    }
}
*/
void seqsearch3_binary(t_TOF *f, t_rec e,
                       int *found, int *i, int *j)
{
    int n = getHeader(f,1);
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

        if (cmp == 0 && buf.upd[mid]==0) {
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
void reorganization(t_TOF *f2, t_TOF *f, t_LNOF *f1)
{
    int index = 1;

    while (index <= getHeader(f,1)) {

        int number = 0;
        t_block buf, buf1;

        /* ===== COUNT BLOCKS ===== */
        TOF_readBlock(f, index, &buf);
        number++;

        int link = buf.link;
        while (link != 0) {
            LNOF_readBlock(f1, link, &buf1); // استخدم LNOF هنا
            number++;
            link = buf1.link;
        }

        /* ===== ALLOCATE HEAP MEMORY (NOT STACK) ===== */
        t_rec *p = (t_rec*)malloc((number+1) * MAXTAB * sizeof(t_rec));
        if (!p) {
            printf("Memory allocation failed\n");
            return;
        }

        int h = 0;

        /* ===== MAIN BLOCK ===== */
        for (int j = 0; j < buf.nb; j++) {
            if (buf.upd[j] == 0) {
                p[h] = buf.tab[j];
                h++;
            }
        }

        /* ===== OVERFLOW BLOCKS ===== */
        link = buf.link;
        while (link != 0) {
            LNOF_readBlock(f1, link, &buf1); // تابع القراءة في LNOF
            for (int k = 0; k < buf1.nb; k++) {
                p[h] = buf1.tab[k];
                h++;
            }
            link = buf1.link;
        }

        /* ===== SORT ===== */
        qsort(p, h, sizeof(t_rec), compare_by_three_fields);

        /* ===== RELOAD ===== */
        loading_file(f2, p, h);

        free(p);
        index++;
    }
}
void affichage(t_TOF *f, t_LNOF *f1)
{
    int n = getHeader(f,1);
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
            LNOF_readBlock(f1, linkk, &buf1); // استخدم LNOF هنا
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
void seqsearch(t_TOF *f, t_LNOF *f1,
               int cle1, int cle2, int cle3,
               int *found, int *i, int *j, int *in_overflow)
{
    int trouve = 0;
    int index1 = 0, index2 = 0;

    *found = 0;
    *in_overflow = 0;

    // --- MAIN FILE BINARY SEARCH ---
    seqsearch3_binary_keys(f, cle1, cle2, cle3,
                           &trouve, &index1, &index2);

    if (trouve) {
        *found = 1;
        *i = index1;
        *j = index2;
        return;
    }

    // --- NOT FOUND: SEARCH IN OVERFLOW ---
    t_block buf, buf1;
    TOF_readBlock(f, index1, &buf);

    int link = buf.link;
    int stop = 0;

    while (link != 0 && !stop)
    {
        LNOF_readBlock(f1, link, &buf1);  // <-- استخدم LNOF هنا

        for (int j1 = 0; j1 < buf1.nb && !stop; j1++)
        {
            t_rec cur = buf1.tab[j1];

            if (cur.student1_id == cle1 &&
                cur.student2_id == cle2 &&
                cur.time == cle3)
            {
                *found = 1;
                *in_overflow = 1;
                *i = link;   // overflow block number
                *j = j1;     // index in overflow block
                stop = 1;
            }
        }

        link = buf1.link;  // go to next overflow block
    }
}
void Bulk_Loading(t_TOF *f){
int number=count_lines("dataset.txt");
    t_block buf;


    t_rec *p = (t_rec*)malloc((number/2) * sizeof(t_rec));
    if(!p) {
        printf("Memory allocation failed\n");

    }
FILE *fi;
fi=fopen("dataset.txt","r");

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
buf.upd[j]=0;

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
buf.upd[0]=0;

//buf.eff[0]=0;

j=1;
}
k++;
}
buf.nb=j;
TOF_writeBlock(f,i,&buf);

setHeader(f,i);


}
void N_insert(t_TOF *f, t_LNOF *f1)
{
    int N;
    printf("Give number of records to insert: ");
    scanf("%d", &N);

    int number = count_lines("dataset.txt");
    int start = number / 2;

    // ========================
    // Read save.txt
    // ========================
    FILE *fs = fopen("save.txt", "r");
    int save = start;

    if (fs != NULL) {
        fscanf(fs, "%d", &save);
        fclose(fs);
    }

    // ========================
    // Check limits
    // ========================
    if (save >= number) {
        printf("All records are already inserted.\n");
        return;
    }

    if (save + N > number) {
        printf("Only %d record(s) can be inserted.\n", number - save);
        N = number - save;
    }

    // ========================
    // Open source file
    // ========================
    FILE *fi = fopen("dataset.txt", "r");
    if (fi == NULL) {
        perror("Error opening dataset.txt");
        return;
    }

    t_rec e;

    // ========================
    // Skip records already loaded (Bulk + previous inserts)
    // ========================
    for (int i = 0; i < save; i++) {
        fscanf(fi, "%d %d %d %d",
               &e.student1_id,
               &e.student2_id,
               &e.rating,
               &e.time);
    }

    // ========================
    // Insert new records
    // ========================
    for (int i = 0; i < N; i++) {
        if (fscanf(fi, "%d %d %d %d",
                   &e.student1_id,
                   &e.student2_id,
                   &e.rating,
                   &e.time) != 4)
            break;

        Insert(f, e, f1);  // <-- f1 الآن من نوع LNOF
        save++;
    }

    fclose(fi);

    // ========================
    // Update save.txt
    // ========================
    fs = fopen("save.txt", "w");
    fprintf(fs, "%d", save);
    fclose(fs);

    printf("%d record(s) inserted successfully.\n", N);
    printf("Inserted records range: [%d .. %d[\n",
           save - N, save);
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

void print_friends(t_TOF *f, t_LNOF *f1, int id1)
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

    int n = getHeader(f,1);

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
            LNOF_readBlock(f1, link, &buf1);  // <-- استخدام LNOF هنا

            for (int k = 0; k < buf1.nb && !stop; k++) {
                if (buf1.tab[k].student1_id == id1) {
                        array[u]=buf1.tab[k]; // <-- كان buf[j]، يجب buf[k]
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
    int n = getHeader(f,1);
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


int cmp3_keys(int id1, int id2, int time, t_rec r)
{
    if (id1 < r.student1_id) return -1;
    if (id1 > r.student1_id) return 1;

    if (id2 < r.student2_id) return -1;
    if (id2 > r.student2_id) return 1;

    if (time < r.time) return -1;
    if (time > r.time) return 1;

    return 0; // equal
}


void seqsearch3_binary_keys(t_TOF *f,
                            int id1, int id2, int time,
                            int *found, int *i, int *j)
{
    int n = getHeader(f,1);
    *found = 0;

    t_block buf;

    int low = 1, high = n;
    int block_pos = n;

    // ============================
    // BINARY SEARCH ON BLOCKS
    // ============================
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

        int cmpFirst = cmp3_keys(id1, id2, time, first);
        int cmpLast  = cmp3_keys(id1, id2, time, last);

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

    // ============================
    // BINARY SEARCH INSIDE BLOCK
    // ============================
    low  = 0;
    high = buf.nb - 1;
    *j   = buf.nb;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        int cmp = cmp3_keys(id1, id2, time, buf.tab[mid]);

        if (cmp == 0 && buf.upd[mid]==0) {
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
void update_rating(t_TOF *f, t_LNOF *f1)
{
    int cle1, cle2, cle3, rating;
    t_block buf, buf1;
    int found, i, j, in_overflow;
    t_rec e;

    // Read record keys
    printf("Give id of student1: "); scanf("%d", &cle1);
    printf("Give id of student2: "); scanf("%d", &cle2);
    printf("Give time: "); scanf("%d", &cle3);

    e.student1_id = cle1;
    e.student2_id = cle2;
    e.time = cle3;

    // Search for record
    seqsearch(f, f1, cle1, cle2, cle3, &found, &i, &j, &in_overflow);



    if (!found) {
        printf("Record does not exist\n");
        return;
    }

    printf("Give new rating: "); scanf("%d", &rating);
    e.rating = rating;

    if (!in_overflow) {  // Found in main TOF
        TOF_readBlock(f, i, &buf);

        if (buf.nb < MAXTAB) {  // Block has space → update
            buf.tab[j].rating = rating;
            TOF_writeBlock(f, i, &buf);
            printf("Updated in main block %d\n", i);
        } else { // Block full → insert in overflow
            if (buf.link == 0) { // First overflow block
                //int k = getHeader(f1) + 1;
                int k=LNOF_allocBlock(f1);
                buf.link = k;
                buf.upd[j]=1;
                TOF_writeBlock(f, i, &buf);

                buf1.tab[0] = e;
                buf1.upd[0] = 0; // duplicate
                buf1.nb = 1;
                buf1.link = 0;
                LNOF_writeBlock(f1, k, &buf1);
                LNOF_setHeader(f1, k);
            } else { // Existing overflow blocks
                buf.upd[j]=1;
                TOF_writeBlock(f, i, &buf);
                int link = buf.link;
                int last = link;
                while (link != 0) {
                    LNOF_readBlock(f1, link, &buf1);
                    if (buf1.nb < MAXTAB) {
                        buf1.tab[buf1.nb] = e;
                        buf1.upd[buf1.nb] = 0; // duplicate
                        buf1.nb++;
                        LNOF_writeBlock(f1, link, &buf1);
                        return;
                    }
                    last = link;
                    link = buf1.link;
                }

                // All overflow full → new block
                //int index = getHeader(f1) + 1;
                int index=LNOF_allocBlock(f1);
                buf1.tab[0] = e;
                buf1.upd[0] = 0;
                buf1.nb = 1;
                buf1.link = 0;
                LNOF_writeBlock(f1, index, &buf1);

                LNOF_readBlock(f1, last, &buf1);
                buf1.link = index;
                LNOF_writeBlock(f1, last, &buf1);
                LNOF_setHeader(f1, index);

            }
            printf("Inserted duplicate in overflow\n");
        }
    } else { // Found in overflow
        LNOF_readBlock(f1, i, &buf1);
        buf1.tab[j].rating = rating;
        buf1.upd[j] = 0;
        LNOF_writeBlock(f1, i, &buf1);
        printf("Updated in overflow block %d\n", i);
    }
}





// Compare two records by student1_id and student2_id
int cmp2_keys(int id1, int id2, t_rec r)
{
    if (id1 < r.student1_id) return -1;
    if (id1 > r.student1_id) return 1;

    if (id2 < r.student2_id) return -1;
    if (id2 > r.student2_id) return 1;

    return 0; // id1 and id2 equal
}




void find_first_record(t_TOF *f, int id1, int id2,
                       int *found, int *block_index, int *rec_index)
{
    int n = getHeader(f,1);
    t_block buf;

    *found = 0;
    *block_index = -1;
    *rec_index = -1;

    int low = 1, high = n;
    int candidate_block = -1;

    // ----- Binary search on blocks -----
    while (low <= high)
    {
        int mid = (low + high) / 2;
        TOF_readBlock(f, mid, &buf);

        if (buf.nb == 0) { high = mid - 1; continue; }

        t_rec first = buf.tab[0];
        t_rec last  = buf.tab[buf.nb - 1];

        if (cmp2_keys(id1, id2, first) < 0)
        {
            high = mid - 1;
        }
        else if (cmp2_keys(id1, id2, last) > 0)
        {
            low = mid + 1;
        }
        else
        {
            candidate_block = mid;
            high = mid - 1; // look for first possible block containing the record
        }
    }

    if (candidate_block == -1) return; // not found

    // ----- Binary search inside the block -----
    TOF_readBlock(f, candidate_block, &buf);

    int l = 0, r = buf.nb - 1;
    int candidate_index = -1;

    while (l <= r)
    {
        int m = (l + r) / 2;
        int cmp = cmp2_keys(id1, id2, buf.tab[m]);

        if (cmp == 0)
        {
            candidate_index = m;
            r = m - 1; // continue to search for first occurrence in this block
        }
        else if (cmp < 0)
        {
            r = m - 1;
        }
        else
        {
            l = m + 1;
        }
    }

    if (candidate_index != -1)
    {
        *found = 1;
        *block_index = candidate_block;
        *rec_index = candidate_index;
    }
}
void print_all_records_id1_id2(t_TOF *f, t_LNOF *f1, int id1, int id2)
{
    int found, i, j;
    int stop = 0;
    t_block buf, buf1;

    // 1️⃣ Find first position in main file
    find_first_record(f, id1, id2, &found, &i, &j);

    // =========================
    // CASE 1: FOUND IN MAIN FILE
    // =========================
    if (found)
    {
        while (i <= getHeader(f,1) && !stop)
        {
            TOF_readBlock(f, i, &buf);

            printf("\n--- Main Block %d ---\n", i);

            // scan main block
            while (j < buf.nb && !stop)
            {
                if (buf.tab[j].student1_id == id1 &&
                    buf.tab[j].student2_id == id2)
                {
                    printf("Main [%d,%d] rating=%d time=%d\n",
                           buf.tab[j].student1_id,
                           buf.tab[j].student2_id,
                           buf.tab[j].rating,
                           buf.tab[j].time);
                    j++;
                }
                else
                {
                    stop = 1; // keys changed
                }
            }

            // scan overflow list
            int link = buf.link;
            while (link != 0)
            {
                LNOF_readBlock(f1, link, &buf1);  // <-- استخدام LNOF هنا

                printf("\n--- Overflow Block %d (linked to %d) ---\n", link, i);

                for (int k = 0; k < buf1.nb; k++)
                {
                    if (buf1.tab[k].student1_id == id1 &&
                        buf1.tab[k].student2_id == id2)
                    {
                        printf("Overflow [%d,%d] rating=%d time=%d\n",
                               buf1.tab[k].student1_id,
                               buf1.tab[k].student2_id,
                               buf1.tab[k].rating,
                               buf1.tab[k].time);
                    }
                }
                link = buf1.link;
            }

            i++;
            j = 0;
        }
    }

    // =========================
    // CASE 2: NOT FOUND IN MAIN → SEARCH ONLY LIST
    // =========================
    else
    {
        TOF_readBlock(f, i, &buf);   // i = best block returned

        int link = buf.link;
        int found_in_list = 0;

        while (link != 0)
        {
            LNOF_readBlock(f1, link, &buf1);  // <-- استخدام LNOF هنا

            printf("\n--- Overflow Block %d (linked to %d) ---\n", link, i);

            for (int k = 0; k < buf1.nb; k++)
            {
                if (buf1.tab[k].student1_id == id1 &&
                    buf1.tab[k].student2_id == id2)
                {
                    printf("Overflow [%d,%d] rating=%d time=%d\n",
                           buf1.tab[k].student1_id,
                           buf1.tab[k].student2_id,
                           buf1.tab[k].rating,
                           buf1.tab[k].time);
                    found_in_list = 1;
                }
            }
            link = buf1.link;
        }

        if (!found_in_list)
        {
            printf("No record found with id1=%d and id2=%d (main + list)\n",
                   id1, id2);
        }
    }
}
void Inserer(t_TOF **f, t_rec e) {
    int trouv, i, j;
    t_block buf;
    t_rec x;
    int continu;

    int n = getHeader(*f,1);

    // If file is empty, create the first block and insert
    if (n == 0) {
        buf.tab[0] = e;
        buf.nb = 1;
        buf.link = 0; // no overflow
        TOF_writeBlock(*f, 1, &buf);
        setHeader(*f, 1);
        return;
    }

    // Find the position where e should be inserted
    seqsearch3_binary(*f, e, &trouv, &i, &j);

    if (trouv) {
        // Already exists, do nothing or update depending on your policy
        return;
    }

    continu = 1; // Start insertion process

    while (continu && i <= getHeader(*f,1)) {
        TOF_readBlock(*f, i, &buf);

        // Save last record for possible overflow
        x = buf.tab[buf.nb - 1];

        // Shift records down starting from the last one to position j
        int k = buf.nb - 1;
        while (k >= j) {
            buf.tab[k + 1] = buf.tab[k];
            k--;
        }

        // Insert e at position j
        buf.tab[j] = e;

        if (buf.nb < MAXTAB) {
            // Block not full anymore
            buf.nb += 1;
            TOF_writeBlock(*f, i, &buf);
            continu = 0; // insertion done
        } else {
            // Block is full, propagate last record x to next block
            TOF_writeBlock(*f, i, &buf);
            i += 1;
            j = 0;  // insert at beginning of next block
            e = x;  // propagate
        }
    }

    // If we exceeded the last block, create a new block
    if (continu && i > getHeader(*f,1)) {
        buf.tab[0] = e;
        buf.nb = 1;
        buf.link = 0;
        TOF_writeBlock(*f, i, &buf);
        setHeader(*f, i);
    }
}







void loading_file(t_TOF *f, t_rec *p, int size)
{
    int i, j, k = 0;
    t_block buf;

    /* ===== FILE EMPTY CASE ===== */
    if (getHeader(f,1) == 0) {
        i = 1;
        buf.nb = 0;
        buf.link = 0;
    }
    /* ===== FILE NOT EMPTY ===== */
    else {
        i = getHeader(f,1);
        TOF_readBlock(f, i, &buf);
    }

    j = buf.nb;

    while (k < size) {

        while (j < MAXTAB && k < size) {
            buf.tab[j] = p[k];
            buf.upd[j] = 0;
            j++;
            k++;
        }

        buf.nb = j;
        TOF_writeBlock(f, i, &buf);

        if (k < size) {
            i++;
            j = 0;
            buf.nb = 0;
            buf.link = 0;
        }
    }

    setHeader(f, i);
}



// Display the content of a specific block and its address
void display_block(t_TOF *f, long block_index) {
    t_block buf;

    if (block_index < 1 || block_index > getHeader(f,1)) {
        printf("Invalid block index: %ld\n", block_index);
        return;
    }

    // Read block
    TOF_readBlock(f, block_index, &buf);


    printf("===== Block %ld =====\n", block_index);
    printf("Block nb = %d, link = %d\n", buf.nb, buf.link);

    for (int j = 0; j < buf.nb; j++) {
        printf("[%d,%d,%d,%d] upd=%d\n",
               buf.tab[j].student1_id,
               buf.tab[j].student2_id,
               buf.tab[j].rating,
               buf.tab[j].time,
               buf.upd[j]);
    }
    printf("--------------------------------------------------------------\n");
}



// Display the entire file including header
void display_file(t_TOF *f) {
    t_block buf;

    // Print header
    printf("=== FILE HEADER ===\n");
    printf("Number of blocks: %d\n", f->h.nBlock);
    printf("--------------------------------------------------------------\n");

    // Print each block
    for (int i = 1; i <= getHeader(f,1); i++) {
        TOF_readBlock(f, i, &buf);

        printf("===== Block %d =====\n", i );
        printf("Block nb = %d, link = %d\n", buf.nb, buf.link);

        for (int j = 0; j < buf.nb; j++) {
            printf("[%d,%d,%d,%d] upd=%d\n",
                   buf.tab[j].student1_id,
                   buf.tab[j].student2_id,
                   buf.tab[j].rating,
                   buf.tab[j].time,
                   buf.upd[j]);
        }
        printf("--------------------------------------------------------------\n");
    }
}

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
t_LNOF*f1 = NULL;
t_TOF *f2 = NULL;

t_block buf,buf1;  // the buffer in main memory








int main(){


/*TOF_open(&f,"fouad_file",'N');
TOF_open(&f1,"LnOF",'N');
TOF_open(&f2,"reo",'N');
if (f == NULL) {
    printf("Error: file not opened!\n");
}
*/

TOF_open(&f,"fouad_file",'N');


Bulk_Loading(f);

TOF_close(f);

 int number = count_lines("dataset.txt");
FILE *fs = fopen("save.txt", "w");

     fprintf(fs,"%d",number/2);
     fclose(fs);
     LNOF_open(&f1,"LnOF",'N');
     LNOF_close(f1);
int i, j, found, in_overflow;

int is_lnof;





    t_TOF *mainFile = NULL;   // pointer to main TOF file
    t_TOF *overflowFile = NULL; // pointer to overflow TOF file
    int choice;

printf("*---------------50/100 of the dataset is loaded automatically into the Main Sequential Sorted File (TOF) with 75/100 loading factor ---------------*,");

    do {
        printf("\n================= TOF PROJECT MENU =================\n");
        printf("1. Insert N friendships\n");   //done
        printf("2. Search for the rating between two students\n"); //done
        printf("3. Update an existing rating.\n"); //done
        printf("4. List all friends of a given student.\n"); //done
        printf("5. Produce a clean, fully sorted file and merge.\n");//done
        printf("6. Reset the overflow area\n");
        printf("7. Display the content of any block and its address.\n");//done
        printf("8. Display the entire file, including its header information.\n"); //done
        printf("0. Exit\n");
        printf("====================================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');



                N_insert(f,f1);
                TOF_close(f);
                LNOF_close(f1);
                break;

            case 2: {
                t_rec e;
                int id1,id2;
                printf("Enter student1_id ");
                scanf("%d", &id1);
                printf("Enter student2_id ");
                scanf("%d", &id2);
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                print_all_records_id1_id2(f, f1, id1, id2);
                TOF_close(f);
                LNOF_close(f1);
                break;
            }

            case 3:
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                update_rating(f,f1);
                TOF_close(f);
                LNOF_close(f1);
                break;

            case 4: {
                // Sequential search for a record by keys
                int id1, id2, time, found, i, j, in_overflow;
                printf("Enter student1_id:");
                scanf("%d", &id1);
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                print_friends(f, f1, id1);
                TOF_close(f);
                LNOF_close(f1);
                break;
            }



            case 5: {
                int id1;
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                TOF_open(&f2,"reo",'N');
                reorganization(f2,f,f1);

                TOF_close(f);
                LNOF_close(f1);
                TOF_close(f2);
                break;
            }

            case 6:
                // Update rating of a record
                // update_rating(mainFile, overflowFile);
                break;

            case 7:{
             int nb;
            printf("Enter number of block u wana see: ");
            scanf("%d", &nb);
            TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                TOF_open(&f2,"reo",'N');
            display_block(f,nb);
            TOF_close(f);
                LNOF_close(f1);
                TOF_close(f2);

                break;
            }
            case 8: {
                TOF_open(&f,"fouad_file",'E');
                LNOF_open(&f1,"LnOF",'E');
                TOF_open(&f2,"reo",'N');
                display_file(f);
                TOF_close(f);
                LNOF_close(f1);
                TOF_close(f2);


                break;
            }

            case 0:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while(choice != 0);






}

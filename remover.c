#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>

#define Uarray_Size 1000000


sem_t *semaphore;
int *id_shared;
long current_index = 0;
long shm_id_arr[Uarray_Size];
long buck_id = 1996;
typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item {
        long key;
        long value;
};

typedef struct HashTable HashTable;

typedef struct LinkedList LinkedList;

// Define the Linkedlist here
struct LinkedList {
        long key;
        long value;
        LinkedList* next;
};





// Define the Hash Table here
struct HashTable {
        // Contains an array of pointers
        // to items
        Ht_item** items;
        LinkedList** buckets;
        long size;
        long count;
};




static LinkedList* allocate_list (long size, key_t id) {



        void *ptr = NULL;


        
        long shm_id;

        if ((shm_id = shmget(id, size , IPC_CREAT | 0666)) < 0) {
                perror("shmget error.");
                printf("errno= %d EINVAL=%d \n ", errno , EINVAL);
                return NULL;
        }

        if ((ptr = shmat(shm_id, NULL, 0)) == (void *) - 1) {
                perror("shmat error");
                //exit(1);
                return NULL;

        }



      
        shm_id_arr[current_index] = shm_id ;
        current_index ++ ;
        return ptr;



}





HashTable* create_table(long size) {



        char trace[100];


        HashTable* table = (HashTable *) malloc(sizeof(HashTable));



        assert(table!=NULL);
        table->size = size;
        table->count = 0;


        // items id needed

        table->items = (Ht_item**) malloc (table->size*sizeof(Ht_item*));





        assert(table->items!=NULL);
        




       


        table->buckets = (LinkedList**) malloc (table->size*sizeof(LinkedList*));

        long list_id;
        LinkedList *head;

        for(long i = 0; i < size; i++){
                //printf("Bucket id:%ld\n", buck_id);
                table->buckets[i] = (LinkedList*) allocate_list(sizeof(LinkedList), buck_id);
                list_id = buck_id;
                head = table->buckets[i];
             for (long j = 0; j <size; j++){
                //printf("List id:%ld ", list_id);
                head->next  = allocate_list(sizeof(LinkedList), list_id);
               head = head->next;
                list_id ++;
                
             }
                //printf("\n");


                buck_id = list_id;
        }





        return table;
}

void remove_sharedmem(){
        for(long i =0; i<current_index; i++){
        //        printf("ID:%d\n", shm_id_arr[i]);
                
                if (shmctl(shm_id_arr[i], IPC_RMID, NULL) < 0) { /* remove the shared memory segment. */
                        perror("shmctl error.\n");

                }
        }
}

int main(){

        long key;
        long shmid1;




        key = ftok("/home/reswara1/shared_mem/semaphore",15);
        shmid1 = shmget(key, 1024 , 0666 | IPC_CREAT);
        if (shmid1 < 0) {
                printf("*** shmget error (server) ***\n");
                exit(1);
        }

        shm_id_arr[current_index] = shmid1 ;

        current_index ++;
       

        semaphore = (sem_t *)shmat(shmid1, NULL, 0);



        key = ftok("/home/reswara1/shared_mem/id_shared",16);
        shmid1 = shmget(key, 1024 , 0666 | IPC_CREAT);
        if (shmid1 < 0) {
                printf("*** shmget error (server) ***\n");
                exit(1);
        }

        id_shared = (int *) shmat(shmid1, NULL, 0);

        shm_id_arr[current_index] = shmid1 ;

        current_index ++;


              HashTable *ht = create_table(50);

                HashTable *ht_2 = create_table(50);
                
                remove_sharedmem();
               return 1;

        }

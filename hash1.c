/* This is the implementation of hashtable using shared memory
* The code uses SHA-256 to compute the hash and creates a hashtable with Y*Y ITEMS AND BUCKETS (Y is the size)
* Copyright 2022 Roja Eswaran, PhD Candidate <reswara1@binghamton.edu>
* Copyright 2022 Prf Kartik Gopalan <kartik@binghamton.edu>
*/

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha-256.h"
#include <assert.h>
#include <semaphore.h>
#include<errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#define MAX_SHM_ID_NUM 100

       

sem_t *semaphore;
int *id_shared;
long buck_id = 8870;
long list_id;
int items_id, buckets_id, ptr_id;
int shm_id_arr[MAX_SHM_ID_NUM];
int shm_buck_arr[MAX_SHM_ID_NUM];
int current_index = -1;
key_t key_current;
long item_count=0;

long pfn_entry = 0, vpn_entry = 0;




long Bucket_Size;
long CAPACITY;

char file1[100];
char file2[100];

long global_index = 0;

FILE *trace_fd_hash;

key_t key;
int shmid1;
int item_one = 0;

long calculate_base_index(long base, long index){
                long id;     
                long item_count = Bucket_Size;

                if(!index)
                        return base;
                else
                        return base+(item_count*(index));
                
                        
}
                




                
             
unsigned long hash_function(char* str) {

        uint8_t hash[32];
        char string [10000];


        unsigned long i = 0;



        calc_sha_256(hash, str, strlen(str));
        hash_to_string(string, hash);

        for (long j=0; string[j]; j++)
                i += string[j];

        return i % CAPACITY;
}

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



static LinkedList* allocate_list (int size, key_t id) {



        void *ptr = NULL;



        int shm_id;

        if ((shm_id = shmget(id, size , 0)) < 0) {
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
        Bucket_Size = size;
        CAPACITY = size;


        assert(table!=NULL);
        table->size = size;
        table->count = 0;




        table->items = (Ht_item**) malloc (table->size*sizeof(Ht_item*));





        assert(table->items!=NULL);








        table->buckets = (LinkedList**) malloc (table->size*sizeof(LinkedList*));

      
        LinkedList *head;
    


        table->count = buck_id;
        for(long i = 0; i < size; i++){
                table->buckets[i] = (LinkedList*) allocate_list(sizeof(LinkedList), buck_id);
                printf("Buck id:%ld\n", buck_id);
                shm_buck_arr[item_count]=buck_id;
                item_count ++;
                list_id = buck_id;
                head = table->buckets[i];

                for (long j = 0; j <size; j++){
           
                        head->next  = allocate_list(sizeof(LinkedList), list_id);
                        head = head->next;
                        list_id ++;

                }
             


                buck_id = list_id;
        }



      

        for(long i =0; i<current_index; i++){

                printf("%ld ", shm_id_arr[i]);
        }
        printf("\n");
        printf("Done creating table\n");
        return table;

}


long ht_search(HashTable* table, long key) {
      


        char str_key[10000];

        sprintf(str_key, "%lx", key);

        long index = hash_function(str_key);

        //index = 0;


          long base_index = calculate_base_index(table->count, index);

        printf("Ht_ insert buckets id:%d\n", base_index);

        LinkedList *head = table->buckets[index];
         
        


        long count =0;




        if(head->key == key){
        

                return 1;
        }

           
       
                while (head->value!=-1) {
                     
                        base_index ++;
                        count ++;

                        if(head->key == key){

                                return count;
                        }
                        

                        head->next = allocate_list(sizeof(LinkedList), base_index);
                        head = head->next; 

                }



          



        return -1;
}





int ht_insert(HashTable* table, long key) {

        printf("Ht insert:%ld\n", key);
        char trace[1000];
 
        char str_key[10000];

        sprintf(str_key, "%lx", key);


        unsigned long index = hash_function(str_key);
        //index = 0;   


        
        
        long base_index = calculate_base_index(table->count, index);

        printf("buck id%d  base:%ld  index:%ld\n", base_index, table->count, index);

        LinkedList *head = table->buckets[index];
      

         
        if (head->key==-1) {



                printf("Inserting 1:%ld\n", key);



                head->key = key;
                head->value = 1;

                printf("Inserting success:%ld\n", table->buckets[index]->key);


                return 1;
        }





        LinkedList *temp = head;

        while (temp->next->value!=-1) {

                base_index ++;
                printf("items:%ld ", temp->key);
                temp = temp->next; 
                
                
        }

         printf("items:%ld ", temp->key);
      

        base_index ++;
        printf("base index:%ld\n", base_index);
        LinkedList *node  = allocate_list(sizeof(LinkedList), base_index);
        node->key = key;
        node->value = 1;
        
        temp->next=node;
        return 3;





}




int main() {




        int key;
        int shmid1;




        key = ftok("/home/reswara1/shared_mem/semaphore",15);  // Optional
        shmid1 = shmget(key, 1024 , 0);
        if (shmid1 < 0) {
                printf("*** shmget error (server) ***\n");
                exit(1);
        }


        shm_id_arr[current_index] = shmid1 ;

        current_index ++;


        semaphore = (sem_t *)shmat(shmid1, NULL, 0);



        key = ftok("/home/reswara1/shared_mem/id_shared",16); // Optional
        shmid1 = shmget(key, 1024 , 0);
        if (shmid1 < 0) {
                printf("*** shmget error (server) ***\n");
                exit(1);
        }

        id_shared = (int *) shmat(shmid1, NULL, 0);



        shm_id_arr[current_index] = shmid1 ;

        current_index ++;

        HashTable* ht = create_table(6);
       HashTable* ht2 = create_table(6);
   
        

        int elt = 0;
        int dummy = 896;
        printf("id_shared:%d\n", *id_shared);

        while(elt<3){
               sem_wait(semaphore);
                ht_insert(ht,dummy);
                ht_insert(ht2,dummy);
                sem_post(semaphore);
                elt ++;
                dummy++; 
                


        }

        printf("Id shared:%d\n", *id_shared);



        sleep(10);        
       printf("Yay coming out\n");

         printf("Id shared:%d\n", *id_shared);

        elt = 0;
        dummy = 1000;
        while(elt<3){
             
             
                if(ht_search(ht,dummy)!=-1)
                        printf("%ld present\n", dummy);
               
                
                if(ht_search(ht2,dummy)!=-1)
                        printf("%ld present\n", dummy);
               

                elt ++;
                dummy++; 
                

        }

       
       while(1);
 
}

# hashtable-using-sharedmemory

1) To set up the shared memory segment for the hashtable run gcc writer.c -o writer -lpthread
2) Hash1.c uses sha256 for hash caluculation when there is a collision, specify the size while creating the table
3) To remove the already created segment, run gcc remover.c -o remover
4) To run, compile Hash1.c and execute it 

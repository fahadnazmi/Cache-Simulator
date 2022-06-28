#include <iostream>
#include <string>
#include <vector>
#include <iomanip> 
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;
int cache_type, block_size, cache_size,number_of_blocks=0;;

int compulsory_misses=0, capacity_misses=0, conflict_misses=0;


#define		DRAM_SIZE		(64*1024*1024)


unsigned int memGen1()
{
    static unsigned int addr=0;
    return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
    static unsigned int addr=0;
    return (addr++)%(1024);
}

unsigned int memGen3()
{
    static unsigned int addr=0;
    return (addr++)%(1024*64);
}

unsigned int memGen4()
{
    static unsigned int addr=0;
    return (addr+=256)%(DRAM_SIZE);
}


// Cache Simulator
bool cacheSim(unsigned int addr, int cache[3][100000], int type, int &block_counter, int index_addr, int tag_addr)
{
    int shift_offset=log2(block_size);
    bool detected=false;
    bool misses_flag=true;
    
    if (cache_type==0)    // Direct Mapped ******************************************
    {
        if (cache[0][index_addr]==tag_addr)
        {
            return true;
        }
        else
            {
                cache[0][index_addr]= tag_addr;
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[1][i]!=1)
                    {   misses_flag=false;
                        i=number_of_blocks;}
                    
                }
                //calculating misses
                if (misses_flag)
                    capacity_misses++;   // Capacity miss because the cache is full
                else
                {
                    if(cache[1][index_addr]==1)
                        conflict_misses++;
                    else
                    {
                        compulsory_misses++;
                    }
                }
                cache[1][index_addr]= 1;
                return 0;
            }
    } // end of directed mapped
    ///////////////////////////////////////////////////////////////////
    
    else if (cache_type==1)   // set asscoiative *************************************
    {
        index_addr=index_addr * type;
        for (int i=0; i < type ; i++)
        {
            if (cache[0][index_addr+i]==tag_addr)
            {
                return 1;
            }
        }
        for (int j=0; j < type; j++)
        {
            if (cache[1][index_addr+j] == -1)
            {
                compulsory_misses++;
                cache[0][index_addr+j]=tag_addr;
                cache[1][index_addr+j]=1;
                return 0;
            }
        }
        
        srand(time(NULL));
        int x=rand()%(type);
        cache[0][index_addr+x]=tag_addr;
        cache[1][index_addr+x]=1;
        capacity_misses++;
        return 0;
        
    }//end of set assciative

    
    
    else if (cache_type==2)        // fully associative **************************************
    {
        if (type==0)    // LRU /////////
        {
            if (block_counter < number_of_blocks)
            {
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==addr >> shift_offset)
                    {
                        detected=true;
                        cache[1][i]=block_counter;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    compulsory_misses++;
                    cache[0][block_counter]=addr>>shift_offset;
                    cache[1][block_counter]=block_counter;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==(addr >> shift_offset))
                    {
                        detected=true;
                        cache[1][i]=block_counter;
                        //block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    int compare=0;
                    for (int i=1; i < number_of_blocks; i++)
                    {
                        if (cache[1][compare] > cache[1][i])
                            compare=i;
                    }
                    cache[0][compare]=addr >> shift_offset;
                    cache[1][compare]=block_counter;
                    capacity_misses++;
                    return false; //miss

                }
            }
        }   // end of LRU
        
        else if (type==1)   // LFU ///////////////
        {
            if (block_counter < number_of_blocks)
            {
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==addr >> shift_offset)
                    {
                        detected=true;
                        cache[1][i]=cache[1][i]+1;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    cache[0][block_counter]=addr>>shift_offset;
                    cache[1][block_counter]=-1;
                    compulsory_misses++;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==(addr >> shift_offset))
                    {
                        detected=true;
                        cache[1][i]++;
                        block_counter--;
                        return detected; //hit
                    }
                }
                if (!detected)
                {
                    int compare2=0;
                    for (int i=1; i < number_of_blocks; i++)
                    {
                        if (cache[1][compare2] >= cache[1][i])
                            compare2=i;
                    }
                    cache[0][compare2]=addr >> shift_offset;
                    cache[1][compare2]=-1;
                    capacity_misses++;
                    return false; //miss
                }
            }

        }  // end of LFU
        
        else if (type==2)
        {
            if (block_counter < number_of_blocks)
            {
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==addr >> shift_offset)
                    {
                        detected=true;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    cache[0][block_counter]=addr>>shift_offset;
                    cache[1][block_counter]=block_counter;
                    compulsory_misses++;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==(addr >> shift_offset))
                    {
                        detected=true;
                        //block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    int compare=0;
                    for (int i=1; i < number_of_blocks; i++)
                    {
                        if (cache[1][compare] > cache[1][i])
                            compare=i;
                    }
                    cache[0][compare]=addr >> shift_offset;
                    cache[1][compare]=block_counter;
                    capacity_misses++;
                    return false; //miss
                }
            }
        }// end of FIFO
        
        else if (type==3)
        {
            if (block_counter < number_of_blocks)
            {
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==addr >> shift_offset)
                    {
                        detected=true;
                        block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    cache[0][block_counter]=addr>>shift_offset;
                    compulsory_misses++;
                    return false;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < number_of_blocks; i++)
                {
                    if (cache[0][i]==(addr >> shift_offset))
                    {
                        detected=true;
                        //block_counter--;
                        return detected; //hit
                    }
                }
                
                if (!detected)
                {
                    srand(time(NULL));
                    cache[0][rand()%number_of_blocks]=addr >> shift_offset;
                    capacity_misses++;
                    return 0; //miss
                }
            }
        }//end of RANDOM
    }  // end of Fully associative
    
    
    
    return true;
}

char *msg[2] = {"Miss","Hit"};


///////////////////////////////////////////////////////////////////

int main() {
    
    int looper=1000000, addr, flag, shift;

    cout << "Please, enter 0 for Direct mapped, 1 for set associative, 2 for fully associative: " << endl;
    cin >> cache_type;
    cout << "Please, enter the size of the block as a Power of 2 between 4 and 128 byte  :" << endl;
    cin >> block_size;
    cout << "Please, enter cache size: 1KB - 64KB; in steps that are power of 2: " << endl;
    cin >> cache_size;
    
    

    int cache[3][100000];
    int block_counter=0;
    int hit_counter=0;
    int index_addr=0, tag_addr=0;
    
    ///////////////////////////////////////////////////////////////////
    if ( cache_type==0)   //Direct_mapped **************
    {
        number_of_blocks= (cache_size*1024)/block_size;
        
        ////////////////////
        for (int i=0; i < 2; i++)   // setting all the cache with -1
            for (int j=0; j < number_of_blocks; j++)
                cache[i][j]=-1;
        //////////////////
        
        for(int i=0; i <looper ;i++)
        {
            addr = memGen1();
            shift= log2(block_size);
            index_addr= (addr >> shift)% number_of_blocks;
            shift= log2(number_of_blocks+block_size);
            tag_addr= addr >>shift;    // shifted the amount the offset and the index sizes
            flag = cacheSim(addr, cache, 0,block_counter, index_addr, tag_addr);
            index_addr=0;
            tag_addr=0;
            
            if (msg[flag]=="Hit")
            {
                hit_counter++;
            }
        }
    cout << "Hits  " << hit_counter<<endl << "Compulsory:  " << compulsory_misses <<endl<< "Capacity:  " << capacity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;

    }
    ///////////////////////////////////////////////////////////////////
    else if (cache_type==2)  //  Fully associative**************
    {
        int replacment_type;
        number_of_blocks= (cache_size*1024)/block_size;
        
        cout << "please, enter the type of replacment for the Fully Associative: LRU->0 , LFU->1, FIFO->2, RANDOM->3  :- " << endl;
        cin >> replacment_type;
        
        
        for (int i=0; i < 2; i++)   // setting all the cache with -1
            for (int j=0; j < number_of_blocks; j++)
                cache[i][j]=-10;
        
        for(int i=0; i <looper ;i++)
        {
            addr = memGen3();
            flag = cacheSim(addr, cache, replacment_type, block_counter, index_addr, tag_addr);
            
            if (msg[flag]=="Hit")
            {
                hit_counter++;
            }
            block_counter++;

        }
        
        cout << "Hits  " << hit_counter<<endl << "Compulsory:  " << compulsory_misses <<endl<< "Capacity:  " << capacity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;
        
    } // end of fully associative main
    else if (cache_type==1) // set associative
    {
        int number_of_ways;
        cout << "please, enter the number of ways for the set associative cache: 2,4,8,16" << endl;
        cin >> number_of_ways;
        number_of_blocks= (cache_size*1024)/(block_size*number_of_ways);
        
        for (int i=0; i < 3; i++)   // setting all the cache with -1
            for (int j=0; j < 100000; j++)
                cache[i][j]=-1;
        
        for(int i=0; i <looper ;i++)
        {
            addr = memGen1();
            shift= log2(block_size);
            index_addr= (addr >> shift)% (number_of_blocks);
            shift= log2(number_of_blocks+block_size);
            tag_addr= addr >>shift;
            flag = cacheSim(addr, cache, number_of_ways, block_counter, index_addr, tag_addr);
            index_addr=0;
            tag_addr=0;
            if (msg[flag]=="Hit")
            {
                hit_counter++;
            }
            block_counter++;
        }
        
        cout << "Hits  " << hit_counter<<endl << "compulsory:  " << compulsory_misses <<endl<< "Capacity:  " << capacity_misses <<endl<< "Conflict:  " << conflict_misses <<  endl;
    }
  
}

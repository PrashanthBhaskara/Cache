
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "cachelab.h"
//Global variables
//command line args: s, E, b, t, v, h (not using h here)
int v, s, E, b = 0;
int hitCount = 0, missCount = 0, evictionCount = 0;
char* t;
int LRUg = 1; //global LRU that will constantly be updated in order



//cache struct needed 

//initializing, loading cache,

//structs used to create a cache
struct block {
	int tag;
	int v_bit;
	int LRU; //used in order to determine what block should be updated ultimately when eviction is referred to
};


struct group{
	struct block *lines;	 //array of blocks
};


struct cache {
	struct group *bls;  //2d array of blocks
};

typedef struct cache cache;
typedef struct group group;
typedef struct block block;


 void simulate(char operation, unsigned long int address, int size, int idxmask, cache* sim, int *arr) //as referenced in p3 description
{	
			unsigned long int temptag = address >> b; //unsigned to have more pos values, for some reazon csil does not let uint64 work
			temptag = temptag >> s;

			unsigned long int gidx = (address >> b) ;
			gidx = gidx & idxmask; //anding here gives idx removed from other unnecessary vals
			int hitflag = 0;
			int emptyspace = 0;
			int missindex = -1;

			for(int j = 0; j<E; j++)
			{
				if ((((sim->bls)[gidx]).lines)[j].v_bit == 0)
				{
					missindex = j;
					emptyspace = 1;

				}


				else if ((((sim->bls)[gidx]).lines)[j].v_bit == 1) 
				{
					//hitflag = 0 but emptyspace = 1
					if ((((sim->bls)[gidx]).lines)[j].tag == temptag) //check to see if hit
					{
						hitflag = 1;
						hitCount++;
						((((sim->bls)[gidx]).lines)[j].LRU) = arr[gidx];
						arr[gidx]++; //used to show that block has been used
						break;  
					}
				}
			}


			if(hitflag == 0 && emptyspace == 1)
			{
				//updating for a miss
				((((sim->bls)[gidx]).lines)[missindex].v_bit) = 1;
				((((sim->bls)[gidx]).lines)[missindex].tag) = temptag;
				((((sim->bls)[gidx]).lines)[missindex].LRU) = arr[gidx];
				arr[gidx]++;
				missCount++;
			}
			else if(hitflag==0 && emptyspace == 0)
			{
				//updating for an eviction
					int a = arr[gidx];
					missCount++;
					evictionCount++;
					//find least recently used and replace it
					int t = 0;
	 				for (int i = 0; i < E; i++) 
	 				{
	 					// printf("LRU:%d\n",(((sim->bls)[gidx]).lines)[i].LRU );
	    				if ((((sim->bls)[gidx]).lines)[i].LRU < a)
	    				{
	    					a = (((sim->bls)[gidx]).lines)[i].LRU;
	       					t = i;
	    				}
	    			}

					((((sim->bls)[gidx]).lines)[t].v_bit) = 1;
					((((sim->bls)[gidx]).lines)[t].tag) = temptag;
					((((sim->bls)[gidx]).lines)[t].LRU) = arr[gidx];
					arr[gidx]++;	

			}
			hitflag = 0;
			emptyspace = 0;
			missindex = -1;
}


int main(int argc, char *argv[])
{

	int parse;
	while((parse = getopt(argc, argv, "s:E:b:t:")) != -1) //looping through everything
	{	

		if(parse == 's')
		{
			s = atoi(optarg); //parses to next one which is val


		}

		else if(parse == 't')
		{
			t = optarg; //file name this time here


		}

		else if (parse == 'E')
		{
			E = atoi(optarg);

		}
		else 
		{
			b = atoi(optarg);

		}
	}


	//2^s = amt of groups

	 int gamt = 2 << (s-1); //amt of sets
	 int idxmask = ((2 <<(s-1)) -1); //mask to get the correct index bits in simulate


	//initialize cache:
	cache *sim = (cache*) malloc(sizeof(cache));
	sim->bls = (group*) malloc(sizeof(group) * gamt);
	for(int i = 0; i < gamt; i++)
	{
		(((sim->bls)[i]).lines) = (block*) malloc(sizeof(block) * E );
	}
	for(int i = 0; i<gamt; i++)
	{
		{
			for(int j = 0; j <E; j ++)
			{
				(((sim->bls)[i]).lines)[j].tag = 0;
				(((sim->bls)[i]).lines)[j].v_bit = 0;
				(((sim->bls)[i]).lines)[j].LRU = 0;
			}
		}

	}


	FILE *f;
	f = fopen(t, "r"); 
	char operation;
	unsigned long int address;
	int size;


//array of counters used in order to figure out LRU when eviction required
	int arr[idxmask+1];
	for(int i = 0; i < idxmask+1; i++)
	{
		arr[i] =1;

	}

	while(fscanf(f, " %c %lx,%d", &operation, &address, &size) >0)
	{
	//	printf("%c\n",operation );
		if(operation == 'I')
		{

		}
		else if(operation == 'L' || operation == 'S')
		{
			simulate( operation, address,  size,  idxmask, sim, arr);
		} 
	
 		else if (operation == 'M')
 		{
 			simulate( operation, address,  size,  idxmask, sim, arr);// sim twice since its a M operation
 			simulate( operation, address,  size,  idxmask, sim, arr);
		}
	}

    //printf("the value is %d", hitCount);
   printSummary(hitCount, missCount, evictionCount);
   fclose(f);
   	for(int i = 0; i < gamt; i++)
	{
		free((((sim->bls)[i]).lines));
	}
   free(sim->bls);
   free(sim);
   return 0;
   }

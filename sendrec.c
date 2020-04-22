#include <lua.h>   
#include <lauxlib.h>
#include <lualib.h> 
#include <stdlib.h>  
#include <string.h>

#define  LUA_START(source) \
		lua_State *L;\
		L = luaL_newstate();\
		luaL_openlibs(L);\
		if (luaL_loadfile(L, source)) bail(L, "luaL_loadfile() failed");\
		if (lua_pcall(L, 0, 0, 0)) bail(L, "lua_pcall() failed");	

#define LUA_FINISH lua_close(L);

#include "mpi.h"
#include <stdio.h>

#define  MPI_START \
		int rank, size;\
		MPI_Init( &argc, &argv );\
		MPI_Comm_rank( MPI_COMM_WORLD, &rank ); \
		MPI_Comm_size( MPI_COMM_WORLD, &size );


#define  MPI_FINISH MPI_Finalize();

void bail(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
	exit(1);
}

struct operation
{ 
   char type[5]; 
   char msg[50]; 
   double part; 
   double sum; 
};

int main( int argc, char *argv[] )
{
    LUA_START (LUA);
    MPI_START;
    MPI_Status status;
    char msg [100];
    // lua-C code block
    
    //bcast stuff
    double part, sum;
    
    
    lua_getglobal(L, "main");               
    lua_pushnumber(L, rank);                    
    lua_pushnumber(L, size);                    
    lua_pushnumber(L, sum);                                                                        
    
    if (lua_pcall(L, 3, 1, 0)) 
    bail(L, "lua_pcall() failed");     

	
	lua_pushnil(L);  /* Make sure lua_next starts at beginning */
	struct operation oper;
	const char *k, *v;
	while (lua_next(L, -2)) {                   
		v = lua_tostring(L, -1);                 
		lua_pop(L,1);                           
		k = lua_tostring(L, -1);  
		if (strcmp(k,"oper") == 0) strcpy(oper.type,v);
		if (strcmp(k,"msg")  == 0) strcpy(oper.msg,v);
		if (strcmp(k,"part")  == 0) oper.part=(double)atof(v);
		if (strcmp(k,"sum")  == 0)  oper.sum=(double)atof(v);
	}	
	if (strcmp(oper.type , "recv")  == 0) {
		for (int i = 1; i < size; i++ ) {
			MPI_Recv( msg, sizeof( msg), MPI_CHAR, i, 1, MPI_COMM_WORLD, &status );
			printf( "%d : %s \n",status.MPI_SOURCE, msg );
		}
	}else if(strcmp(oper.type , "send")  == 0) {
		MPI_Send( oper.msg,sizeof(oper.msg)+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}else if(strcmp(oper.type , "bcast")  == 0) {
		MPI_Bcast(oper.msg, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}else if(strcmp(oper.type , "reduce")  == 0) {
		MPI_Reduce(&part, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	}else if(strcmp(oper.type , "_")  == 0) {
		printf( "%s\n",oper.msg );
	}
  
    MPI_FINISH;
    LUA_FINISH;
    
    return 0;
}

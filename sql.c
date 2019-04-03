#include <sqlite3.h>
#include <stdio.h>


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
  }
  
int main(){
    
    sqlite3* db;
    char *zErrMsg = 0;
    int opendb = sqlite3_open("proj1.db", &db);
    
    
    if(opendb != SQLITE_OK){
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    char* sql_table = "CREATE TABLE Profiles(Id INT PRIMARY KEY NOT NULL, Name TEXT, Surname TEXT, Photo BLOB, Addr TEXT, Academic TEXT, Abilities TEXT, Exp TEXT)";
    
    opendb = sqlite3_exec(db,sql_table, callback, 0, &zErrMsg);
    if(opendb != SQLITE_OK){
        printf("SQL error: %s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    
    
    
    sqlite3_close(db);
    return 0;
    
}

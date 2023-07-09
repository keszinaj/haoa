#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>

#define myCandidate 178

/*
   Structure to help remember data
*/

typedef struct KeyValue {
    SQLINTEGER key;
    SQLINTEGER value;
    struct KeyValue* next;
} KeyValue;

typedef struct Dictionary {
    KeyValue* head;
} Dictionary;

typedef struct DB_properties{
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret;
}DB_properties;

Dictionary* dictCreate() {
    Dictionary* list = (Dictionary*)malloc(sizeof(Dictionary));
    list->head = NULL;
    return list;
}

/*
  Inserting a key and value
*/
void dictInsert(Dictionary* list, SQLINTEGER key, SQLINTEGER value) {
    KeyValue* entry = (KeyValue*)malloc(sizeof(KeyValue));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;

    if (list->head == NULL) {
        list->head = entry;
    } else {
        KeyValue* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = entry;
    }
}

/*
  Key-based retrieval of values
*/
SQLINTEGER dictGetValue(Dictionary* list, SQLINTEGER key) {
    KeyValue* current = list->head;
    while (current != NULL) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return -1;  // the key does not exist
}

void destroyDict(Dictionary* list) {
    KeyValue* current = list->head;
    while (current != NULL) {
        KeyValue* next = current->next;
        free(current->key);
        free(current);
        current = next;
    }
    free(list);
}

int connect_to_db(DB_properties *db_properties)
{
    // Initialize the ODBC environment
    db_properties->ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &db_properties->env);
    db_properties->ret = SQLSetEnvAttr(db_properties->env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    db_properties->ret = SQLAllocHandle(SQL_HANDLE_DBC, db_properties->env, &db_properties->dbc);
    SQLCHAR* connectionString = (SQLCHAR*)"DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D:\\zdyskuH\\WINvote\\DATA\\MachineTally.mdb;PWD=shoup;";
    db_properties->ret = SQLDriverConnect(db_properties->dbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    return 0;
}

int disconnect_from_db(DB_properties *db_properties)
{
    SQLDisconnect(db_properties->dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, db_properties->dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, db_properties->env);
}

int revote(DB_properties *db_properties, int candidate, int votes)
{

    if (db_properties->ret == SQL_SUCCESS || db_properties->ret == SQL_SUCCESS_WITH_INFO) {
        printf("Connected to the MDB database.\n");

        SQLHSTMT stmt;
        db_properties->ret = SQLAllocHandle(SQL_HANDLE_STMT, db_properties->dbc, &stmt);
        SQLCHAR* query;
        asprintf(&query, (SQLCHAR*)"UPDATE BallotCount SET nVoteCount = %d WHERE nCandidateID = %d", votes, candidate);
        db_properties->ret = SQLExecDirect(stmt, query, SQL_NTS);

        if (db_properties->ret == SQL_SUCCESS || db_properties->ret == SQL_SUCCESS_WITH_INFO) {
            printf("Query executed successfully.\n");

        }
        else {
            fprintf(stderr, "Failed to execute the UPDATE query.\n");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
   else{
    printf("NOT connected to the MDB database.\n");
   }
    return 0;

}

int count_votes(DB_properties *db_properties, Dictionary* vote_data, int id)
{
    int votes_for_others = 0;
    if (db_properties->ret == SQL_SUCCESS || db_properties->ret == SQL_SUCCESS_WITH_INFO) {
        printf("Connected to the MDB database.\n");

        SQLHSTMT stmt;
        db_properties->ret = SQLAllocHandle(SQL_HANDLE_STMT, db_properties->dbc, &stmt);
        SQLCHAR* query;
        asprintf(&query, (SQLCHAR*)"SELECT nCandidateID, nVoteCount FROM BallotCount", id);
        db_properties->ret = SQLExecDirect(stmt, query, SQL_NTS);

        if (db_properties->ret == SQL_SUCCESS || db_properties->ret == SQL_SUCCESS_WITH_INFO) {
            printf("Query executed successfully.\n");

            SQLCHAR data[256];
            SQLLEN indicator;
            SQLINTEGER columnValue1;
            SQLINTEGER columnValue2;
            int row = 0;

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                ++row;
                SQLGetData(stmt, 1, SQL_C_LONG, &columnValue1, 0, NULL);
                SQLGetData(stmt, 2, SQL_C_LONG, &columnValue2, 0, NULL);
                dictInsert(vote_data, columnValue1, columnValue2);

                printf("Row %d: %d\n", columnValue1, columnValue2);
                if(columnValue1 != id)
                {
                    votes_for_others += columnValue2;
                }
            }

            printf("Total rows fetched: %d\n", row);

        }
        else {
            fprintf(stderr, "Failed to execute the UPDATE query.\n");
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
   else{
    printf("NOT connected to the MDB database.\n");
   }

    return votes_for_others;
}

int change_votes(Dictionary* vote_data, int phase)
{
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret;


    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    SQLCHAR* connectionString = (SQLCHAR*)"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=D:\\SVS\\DATA\\MachineTally.mdb;PWD=shoup;";
    ret = SQLDriverConnect(dbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);


    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        printf("Connected to the MDB database.\n");
        SQLHSTMT stmt;
        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        SQLCHAR* query= (SQLCHAR*)"SELECT nCandidateID, nVoteCount FROM BallotCount";
        ret = SQLExecDirect(stmt, query, SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            printf("Query executed successfully.\n");
            SQLCHAR data[256];
            SQLLEN indicator;
            SQLINTEGER columnValue1;
            SQLINTEGER columnValue2;
            int row = 0;

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                ++row;
                SQLGetData(stmt, 1, SQL_C_LONG, &columnValue1, 0, NULL);
                SQLGetData(stmt, 2, SQL_C_LONG, &columnValue2, 0, NULL);

                dictInsert(vote_data, columnValue1, columnValue2);
                printf("Row %d: %d\n", columnValue1, columnValue2);
            }

            printf("Total rows fetched: %d\n", row);
        }
        else {
            fprintf(stderr, "Failed to execute the UPDATE query.\n");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        SQLDisconnect(dbc);
    }
    else {
        fprintf(stderr, "Failed to connect to the MDB database.\n");
    }
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    return 0;

}

int main()
{
    Dictionary* vote_data = dictCreate();
    DB_properties db_properties;

    connect_to_db(&db_properties);
    int votes_for_others = count_votes(&db_properties, vote_data, myCandidate);
    printf("oddana %d glosow\n\n", votes_for_others);
    KeyValue* current = vote_data->head;
    while (current != NULL) {
            if(current->key != myCandidate)
            {
                revote(&db_properties, current->key, 0);
            }
        KeyValue* next = current->next;
        current = next;
    }
    revote(&db_properties, myCandidate, votes_for_others);
    disconnect_from_db(&db_properties);

    /**vote_for_mycandidate(178);
    change_votes(prev_vote_data, 1);
    KeyValue* current = prev_vote_data->head;
    while (current != NULL) {
        printf("%d %d \n", current->key, current->value );
        KeyValue* next = current->next;
        current = next;
    } **/
    return 0;
}
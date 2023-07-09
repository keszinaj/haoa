#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>

// Struktura przechowująca klucz i wartość
typedef struct KeyValue {
    SQLINTEGER key;
    SQLINTEGER value;
    struct KeyValue* next;
} KeyValue;

// Struktura reprezentująca listę klucz-wartość
typedef struct Dictionary {
    KeyValue* head;
} Dictionary;

// Tworzenie nowej listy klucz-wartość
Dictionary* dictCreate() {
    Dictionary* list = (Dictionary*)malloc(sizeof(Dictionary));
    list->head = NULL;
    return list;
}

// Wstawianie klucza i wartości do listy
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

// Pobieranie wartości na podstawie klucza
SQLINTEGER dictGetValue(Dictionary* list, SQLINTEGER key) {
    KeyValue* current = list->head;
    while (current != NULL) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    return -1;  // Klucz nie istnieje na liście
}
int dictUpdateValue(Dictionary* list, SQLINTEGER key, SQLINTEGER newvalue) {
    KeyValue* current = list->head;
    while (current != NULL) {
        if (current->key == key) {
            current->value == newvalue;
            return 0;
        }
        current = current->next;
    }
    return -1;  // Klucz nie istnieje na liście
}
// Zwalnianie pamięci zajmowanej przez listę
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




int save_to_file(int candidate, int nVotes)
{
    FILE *file;
    file = fopen("data.txt", "a");

    if (file == NULL) {
        printf("Unable to open the file.\n");
        return 1;
    }
    time_t currentTime = time(NULL);

    // Konwersja czasu na lokalną reprezentację
    struct tm *localTime = localtime(&currentTime);

    // Pobranie poszczególnych składowych godziny
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;


    fprintf(file, "Zagłosowano na kandydata %d o godzinie %02d:%02d:%02d\n", candidate, hour, minute, second);
    fclose(file);
    return 0;
}

int check_db(int first_run, Dictionary* prev_vote_data)
{
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret;

    // Initialize the ODBC environment
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

    // Allocate a connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    // zmień na poprawne sterowniki itd....(SQLCHAR*)"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=D:\\SVS\\Data\\MachineTally.mdb;PWD=shoup;";
    SQLCHAR* connectionString = (SQLCHAR*)"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=D:\\SVS\\Data\\MachineTally.mdb;PWD=shoup;";
    ret = SQLDriverConnect(dbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);


    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        printf("Connected to the MDB database.\n");

        SQLHSTMT stmt;
        ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
        SQLCHAR* query = (SQLCHAR*)"SELECT nCandidateID, nVoteCount FROM BallotCount";
        ret = SQLExecDirect(stmt, query, SQL_NTS);

        if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
            printf("Query executed successfully.\n");

            // Fetch and display the result set
            SQLCHAR data[256];
            SQLLEN indicator;
            SQLINTEGER columnValue1;
            SQLINTEGER columnValue2;
            int row = 0;

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                ++row;
                SQLGetData(stmt, 1, SQL_C_LONG, &columnValue1, 0, NULL);
                SQLGetData(stmt, 2, SQL_C_LONG, &columnValue2, 0, NULL);
                if(first_run)
                {
                    dictInsert(prev_vote_data, columnValue1, columnValue2);

                }
                else{
                    SQLINTEGER nOldCount = dictGetValue(prev_vote_data, columnValue1);
                    if(nOldCount != columnValue2)
                    {
                        dictUpdateValue(prev_vote_data, columnValue1, columnValue2);
                        save_to_file(columnValue1, columnValue2);
                        printf("zapisane: %d, %d", nOldCount, columnValue2);
                        Sleep(100);

                    }
                }
                printf("Row %d: %d\n", columnValue1, columnValue2);
            }

            printf("Total rows fetched: %d\n", row);
        }
        else {
            fprintf(stderr, "Failed to execute the UPDATE query.\n");
        }

        // Free the statement handle
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);


        // Disconnect from the database
        SQLDisconnect(dbc);
    }
    else {
        fprintf(stderr, "Failed to connect to the MDB database.\n");
    }

    // Free the connection handle and environment
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);

    return 0;

}
int waitForChange()
{
    HANDLE hChange;
    DWORD dwWaitStatus;
    TCHAR szDir[MAX_PATH] = "D:\\zdyskuH\\WINvote\\DATA";
    TCHAR szFile[MAX_PATH] = "MachineTally.mdb";

    // ustowienie hendlera zmiany
    hChange = FindFirstChangeNotification(szDir, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
    if (hChange == INVALID_HANDLE_VALUE) {
        printf("Błąd FindFirstChangeNotification (%d)\n", GetLastError());
        return 1;
    }

    printf("Monitorowanie pliku moj.mdb...\n");

    // Pętla oczekiwania na zmianę
    while (1) {
        dwWaitStatus = WaitForSingleObject(hChange, INFINITE);
        if (dwWaitStatus == WAIT_OBJECT_0) {
            printf("Plik moj.mdb został zmieniony!\n");
            FindCloseChangeNotification(hChange);
            return 0;

            // Wykonaj swoje akcje tutaj po zmianie pliku
            char data[] = "Zminanaa\n";
           // save_to_file(data);
           // check_db(first_run, prev_vote_data);
            if (FindNextChangeNotification(hChange) == FALSE) {
                printf("Błąd FindNextChangeNotification (%d)\n", GetLastError());
                break;
            }
        } else {
            printf("Błąd WaitForSingleObject (%d)\n", GetLastError());
            break;
        }
    }

    // Zamknięcie uchwytu monitorowania zmian
    FindCloseChangeNotification(hChange);
    return 0;

}
int main() {
    Dictionary* prev_vote_data = dictCreate();
    int first_run = 1;
    check_db(first_run, prev_vote_data);
    first_run = 0;

    while(1)
    {
        waitForChange();
        check_db(first_run, prev_vote_data);


    }

    return 0;
}
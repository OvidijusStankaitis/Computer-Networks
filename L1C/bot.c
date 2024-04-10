#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <libpq-fe.h>
#include <time.h>

#define MAX_QUESTIONS 20
#define MAX_QUESTION_LENGTH 256

char questions[MAX_QUESTIONS][MAX_QUESTION_LENGTH] = {
    "# What is the capital of France?",
    "# What year is it?",
    "# Who wrote 'To Kill a Mockingbird'?",
    "# What is the speed of light?",
    "# Who is known as the father of computers?",
    "# What is the largest ocean on Earth?",
    "# Who painted the Mona Lisa?",
    "# What element does 'O' represent on the periodic table?",
    "# What is the highest mountain in the world?",
    "# Who discovered penicillin?",
    "# What is the capital of Japan?",
    "# How many continents are there?",
    "# What is the currency of the European Union?",
    "# Who is the author of '1984'?",
    "# What gas do plants absorb from the atmosphere?",
    "# What is the largest planet in our solar system?",
    "# What is the formula for water?",
    "# Who invented the telephone?",
    "# What is the longest river in the world?",
    "# What is the distance between the Earth and the Moon?"};

const char *DB_HOST = "localhost";
const char *DB_USER = "ovidijus";
const char *DB_NAME = "ovidijus";
const char *DB_PASSWORD = "12345678";
const char *DB_PORT = "5432";

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 20000
#define BOT_NAME "BOT'AS\n"

int CURRENT_ASKED_QUESTION = 0;
PGconn *dbConnection;

void initializeDatabaseConnection(void);
void *sendQuestions(void *arg);
void connectToServer(void);
int storeQuestion(const char *question);
void *listenForMessages(void *arg);
void storeAnswer(const char *answer, int questionId, const char *username);
void sendUserInfo(int sock, const char *username);

int main()
{
    initializeDatabaseConnection();

    connectToServer();

    PQfinish(dbConnection);
    return 0;
}

void initializeDatabaseConnection(void)
{
    char conninfo[256];
    snprintf(conninfo, sizeof(conninfo), "host=%s port=%s dbname=%s user=%s password=%s",
             DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASSWORD);

    dbConnection = PQconnectdb(conninfo);

    if (PQstatus(dbConnection) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(dbConnection));
        PQfinish(dbConnection);
        exit(1);
    }

    PGresult *res = PQexec(dbConnection, "TRUNCATE TABLE questions, answers RESTART IDENTITY;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Failed to clear tables: %s", PQerrorMessage(dbConnection));
    }
    PQclear(res);
}

void *sendQuestions(void *arg)
{
    int sock = *(int *)arg;

    for (int i = 0; i < MAX_QUESTIONS; i++)
    {
        CURRENT_ASKED_QUESTION = i + 1;

        char question[MAX_QUESTION_LENGTH + 1];
        memset(question, 0, sizeof(question));
        strncpy(question, questions[i], MAX_QUESTION_LENGTH - 2);
        size_t len = strlen(question);
        question[len] = '\n';
        question[len + 1] = '\0';

        send(sock, question, strlen(question), 0);

        storeQuestion(question);

        sleep(20);
    }

    send(sock, "BOT is shutting down.\n", 22, 0);
    exit(0);

    return NULL;
}

void connectToServer(void)
{
    int sock;
    struct sockaddr_in serverAddr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    send(sock, BOT_NAME, strlen(BOT_NAME), 0);

    pthread_t questionThreadId;
    if (pthread_create(&questionThreadId, NULL, sendQuestions, (void *)&sock) != 0)
    {
        perror("Failed to create question sending thread");
        exit(EXIT_FAILURE);
    }

    pthread_t listenThreadId;
    if (pthread_create(&listenThreadId, NULL, listenForMessages, (void *)&sock) != 0)
    {
        perror("Failed to create message listening thread");
        exit(EXIT_FAILURE);
    }

    pthread_join(questionThreadId, NULL);
    pthread_join(listenThreadId, NULL);
    close(sock);
}

int storeQuestion(const char *question)
{
    int lastQuestionId = -1;

    char *escapedQuestion = PQescapeLiteral(dbConnection, question, strlen(question));
    if (escapedQuestion == NULL)
    {
        fprintf(stderr, "Failed to escape question: %s\n", PQerrorMessage(dbConnection));
        return lastQuestionId;
    }

    char query[1024];
    snprintf(query, sizeof(query), "INSERT INTO questions (question) VALUES (%s) RETURNING id", escapedQuestion);

    PGresult *res = PQexec(dbConnection, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Inserting question failed: %s", PQerrorMessage(dbConnection));
    }
    else
    {
        lastQuestionId = atoi(PQgetvalue(res, 0, 0));
    }

    PQclear(res);
    PQfreemem(escapedQuestion);

    return lastQuestionId;
}

void *listenForMessages(void *arg)
{
    int sock = *(int *)arg;
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0)
        {
            break;
        }

        buffer[bytesReceived] = '\0';

        if (strstr(buffer, "BOT'AS:") != NULL)
        {
            continue;
        }

        char *msgStart = strstr(buffer, "PRANESIMAS ");
        if (msgStart != NULL)
        {
            char *username = msgStart + 10;
            char *colonPos = strchr(username, ':');
            if (colonPos != NULL)
            {
                *colonPos = '\0';
                char *message = colonPos + 2;

                if (strncmp(message, "#ANS", 4) == 0)
                {
                    char *answer = message + 5;
                    storeAnswer(answer, CURRENT_ASKED_QUESTION, username);
                }
                else if (strncmp(message, "#INFO", 5) == 0)
                {
                    sendUserInfo(sock, username);
                }
            }
        }
    }

    return NULL;
}

void storeAnswer(const char *answer, int questionId, const char *username)
{
    char *escapedAnswer = PQescapeLiteral(dbConnection, answer, strlen(answer));
    char *escapedUsername = PQescapeLiteral(dbConnection, username, strlen(username));
    if (escapedAnswer == NULL || escapedUsername == NULL)
    {
        fprintf(stderr, "Failed to escape answer or username: %s\n", PQerrorMessage(dbConnection));
        return;
    }

    char query[1024];
    snprintf(query, sizeof(query), "INSERT INTO answers (answer, question_id, username) VALUES (%s, %d, %s)", escapedAnswer, questionId, escapedUsername);

    PGresult *res = PQexec(dbConnection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Inserting answer failed: %s\n", PQerrorMessage(dbConnection));
    }

    PQclear(res);
    PQfreemem(escapedAnswer);
    PQfreemem(escapedUsername);
}

void sendUserInfo(int sock, const char *username)
{
    char *escapedUsername = PQescapeLiteral(dbConnection, username, strlen(username));
    if (escapedUsername == NULL)
    {
        fprintf(stderr, "Failed to escape username: %s\n", PQerrorMessage(dbConnection));
        return;
    }

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT q.question, a.answer FROM questions q "
             "JOIN answers a ON q.id = a.question_id "
             "WHERE a.username = %s ORDER BY q.time_asked",
             escapedUsername);

    PGresult *res = PQexec(dbConnection, query);
    if (PQresultStatus(res) == PGRES_TUPLES_OK)
    {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i)
        {
            char questionMsg[2048];
            char answerMsg[2048];
            const char *question = PQgetvalue(res, i, 0);
            const char *answer = PQgetvalue(res, i, 1);

            snprintf(questionMsg, sizeof(questionMsg), "Q: %s\n", question);
            send(sock, questionMsg, strlen(questionMsg), 0);
            usleep(100000);

            snprintf(answerMsg, sizeof(answerMsg), "A: %s\n", answer ? answer : "No answer");
            send(sock, answerMsg, strlen(answerMsg), 0);
            usleep(100000);
        }
    }

    PQclear(res);
    PQfreemem(escapedUsername);
}
CFLAGS = -Wall -Wextra

CLIENT_TARGET = client
SERVER_TARGET = server

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): client.c
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) client.c

$(SERVER_TARGET): server.c
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) server.c

run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)
CC = gcc
CFLAGS = -I/usr/include/postgresql
LDFLAGS = -lpq

all: chat_server bot

chat_server: chat_server.c
	$(CC) $(CFLAGS) -o chat_server chat_server.c
	clear

bot: bot.c
	$(CC) $(CFLAGS) -o bot bot.c $(LDFLAGS)
	clear

clean:
	rm -f chat_server bot
	clear

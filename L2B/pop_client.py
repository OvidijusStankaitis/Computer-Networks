import poplib
import os
from pop_commands import *
import threading

pop_server = 'outlook.office365.com'
port = 995  
username = ''  
password = ''  

server = poplib.POP3_SSL(pop_server, port)
server.user(username)
server.pass_(password)

noop_condition = threading.Condition()

noop_thread = threading.Thread(target=handle_noop_command, args=(server, noop_condition))
noop_thread.start()

while True:
    print("\nAvailable Commands:")
    print("STAT, LIST, RETR, DELE, RSET, QUIT")
    command = input("Enter command: ").upper()
    if command == "QUIT":
        print("Connection closed.")
        with noop_condition:
            noop_condition.notify()  # Notify the thread that it should stop
        noop_thread.join()  # Wait for the thread to finish
        server.quit()
        break
    elif command == "STAT":
        handle_stat_command(server)
    elif command == "LIST":
        handle_list_command(server)
    elif command == "RETR":
        handle_retr_command(server)
    elif command == "DELE":
        handle_dele_command(server)
    elif command == "RSET":
        handle_rset_command(server)
    elif command == "CLEAR":
        os.system('clear')
    else:
        print("Unknown command.")


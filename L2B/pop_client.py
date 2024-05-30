import socket
import ssl
import os
from pop_commands import *

pop_server = 'outlook.office365.com'
port = 995  
username = ''  
password = ''  

context = ssl.create_default_context()
with socket.create_connection((pop_server, port)) as sock:
    with context.wrap_socket(sock, server_hostname=pop_server) as server:
        server.settimeout(10)

        recv(server)  
        send(server, f'USER {username}\r\n')
        recv(server)
        send(server, f'PASS {password}\r\n')
        recv(server)

        while True:
            print("\nAvailable Commands:")
            print("STAT - Shows the total number of emails and combined size of all emails in your mailbox.")
            print("LIST - Lists all emails with their number and size, or shows size of a specific email if a number is given.")
            print("RETR - Retrieves the full content of a specified email.")
            print("DELE - Marks a specified email for deletion.")
            print("RSET - Undoes any emails marked for deletion during this session.")
            print("QUIT - Closes the connection to the server and exits the program.")
            command = input("Enter command: ").upper()
            
            if command == "QUIT":
                print("Connection closed.")
                send(server, 'QUIT\r\n')
                recv(server)
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
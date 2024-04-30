from email.parser import Parser
import time

def handle_retr_command(server):
    input_msg_num = input("Enter message number to retrieve: ").strip()

    if not input_msg_num.isdigit() or int(input_msg_num) < 1:
        print("Error: Invalid message number entered.")
        return 

    msg_num = int(input_msg_num)

    lines = server.retr(msg_num)[1]
    msg_data = b"\n".join(lines).decode('utf-8')  
    msg = Parser().parsestr(msg_data)  

    if msg.is_multipart():
        for part in msg.walk():
            if part.get_content_type() == 'text/plain':
                print("Email content:\n", part.get_payload(decode=True).decode('utf-8'))
    else:
        print("Email content:\n", msg.get_payload(decode=True).decode('utf-8'))

def handle_list_command(server):
    msg_number = input("Enter message number or leave blank for all messages: ").strip()
    if msg_number:
        msg_number = int(msg_number)
        listing = server.list(msg_number)
        print("Scan listing for message {0}: {1}".format(msg_number, listing.decode('utf-8')))
    else:
        listings = server.list()[1]
        print("Total messages: {0}".format(len(listings)))
        number = 1
        for listing in listings:
            print("Scan listing for message {0}: {1}".format(number, listing.decode('utf-8')))
            number += 1

def handle_noop_command(server, condition):
    with condition:
        while True:
            server.noop()
            if condition.wait(timeout=5): 
                break

def handle_stat_command(server):
    message_count, total_size = server.stat()
    print("Mailbox status: {0} messages, combined size of {1} bytes.".format(message_count, total_size))

def handle_dele_command(server):
    msg_num = int(input("Enter message number to delete: "))
    server.dele(msg_num)
    print(f"Message {msg_num} marked for deletion.")

def handle_rset_command(server):
    server.rset()
    print("Reset state, undeleted all deleted messages.")
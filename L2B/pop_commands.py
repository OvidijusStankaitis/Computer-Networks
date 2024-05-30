import os
import email
from email.parser import Parser
from email.policy import default
import re

keyword = "KOMPASGOOD"
secret_contacts = ['kompiuteriskompiuteriskis@gmail.com', 'ovidijus.stankaitis@mif.stud.vu.lt']

def extract_email(sender):
    """Extract email address from sender string."""
    match = re.search(r'<([^>]+)>', sender)
    if match:
        return match.group(1)
    return sender  # Return the original string if no email format found

def send(server, message):
    """ Send a message to the server. """
    server.sendall(message.encode())

def recv(server):
    """ Receive a message from the server. """
    response = server.recv(4096).decode()
    print(response)
    return response

def handle_stat_command(server):
    """ Handle the STAT command which shows the total number of emails and the combined size. """
    send(server, 'STAT\r\n')
    return recv(server)

def handle_list_command(server):
    """ Handle the LIST command which lists all emails or a specific email's size. """
    email_number = input("Enter email number or press Enter for all emails: ")
    if email_number.strip():
        send(server, f'LIST {email_number}\r\n')
    else:
        send(server, 'LIST\r\n')
    return recv(server)

def recv_complete_message(server):
    """ Receive the complete message until the POP3 end-of-message delimiter '\r\n.\r\n' is found. """
    response_parts = []
    while True:
        part = server.recv(4096).decode()
        response_parts.append(part)
        if part.endswith('\r\n.\r\n'):
            break
    full_response = ''.join(response_parts)
    return full_response

def create_cipher_alphabet(keyword):
    """Create the substitution alphabet based on the keyword."""
    seen = set()
    filtered_keyword = ''.join([char for char in keyword.upper() if char not in seen and not seen.add(char)])
    alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    remaining_letters = ''.join([char for char in alphabet if char not in filtered_keyword])
    cipher_alphabet = filtered_keyword + remaining_letters
    return cipher_alphabet

def decrypt_text(text, keyword):
    """Decrypt text using the substitution cipher."""
    cipher_alphabet = create_cipher_alphabet(keyword)
    standard_alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    mapping = dict(zip(cipher_alphabet, standard_alphabet))
    result = ''
    for char in text.upper():
        if char in mapping:
            result += mapping[char]
        else:
            result += char  # Non-alphabetic characters are copied as is
    return result

def handle_retr_command(server):
    """Handle the RETR command to retrieve the full content of a specified email."""
    email_number = input("Enter email number to retrieve: ")
    send(server, f'RETR {email_number}\r\n')
    email_content = recv_complete_message(server)

    # Remove the initial server response (+OK)
    email_content = '\n'.join(email_content.split('\n')[1:])

    # Parse the email content
    message = email.message_from_string(email_content, policy=default)
    sender = message.get('From')
    subject = message.get('Subject')
    body = ""
    attachments = []

    if message.is_multipart():
        for part in message.walk():
            content_disposition = part.get('Content-Disposition')
            if part.get_content_type() == 'text/plain' and (content_disposition is None or 'attachment' not in content_disposition):
                body = part.get_payload(decode=True).decode('utf-8')
            elif content_disposition and 'attachment' in content_disposition:
                filename = part.get_filename()
                file_data = part.get_payload(decode=True)
                filepath = os.path.join('/home/ovidijus/Desktop/KT/L2B', filename)
                with open(filepath, 'wb') as f:
                    f.write(file_data)
                attachments.append(filepath)
    else:
        body = message.get_payload(decode=True).decode('utf-8')

    # Extract email from sender
    email_address = extract_email(sender)
    print("\nSender:", email_address)
    
    # Check if the email address is in secret contacts
    if email_address in secret_contacts:
        subject = decrypt_text(subject, keyword)
        body = decrypt_text(body, keyword)
        print("Decrypted Subject:", subject)
        print("Decrypted Text:", body)
    else:
        print("Subject:", subject)
        print("Text:", body.strip())

    if attachments:
        print("Attachments saved:", ', '.join(attachments))
    else:
        print("No attachments found.")

def handle_dele_command(server):
    """ Handle the DELE command to mark a specified email for deletion. """
    email_number = input("Enter email number to delete: ")
    send(server, f'DELE {email_number}\r\n')
    return recv(server)

def handle_rset_command(server):
    """ Handle the RSET command to undo any deletions marked during this session. """
    send(server, 'RSET\r\n')
    return recv(server)

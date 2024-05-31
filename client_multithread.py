import socket

def start_client(server_ip, message):
    server_address = (server_ip, 65432)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect(server_address)
        print(f"Connected to server at {server_address}")
        
        # Send a message
        message_data = message.encode('utf-8')
        message_length = len(message_data).to_bytes(4, 'big')
        client_socket.sendall(message_length + message_data)
        
        count = 0
        ack = '1'
        f = open("cipher_" + message + "_test.txt", "w")
        while count < 129600:
            count += 1
            # Receive the response
            header = client_socket.recv(4)
            if header:
                response_length = int.from_bytes(header, 'big')
                response_data = client_socket.recv(response_length)
                print(f"Received response from server: {response_data.decode('utf-8')}")
                client_socket.sendall(ack.encode('utf-8'))
                print(count)
                f.write(response_data.decode('utf-8'))
                f.write("\n")

        print(f"Data {message.upper()} count: {count}\n")
        f.close()
        
if __name__ == "__main__":
    server_ip = input("Enter the server's IP address: ")
    message = input("Enter the message to send: ")
    start_client(server_ip, message)

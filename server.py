import socket
import threading
import time

def handle_client(connection, client_address):
    try:
        print(f"Connection from {client_address}")
        connection.settimeout(15)  # Set a timeout for socket operations

        while True:
            # Receive the message length first
            header = connection.recv(4)
            if not header:
                print(f"No data received from {client_address}. Closing connection.")
                break
            message_length = int.from_bytes(header, 'big')
            
            # Receive the actual message
            data = connection.recv(message_length)
            if not data:
                print(f"No message received from {client_address}. Closing connection.")
                break
            print(f"Received message: {data.decode('utf-8')}")
            
            # Time start
            start = time.time()

            with open ('./dat/ciphertext_b.dat', 'r') as f:
                count = 0
                for line in f:
                    msg=line.strip('\n')
                    msg_len = len(msg).to_bytes(4, 'big')
                    connection.sendall(msg_len + msg.encode('utf-8'))
                    count = count + 1
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
            
            time.sleep(1)

            with open('./dat/ciphertext_g.dat', 'r') as f:
                count = 0
                for line in f:
                    msg=line.strip('\n')
                    msg_len = len(msg).to_bytes(4, 'big')
                    connection.sendall(msg_len + msg.encode('utf-8'))
                    count = count + 1
                    while connection.recv(1).decode('utf-8') != '1':
                        pass

            time.sleep(1)
            
            with open('./dat/ciphertext_r.dat', 'r') as f:
                count = 0
                for line in f:
                    msg=line.strip('\n')
                    msg_len = len(msg).to_bytes(4, 'big')
                    connection.sendall(msg_len + msg.encode('utf-8'))
                    count = count + 1
                    while connection.recv(1).decode('utf-8') != '1':
                        pass

            time.sleep(1)

            time_end = time.time()

    except socket.timeout:
        print(f"Connection with {client_address} timed out.")
    except Exception as e:
        print(f"An error occurred with {client_address}: {e}")        
    finally:
        connection.close()
        print(f"Connection with {client_address} closed")
        print(f"Time taken: {time_end - start:.2f} seconds")

def start_server():
    server_address = ('0.0.0.0', 65432)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind(server_address)
        server_socket.listen(5)
        print(f"Server started and listening on {server_address}")
        
        while True:
            connection, client_address = server_socket.accept()
            client_thread = threading.Thread(target=handle_client, args=(connection, client_address))
            client_thread.start()

if __name__ == "__main__":
    start_server()

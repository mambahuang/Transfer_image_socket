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
            with open('./dat/ciphertext_b.dat', 'r') as f1:
                    cipher_b = f1.read()
            cipher_b = cipher_b.split('\n')
            with open('./dat/ciphertext_g.dat', 'r') as f2:
                    cipher_g = f2.read()
            cipher_g = cipher_g.split('\n')
            with open('./dat/ciphertext_r.dat', 'r') as f3:
                    cipher_r = f3.read()
            cipher_r = cipher_r.split('\n')
            if data.decode('utf-8') == "b1":
                for b_idx in range(32400):
                    msg_len = len(cipher_b[b_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_b[b_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "b2":
                for b_idx in range(32400*1, 32400*2, 1):
                    msg_len = len(cipher_b[b_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_b[b_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "b3":
                for b_idx in range(32400*2, 32400*3, 1):
                    msg_len = len(cipher_b[b_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_b[b_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "b4":
                for b_idx in range(32400*3, 32400*4, 1):
                    msg_len = len(cipher_b[b_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_b[b_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "g1":
                for g_idx in range(32400):
                    msg_len = len(cipher_g[g_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_g[g_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "g2":
                for g_idx in range(32400*1, 32400*2, 1):
                    msg_len = len(cipher_g[g_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_g[g_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "g3":
                for g_idx in range(32400*2, 32400*3, 1):
                    msg_len = len(cipher_g[g_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_g[g_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "g4":
                for g_idx in range(32400*3, 32400*4, 1):
                    msg_len = len(cipher_g[g_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_g[g_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "r1":
                for r_idx in range(32400):
                    msg_len = len(cipher_r[r_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_r[r_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "r2":
                for r_idx in range(32400*1, 32400*2, 1):
                    msg_len = len(cipher_r[r_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_r[r_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "r3":
                for r_idx in range(32400*2, 32400*3, 1):
                    msg_len = len(cipher_r[r_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_r[r_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)
            elif data.decode('utf-8') == "r4":
                for r_idx in range(32400*3, 32400*4, 1):
                    msg_len = len(cipher_r[r_idx]).to_bytes(4, 'big')
                    connection.sendall(msg_len + cipher_r[r_idx].encode('utf-8'))
                    while connection.recv(1).decode('utf-8') != '1':
                        pass
                time.sleep(1)

            time_end = time.time()
            f1.close()
            f2.close()
            f3.close()

    except socket.timeout:
        print(f"Connection with {client_address} timed out.")
    except Exception as e:
        print(f"An error occurred with {client_address}: {e}")        
    finally:
        connection.close()
        print(f"Time taken: {time_end - start:.2f} seconds")
        print(f"Connection with {client_address} closed")

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

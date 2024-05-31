"""
create    plaintext_b.dat, plaintext_g.dat, plaintext_r.dat --> image_padding.png rgb channels
          key.dat, ciphertext_b.dat, ciphertext_g.dat, ciphertext_r.dat
          decrypted_b.dat, decrypted_g.dat, decrypted_r.dat --> verify plaintext and decrypted text are the same
"""

from Crypto.Cipher import AES
import cv2
import numpy as np

# ------------ Custom key (16 bytes) ------------
def custom_key(key):
    key_base = [ord(c) for c in key]
    tmp = []
    key = []
    for j in range(129600):
        for i in key_base:
            tmp.append(hex(i)[2:].zfill(2))
        key.append(tmp)
        tmp = []
    return key

# ------------ Image to hex array ------------
def image_hex_array(image_path):
    image = cv2.imread(image_path)
    b, g, r = cv2.split(image)
    # image_bytes = image.tobytes()
    r_arr = np.array(r)
    g_arr = np.array(g)
    b_arr = np.array(b)

    plaintext_b=[]
    for i in range(1080):
        tmp_b=[]
        for j in range(1920):
            plaintext_byte=b_arr[i][j]
            tmp_b.append(hex(plaintext_byte)[2:].zfill(2))
            if len(tmp_b) == 16:
              plaintext_b.append(tmp_b)
              tmp_b = []
    plaintext_g=[]
    for i in range(1080):
        tmp_g=[]
        for j in range(1920):
            plaintext_byte=g_arr[i][j]
            tmp_g.append(hex(plaintext_byte)[2:].zfill(2))
            if len(tmp_g) == 16:
              plaintext_g.append(tmp_g)
              tmp_g = []
    plaintext_r=[]
    for i in range(1080):
        tmp_r=[]
        for j in range(1920):
            plaintext_byte=r_arr[i][j]
            tmp_r.append(hex(plaintext_byte)[2:].zfill(2))
            if len(tmp_r) == 16:
              plaintext_r.append(tmp_r)
              tmp_r = []

    # print(plaintext_r[0:5])
    print(r_arr.shape)
    return plaintext_b, plaintext_g, plaintext_r

plaintext_b, plaintext_g, plaintext_r = image_hex_array("./img/image_padding.png")

# ----------------- Encrypt & Write file -----------------
def Encrypt(plaintext_b, plaintext_g, plaintext_r, key):
    
    f1=open("./dat/plaintext_b.dat","w")
    f2=open("./dat/plaintext_g.dat","w")
    f3=open("./dat/plaintext_r.dat","w")
    f4=open("./dat/key.dat","w")
    f5=open("./dat/ciphertext_b.dat","w")
    f6=open("./dat/ciphertext_g.dat","w")
    f7=open("./dat/ciphertext_r.dat","w")
    ciphertext_global_b = []
    ciphertext_global_g = []
    ciphertext_global_r = []

    for i in range(129600):
        current_plaintext_b=[]
        current_plaintext_g=[]
        current_plaintext_r=[]

        for j in range(16):
            f1.write(plaintext_b[i][j])
            current_plaintext_b.append(int(plaintext_b[i][j],16))

        f1.write("\n")

        for j in range(16):
            f2.write(plaintext_g[i][j])
            current_plaintext_g.append(int(plaintext_g[i][j],16))

        f2.write("\n")

        for j in range(16):
            f3.write(plaintext_r[i][j])
            current_plaintext_r.append(int(plaintext_r[i][j],16))

        f3.write("\n")

        current_key=[]

        for j in range(16):
            f4.write(key[i][j])
            current_key.append(int(key[i][j],16))

        f4.write("\n")

        cipher=AES.new(bytes(current_key), AES.MODE_ECB)
        ciphertext_b=cipher.encrypt(bytes(current_plaintext_b))
        ciphertext_g=cipher.encrypt(bytes(current_plaintext_g))
        ciphertext_r=cipher.encrypt(bytes(current_plaintext_r))
        ciphertext_global_b.append(ciphertext_b)
        ciphertext_global_g.append(ciphertext_g)
        ciphertext_global_r.append(ciphertext_r)

    #     print(ciphertext.hex())
        f5.write(ciphertext_b.hex())
        f5.write("\n")
        f6.write(ciphertext_g.hex())
        f6.write("\n")  
        f7.write(ciphertext_r.hex())
        f7.write("\n")

    f1.close()
    f2.close()
    f3.close()
    f4.close()
    f5.close()
    f6.close()
    f7.close()

    return ciphertext_global_b, ciphertext_global_g, ciphertext_global_r

# ----------------- Decrypt & Write file -----------------
def Decrypt(ciphertext_global_b, ciphertext_global_g, ciphertext_global_r, key):
    f1=open("./dat/decrypted_b.dat","w")
    f2=open("./dat/decrypted_g.dat","w")
    f3=open("./dat/decrypted_r.dat","w")

    for i in range(129600):
        current_key=[]

        for j in range(16):
            current_key.append(int(key[i][j],16))

        cipher=AES.new(bytes(current_key), AES.MODE_ECB)
        decrypted_b=cipher.decrypt(ciphertext_global_b[i])
        decrypted_g=cipher.decrypt(ciphertext_global_g[i])
        decrypted_r=cipher.decrypt(ciphertext_global_r[i])

        f1.write(decrypted_b.hex())
        f1.write("\n")
        f2.write(decrypted_g.hex())
        f2.write("\n")
        f3.write(decrypted_r.hex())
        f3.write("\n")

    f1.close()
    f2.close()
    f3.close()
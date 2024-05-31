def re_formatFile():
    # open file
    f1 = open('cipher_b_test.txt', 'r')
    f2 = open('cipher_g_test.txt', 'r')
    f3 = open('cipher_r_test.txt', 'r')

    # create new files and write to them
    f4 = open('./header/cipher_b_test.h', 'w')
    f5 = open('./header/cipher_g_test.h', 'w')
    f6 = open('./header/cipher_r_test.h', 'w')

    # write to the blue file
    f4.write("#ifndef CIPHER_B_H_\n")
    f4.write("#define CIPHER_B_H_\n")
    f4.write("\n")
    f4.write("#define HSize 1920\n")
    f4.write("#define VSize 1080\n")
    f4.write("#define PixelFrameSize HSize*VSize\n")
    f4.write("\n")
    f4.write("unsigned char cipher_b[PixelFrameSize] = {\n")
    for line in f1:
        for idx in range(0, len(line) - 2, 2):
            f4.write("0x" + line[idx:idx+2] + ", ")
        f4.write("\n")
    f4.write("\n};\n")
    f4.write("\n#endif")

    # write to the green file
    f5.write("#ifndef CIPHER_G_H_\n")
    f5.write("#define CIPHER_G_H_\n")
    f5.write("\n")
    f5.write("#define HSize 1920\n")
    f5.write("#define VSize 1080\n")
    f5.write("#define PixelFrameSize HSize*VSize\n")
    f5.write("\n")
    f5.write("unsigned char cipher_g[PixelFrameSize] = {\n")
    for line in f2:
        for idx in range(0, len(line) - 2, 2):
            f5.write("0x" + line[idx:idx+2] + ", ")
        f5.write("\n")
    f5.write("\n};\n")
    f5.write("\n#endif")

    # write to the red file
    f6.write("#ifndef CIPHER_R_H_\n")
    f6.write("#define CIPHER_R_H_\n")
    f6.write("\n")
    f6.write("#define HSize 1920\n")
    f6.write("#define VSize 1080\n")
    f6.write("#define PixelFrameSize HSize*VSize\n")
    f6.write("\n")
    f6.write("unsigned char cipher_r[PixelFrameSize] = {\n")

    for line in f3:
        for idx in range(0, len(line) - 2, 2):
            f6.write("0x" + line[idx:idx+2] + ", ")
        f6.write("\n")
    f6.write("\n};\n")
    f6.write("\n#endif")
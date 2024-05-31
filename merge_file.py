def merge_file():
    f1 = open('cipher_b_test.txt', 'w')
    f1_1 = open('cipher_b1_test.txt', 'r')
    f1_2 = open('cipher_b2_test.txt', 'r')
    f1_3 = open('cipher_b3_test.txt', 'r')
    f1_4 = open('cipher_b4_test.txt', 'r')

    f2 = open('cipher_g_test.txt', 'w')
    f2_1 = open('cipher_g1_test.txt', 'r')
    f2_2 = open('cipher_g2_test.txt', 'r')
    f2_3 = open('cipher_g3_test.txt', 'r')
    f2_4 = open('cipher_g4_test.txt', 'r')

    f3 = open('cipher_r_test.txt', 'w')
    f3_1 = open('cipher_r1_test.txt', 'r')
    f3_2 = open('cipher_r2_test.txt', 'r')
    f3_3 = open('cipher_r3_test.txt', 'r')
    f3_4 = open('cipher_r4_test.txt', 'r')

    for line in f1_1:
        f1.write(line)
    for line in f1_2:
        f1.write(line)
    for line in f1_3:
        f1.write(line)
    for line in f1_4:
        f1.write(line)

    f1.close()
    f1_1.close()
    f1_2.close()
    f1_3.close()
    f1_4.close()

    for line in f2_1:
        f2.write(line)
    for line in f2_2:
        f2.write(line)
    for line in f2_3:
        f2.write(line)
    for line in f2_4:
        f2.write(line)

    f2.close()
    f2_1.close()
    f2_2.close()
    f2_3.close()
    f2_4.close()

    for line in f3_1:
        f3.write(line)
    for line in f3_2:
        f3.write(line)
    for line in f3_3:
        f3.write(line)
    for line in f3_4:
        f3.write(line)

    f3.close()
    f3_1.close()
    f3_2.close()
    f3_3.close()
    f3_4.close()

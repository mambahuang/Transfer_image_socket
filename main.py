import sys
import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import numpy as np
from PyQt5.QtCore import QCoreApplication
from PyQt5 import QtCore, QtWidgets, QtGui
from PyQt5.QtWidgets import QMainWindow, QLabel, QGridLayout, QWidget
from PyQt5.QtWidgets import QPushButton
from PyQt5.QtCore import QSize
from PyQt5.QtGui import *

from UI import Ui_Dialog
from graffiti import GraffitiDialog
from image_padding import image_to_hex, pad_image_center, save_hex_image
from aes import image_hex_array, custom_key, Encrypt, Decrypt

class MainWindow(QMainWindow, Ui_Dialog):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle("Signature Pad")
        self.lb_pwd_confirm.hide()
        
        # confirm & reset buttons
        self.btn_confirm.clicked.connect(self.on_confirm_clicked)
        self.btn_reset.clicked.connect(self.on_reset_clicked)
        self.btn_quit.clicked.connect(QCoreApplication.instance().quit)
        self.btn_pwd_confirm.clicked.connect(self.on_pwd_confirm_clicked)
        self.lb_ds_img = GraffitiDialog(self.frame1_4)

    def on_confirm_clicked(self):
        GraffitiDialog.save(self.lb_ds_img, "./img/output.png")
        input_image_path = "./img/output.png"
        output_image_path = './img/image_padding.png'

        # Convert the original image to hex
        hex_values = image_to_hex(input_image_path)

        # Pad the hex values to 1920x1080 centered
        padded_hex_image = pad_image_center(hex_values, 1920, 1080)

        # Save the padded hex image
        save_hex_image(padded_hex_image, output_image_path)

        print("Image processing complete!")
        self.process_img(output_image_path)

    def on_reset_clicked(self):
        GraffitiDialog.clear(self.lb_ds_img)
        self.lb_state.setText("Button Reset Clicked!")

    def on_pwd_confirm_clicked(self):
        self.lb_pwd_confirm.setText("Password Saved!")
        self.lb_pwd_confirm.show()
        imgKey = self.pwd_le.text()
        imgKey_confirm = self.pwd_confirm_le.text()
        while imgKey != imgKey_confirm:
            self.lb_pwd_confirm.setText("Password Mismatch!")
            self.lb_pwd_confirm.show()
            imgKey = self.pwd_le.text()
            imgKey_confirm = self.pwd_confirm_le.text()
        self.lb_pwd_confirm.setText("Password Confirmed!")
        key = custom_key(imgKey)
        plaintext_b, plaintext_g, plaintext_r = image_hex_array("./img/image_padding.png")
        ciphertext_global_b, ciphertext_global_g, ciphertext_global_r = Encrypt(plaintext_b, plaintext_g, plaintext_r, key)
        print("Encryption complete!")
        Decrypt(ciphertext_global_b, ciphertext_global_g, ciphertext_global_r, key)
        print("Decryption complete!")

    def process_img(self, img_path):
        img = cv2.imread(img_path)
        print(f"Image Hight is: {img.shape[0]}\nImage Width is: {img.shape[1]}")
        b, g, r = cv2.split(img)
        # print(img.shape)

        r_arr = np.array(r)
        g_arr = np.array(g)
        b_arr = np.array(b)
        img_arr = np.zeros((img.shape[0], img.shape[1], 3), dtype=np.uint8)
        img_arr[:, :, 0] = b_arr
        img_arr[:, :, 1] = g_arr
        img_arr[:, :, 2] = r_arr
        # print(img_arr[:, :, 2])
        cv2.imshow("Original Image", img_arr)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

        # ---------------- VGA ------------------
        # with open ("C://Users//angel//project_paper//My_project//20231116_AES_tes//AES_test//AES_2//src//b_channel_alt.h", "w") as f:
        #     f.write("#ifndef SRC_B_CHANNEL_H_\n"
        #             "#define SRC_B_CHANNEL_H_\n\n")
            
        #     f.write("u32 b_img[] = {\n\t\t")
        #     for i in range(b_arr.shape[0]):
        #         for j in range(b_arr.shape[1]):
        #             f.write("0x" + hex(b_arr[i][j])[2:] + ", ")
        #             if j % 16 == 15:
        #                 f.write("\n\t\t")
        #     f.write("\n};")
        #     f.write("\n\n#endif")

        # with open ("C://Users//angel//project_paper//My_project//20231116_AES_tes//AES_test//AES_2//src//g_channel_alt.h", "w") as f:
        #     f.write("#ifndef SRC_G_CHANNEL_H_\n"
        #             "#define SRC_G_CHANNEL_H_\n\n")
            
        #     f.write("u32 g_img[] = {\n\t\t")
        #     for i in range(g_arr.shape[0]):
        #         for j in range(g_arr.shape[1]):
        #             f.write("0x" + hex(g_arr[i][j])[2:] + ", ")
        #             if j % 16 == 15:
        #                 f.write("\n\t\t")
        #     f.write("\n};")
        #     f.write("\n\n#endif")

        # with open ("C://Users//angel//project_paper//My_project//20231116_AES_tes//AES_test//AES_2//src//r_channel_alt.h", "w") as f:
        #     f.write("#ifndef SRC_R_CHANNEL_H_\n"
        #             "#define SRC_R_CHANNEL_H_\n\n")
            
        #     f.write("u32 r_img[] = {\n\t\t")
        #     for i in range(r_arr.shape[0]):
        #         for j in range(r_arr.shape[1]):
        #             f.write("0x" + hex(r_arr[i][j])[2:] + ", ")
        #             if j % 16 == 15:
        #                 f.write("\n\t\t")
        #     f.write("\n};")
        #     f.write("\n\n#endif")
        # --------------------- VGA end ---------------------

        # ---------------------- HDMI ----------------------
        with open ("C://Users//angel//project_paper//20231201_VDMA_test//project_1//VDMA_app//src//b_channel_alt.h", "w") as f:
            f.write("#ifndef SRC_B_CHANNEL_H_\n"
                    "#define SRC_B_CHANNEL_H_\n\n")
            
            f.write("#define HSize 1920\n")
            f.write("#define VSize 1080\n")
            f.write("#define PixelFrameSize HSize*VSize\n\n")

            f.write("unsigned char b_img[PixelFrameSize] = {\n\t\t")
            for i in range(b_arr.shape[0]):
                for j in range(b_arr.shape[1]):
                    f.write("0x" + hex(b_arr[i][j])[2:] + ", ")
                    if j % 16 == 15:
                        f.write("\n\t\t")
            f.write("\n};")
            f.write("\n\n#endif")


        with open ("C://Users//angel//project_paper//20231201_VDMA_test//project_1//VDMA_app//src//g_channel_alt.h", "w") as f:
            f.write("#ifndef SRC_G_CHANNEL_H_\n"
                    "#define SRC_G_CHANNEL_H_\n\n")
            
            f.write("#define HSize 1920\n")
            f.write("#define VSize 1080\n")
            f.write("#define PixelFrameSize HSize*VSize\n\n")

            f.write("unsigned char g_img[PixelFrameSize] = {\n\t\t")
            for i in range(b_arr.shape[0]):
                for j in range(b_arr.shape[1]):
                    f.write("0x" + hex(b_arr[i][j])[2:] + ", ")
                    if j % 16 == 15:
                        f.write("\n\t\t")
            f.write("\n};")
            f.write("\n\n#endif")
        
        with open ("C://Users//angel//project_paper//20231201_VDMA_test//project_1//VDMA_app//src//r_channel_alt.h", "w") as f:
            f.write("#ifndef SRC_R_CHANNEL_H_\n"
                    "#define SRC_R_CHANNEL_H_\n\n")
            
            f.write("#define HSize 1920\n")
            f.write("#define VSize 1080\n")
            f.write("#define PixelFrameSize HSize*VSize\n\n")

            f.write("unsigned char r_img[PixelFrameSize] = {\n\t\t")
            for i in range(b_arr.shape[0]):
                for j in range(b_arr.shape[1]):
                    f.write("0x" + hex(b_arr[i][j])[2:] + ", ")
                    if j % 16 == 15:
                        f.write("\n\t\t")
            f.write("\n};")
            f.write("\n\n#endif")
        # ---------------------- HDMI end ----------------------
    
if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    mainWin = MainWindow()
    mainWin.show()
    sys.exit(app.exec_())
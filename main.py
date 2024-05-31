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

class MainWindow(QMainWindow, Ui_Dialog):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setWindowTitle("Signature Pad")
        
        # confirm & reset buttons
        self.btn_confirm.clicked.connect(self.on_confirm_clicked)
        self.btn_reset.clicked.connect(self.on_reset_clicked)
        self.btn_quit.clicked.connect(QCoreApplication.instance().quit)
        self.lb_ds_img = GraffitiDialog(self.frame1_4)

    def on_confirm_clicked(self):
        GraffitiDialog.save(self.lb_ds_img, "output.png")
        img_path = "output.png"
        self.process_img(img_path)

    def on_reset_clicked(self):
        GraffitiDialog.clear(self.lb_ds_img)
        self.lb_state.setText("Button Reset Clicked!")

    def process_img(self, img_path):
        img = cv2.imread(img_path)
        print(f"Image Hight is: {img.shape[0]}\nImage Width is: {img.shape[1]}")
        img_resize = cv2.resize(img, (1920, 1080))
        cv2.imwrite("output_resize.png", img_resize)
        img = cv2.imread("output_resize.png")
        # print(f"Image Hight is: {img.shape[0]}\nImage Width is: {img.shape[1]}")
        b, g, r = cv2.split(img)
        # print(img.shape)

        # cv2.imshow("Model Blue Image", b) 
        # cv2.imshow("Model Green Image", g)
        # cv2.imshow("Model Red Image", r)

        r_arr = np.array(r)
        g_arr = np.array(g)
        b_arr = np.array(b)
        img_arr = np.zeros((img.shape[0], img.shape[1], 3), dtype=np.uint8)
        img_arr[:, :, 0] = b_arr
        img_arr[:, :, 1] = g_arr
        img_arr[:, :, 2] = r_arr
        print(img_arr[:, :, 2])
        cv2.imshow("Original Image", img_arr)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
        # print(r_arr.shape)
        # print(g_arr[0])
        # print(b_arr[0])

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
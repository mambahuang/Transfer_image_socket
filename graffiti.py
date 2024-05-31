from PyQt5.QtGui import QPainter, QPen, QPixmap, QImage
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtWidgets import QWidget
from PyQt5 import QtWidgets
from PyQt5 import QtCore, QtWidgets, QtGui
import numpy as np

class GraffitiDialog(QWidget):
    def __init__(self, parent=None, x = 280, y = 120, width = 640, height = 480):
        super().__init__(parent)
        self.setGeometry(x, y, width, height)
        self.drawing = False
        self.lastPoint = None
        self.brushSize = 10
        self.image = QPixmap(width, height)
        self.image.fill(Qt.black)
        self.label = QtWidgets.QLabel(self)
        self.label.setFixedSize(390, 310)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawPixmap(0, 0, self.image)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.drawing = True
            self.lastPoint = event.pos()
    
    def mouseMoveEvent(self, event):
        if event.buttons() and Qt.LeftButton and self.drawing:
            painter = QPainter(self.image)
            painter.setPen(QPen(Qt.white, self.brushSize, Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin))
            painter.drawLine(self.lastPoint, event.pos())
            self.lastPoint = event.pos()
            self.update()

    def mouseReleaseEvent(self, event):
        if event.button == Qt.LeftButton:
            self.drawing = False

    def clear(self):
        self.image.fill(Qt.black)
        self.label.clear()
        self.update()

    def showImage(self):
        self.filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open a file', 
                                                         './', 
                                                         'All files(*.*)')
        self.pixmap = QtGui.QPixmap(self.filename[0])
        self.pixmap = self.pixmap.scaled(391, 311)
        self.label.setPixmap(self.pixmap)
        self.label.show()

    def getResizedImage(self):
        resized_image = self.image.scaled(QSize(32, 32), Qt.KeepAspectRatio)
        image = QImage(resized_image)
        image = image.convertToFormat(QImage.Format_RGB888)  # Convert to a format easily usable by NumPy
        width = image.width()
        height = image.height()

        ptr = image.bits()
        ptr.setsize(image.byteCount())
        arr = np.array(ptr).reshape(height, width, 3)  # Convert QImage to NumPy array

        return arr
    
    def save(self, filename = 'graffiti.png'):
        self.image.save(filename, 'png')
    
    @staticmethod
    def loadImage(filename):
        return QPixmap(filename)
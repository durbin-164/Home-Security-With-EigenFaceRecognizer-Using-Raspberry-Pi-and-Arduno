
import urllib.request
import cv2
import time
import numpy as np

UUurl='http://192.168.1.100:8080/shot.jpg'

class OpenCVCapture(object):
    def __init__(self,root_url=UUurl):
        #self.url = 'http://'+root_url+'/shot.jpg'
        self.url = UUurl;

    def read(self):
        # Get our image from the phone
        imgResp = urllib.request.urlopen(self.url)

        # Convert our image to a numpy array so that we can work with it
        imgNp = np.array(bytearray(imgResp.read()),dtype=np.uint8)

        # Convert our image again but this time to opencv format
        img = cv2.imdecode(imgNp,-1)

        return img


import cv2
import config
import face
import numpy as np

import smbus
import time
import os
import io

import busconfig as bus

print ('Loading training data...')
model = cv2.face.EigenFaceRecognizer_create()
print("Cascasde complete....")
model.read(config.TRAINING_FILE)
print ('Training data loaded!')
	
camera = config.get_camera()
cheackTime=10

while True:

    number = bus.readNumber() #Read from Arduno

    if number:
        print("Found number : ",number);
    
        detect=0
        undetect=0
        cont=0
        while True:
            rgbimage = camera.read()
            #ret,image = cam.read()
            cv2.imshow("Frame",rgbimage)
            cv2.waitKey(1)
            image=np.array(rgbimage,'uint8')
            # Convert image to grayscale.
            image = cv2.cvtColor(rgbimage,cv2.COLOR_BGR2GRAY)
            # Get coordinates of single face in captured image.
            result = face.detect_single(image)
            if result is None:
                continue
                                                
            x, y, w, h = result
            # Crop and resize image to face.
            crop = face.resize(face.crop(image, x, y, w, h))
            # Test face against model.
            label, confidence = model.predict(crop)
            
            print ('Predicted {0} face with confidence {1} (lower is more confident).'.format(
                   'POSITIVE' if label == config.POSITIVE_LABEL else 'NEGATIVE', confidence))



            if label == config.POSITIVE_LABEL and confidence < config.POSITIVE_THRESHOLD:
                print ('Recognized face!')
                detect+=1;
                                                
            else:
                print ('Did not recognize face!')
                undetect+=1;

            cv2.rectangle(rgbimage,(x,y),(x+w,y+h),(255,0,0),2)
            cv2.imshow("Frame",rgbimage)
            cv2.waitKey(1)
            cont+=1;#for first second loob break ater five time
            if(cont==cheackTime):
                break
            
            if(cv2.waitKey(1)==ord('q')):
                break;
        print("Detect = ",detect,"  Undetect = ",undetect);

        if(detect>=undetect):
            print("*******Main face is Detect Correctly***********")
            bus.datasend(True);# send command in arduno
        else:
            print("********Main face not Detected*************");
            bus.datasend(False);

        cv2.destroyAllWindows()

    else:
        print("Press B in Arduno Keyboard")







            

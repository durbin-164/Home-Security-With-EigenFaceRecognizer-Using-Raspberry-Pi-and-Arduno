import glob
import os
import sys
import select
import numpy as np

import cv2
import config
import face

# Prefix for positive training image filenames.
POSITIVE_FILE_PREFIX = 'positive_'


camera = config.get_camera()
	
# Create the directory for positive training images if it doesn't exist.
if not os.path.exists(config.POSITIVE_DIR):
    os.makedirs(config.POSITIVE_DIR)
# Find the largest ID of existing positive images.
# Start new images after this ID value.
files = sorted(glob.glob(os.path.join(config.POSITIVE_DIR, 
	POSITIVE_FILE_PREFIX + '[0-9][0-9][0-9].pgm')))
count = 0
if len(files) > 0:
    # Grab the count from the last filename.
    count = int(files[-1][-7:-4])+1


while True:
    print ('Capturing image...')
    rgbimage = camera.read()
    #ret,image = cam.read();
    cv2.imshow("Frame",rgbimage)
    cv2.waitKey(1)
    #image=np.array(image,'uint8')
    # Convert image to grayscale.
    image = cv2.cvtColor(rgbimage,cv2.COLOR_BGR2GRAY)
    
    # Get coordinates of single face in captured image.
    result = face.detect_single(image)
    if result is None:
        continue
				  
    x, y, w, h = result
    # Crop image as close as possible to desired face aspect ratio.
    # Might be smaller if face is near edge of image.
    crop = face.crop(image, x, y, w, h)
    # Save image to file.
    filename = os.path.join(config.POSITIVE_DIR, POSITIVE_FILE_PREFIX + '%03d.pgm' % count)
    cv2.imwrite(filename, crop)
    count += 1
    cv2.rectangle(rgbimage,(x,y),(x+w,y+h),(255,255,0),2)
    cv2.imshow("Frame",rgbimage)
    cv2.waitKey(1)
    if(cv2.waitKey(1)==ord('q')):
        break;
    
cv2.destroyAllWindows()


















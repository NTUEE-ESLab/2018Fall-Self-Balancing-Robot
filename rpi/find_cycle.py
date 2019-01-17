import cv2
import sys
import imutils
import time
from imutils.video import VideoStream
def imgproc(frame):
    
    # convert color to gray scale and show it
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    #cv2.imshow('gray', gray)
    
    blur = cv2.blur(gray, (5,5))
    edge = cv2.Canny(blur, 30, 100)
    edge = cv2.blur(edge, (2,2))
    #cv2.imshow('blured edge', edge)
    
    
    # convert image to black and white and show it
    thresh1, thresh = cv2.threshold(edge, 60, 255, cv2.THRESH_BINARY)
    #cv2.imshow('thresh', thresh)
    
    # find contours!
    _,contours, hry = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    # draw all the contours
    cpframe = frame.copy()
    cv2.drawContours(cpframe, contours, -1, (0,255,0), 3)
    cv2.imshow('cpframe', cpframe)
    
    # ================== TODO ===================
    
    # Modify these code to suit your need
    contours = [ctr for ctr in contours if cv2.contourArea(ctr) > 100]
    contours = [cv2.approxPolyDP(ctr, 5 , True) for ctr in contours]
    contours = [ctr for ctr in contours if cv2.isContourConvex(ctr)]
    #contours = [ctr for ctr in contours if len(ctr) == 3]
    #contours = [ctr for ctr in contours if len(ctr) == 4]
    contours = [ctr for ctr in contours if is_circle(ctr)]
    print("find %d circle"%(len(contours)))
    # ============================================
    
    
    # draw on the frame
    cv2.drawContours(frame, contours, -1, (255,0,0), -1)
    
    return frame

def is_circle(ctr):
    if len(ctr) < 5:
        return False
    max_x = ctr[0][0][0]
    max_y = ctr[0][0][1]
    min_x = ctr[0][0][0]
    min_y = ctr[0][0][1]
    for pt in ctr:
        if max_x < pt[0][0]:
            max_x = pt[0][0]
        if max_y < pt[0][1]:
            max_y = pt[0][1]
        if min_x > pt[0][0]:
            min_x = pt[0][0]
        if min_y > pt[0][1]:
            min_y = pt[0][1]
    ratio = 1.0 * (max_x - min_x) / (max_y - min_y)
    if ratio > 1.1 or 1 / ratio > 1.1:
        return False
    x = (max_x + min_x) / 2
    y = (max_y + min_y) / 2
    r = ((max_x - min_x) + (max_y - min_y)) / 4
    for pt in ctr:
        ratio = (1.0 * ((pt[0][0] - x) ** 2 + (pt[0][1] - y) ** 2) / r ** 2) ** 0.5
        if ratio > 1.1 or 1 / ratio > 1.1:
            return False
    return True

if __name__ == "__main__":
    if(sys.argv[1]=='0'):
        #vs = VideoStream(usePiCamera=True).start()
        vs = VideoStream().start()
        time.sleep(1.0)


        # loop over frames from the video stream
        while True:
            # grab the frame from the threaded video file stream, resize
            # it, and convert it to grayscale
            # channels)
            frame = vs.read()
            frame = imutils.resize(frame, width=480)
            #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            img = imgproc(frame)
            cv2.imshow('Press any key to exit', img)
            cv2.waitKey(0)
            # wait for the key
            # time.sleep(1)
    else:
        img = cv2.imread(sys.argv[1])
        img = imgproc(img)
        # cv2.imshow('Press any key to exit', img)
        # cv2.waitKey(0)
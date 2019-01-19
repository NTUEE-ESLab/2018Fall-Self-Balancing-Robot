import cv2
import numpy as np
import sys
import time
import imutils
from imutils.video import VideoStream
from webcam_gui import webcam_gui
def angle(p1, p2, p3):
    v1 = p1 - p2
    v2 = p3 - p2
    v1 = v1.astype(float)
    v2 = v2.astype(float)
    v1 = v1 / np.sqrt(np.dot(v1, v1))
    v2 = v2 / np.sqrt(np.dot(v2, v2))
    return np.degrees(np.arccos(np.dot(v1, v2)))
    

def isArrow(heptagon):
    hull = cv2.convexHull(heptagon, returnPoints = False)
    if len(hull) > 2:
        defects = cv2.convexityDefects(heptagon, hull)
        if defects is None or len(defects) != 2: 
            return False
      
        farpoints = [d[0][2] for d in defects]    
        if not np.abs(farpoints[0] - farpoints[1]) in [3, 4]:
            return False

        for defect in defects:
            s, e, f, d = defect[0]
            #    print defects
            #    s, e, f, d = defect[0]
            ps = heptagon[s, 0]
            pe = heptagon[e, 0]
            pd = heptagon[f, 0]
            if angle(ps, pd, pe) < 120:
                return True    

        return False
def direction(arrow):
    x_num = []
    for a in arrow:
        x_num.append(a[0][0])
    x_num = np.sort(np.array(x_num))
    right = x_num[1] - x_num[0]
    left = x_num[6] - x_num[5]
    if(right > left):
        return False
    else:
        return True

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
        
# def distance(p1,p2):
#     return np.linalg.norm(p1-p2)
# def tip(arrow):
#     hull = cv2.convexHull(arrow, returnPoints = False)
#     defects = cv2.convexityDefects(arrow, hull)
#     farpoints = [d[0][2] for d in defects]
#     if np.abs(farpoints[0] - farpoints[1]) == 4:
#         return arrow[sum(farpoints) / 2, 0]
#     else:
#         return arrow[0, 0]


def imgproc(frame):

    # convert color to gray scale and show it
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    cv2.imshow('gray', gray)
    cv2.waitKey(0)
    
    blur = cv2.GaussianBlur(gray, (5,5),0)
    edge = cv2.Canny(blur, 30, 200)
    edge = cv2.GaussianBlur(edge, (5,5),0)
    cv2.imshow('blured edge', edge)
    cv2.waitKey(0)

    # convert image to black and white and show it
    thresh1, thresh = cv2.threshold(edge, 50, 255, cv2.THRESH_BINARY)
    cv2.imshow('thresh', thresh)
    cv2.waitKey(0)
    # find contours!
    _,contours, hry = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    # draw all the contours
    cpframe = frame.copy()
    cv2.drawContours(cpframe, contours, -1, (0,0,255), 3)
    cv2.imshow('cpframe', cpframe)
    cv2.waitKey(0)
    
    # ================== TODO ===================
    
    # Modify these code to suit your need
    contours = [ctr for ctr in contours if cv2.contourArea(ctr) > 10]
    contours = [cv2.approxPolyDP(ctr, epsilon = 5, closed = True) for ctr in contours]
    heptagons = [ctr for ctr in contours if len(ctr) ==7]
    arrows = [hepta for hepta in heptagons if isArrow(hepta)]
    # if arrows == [] :
    #     print("no arrows")
    # else:
    #     print("yes ")
    #tips = [ tip(a) for a in arrows ]
    # print(len(heptagons))
    print("%f arrows"%(len(arrows)))
    #contours = [ctr for ctr in contours if cv2.isContourConvex(ctr)]
    
    # ============================================
    for arrow in (arrows):
        print(direction(arrow))
    # print(direction(arrows[4]))
    #print(arrows[4])
    # draw on the frame
    #cv2.drawContours(frame, heptagons, -1, (0,255,0), 3)
    cv2.drawContours(frame,arrows, -1, (255, 0, 0), -1)
    # draw tips
    # for t in tips:
    #    cv2.circle(frame, tuple(t), 5, (0, 0, 255), -1)

    return frame

if __name__ == "__main__":
    #webcam_gui(imgproc,sys.argv[1])
    #img = cv2.imread(sys.argv[1])
    #vs = VideoStream(src=0).start()
    if(sys.argv[1]=='0'):
        #vs = VideoStream(usePiCamera=True).start()
        time.sleep(1.0)
        vs = VideoStream().start()

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
            time.sleep(1)
    else:
        img = cv2.imread(sys.argv[1])
        img = imgproc(img)
        cv2.imshow('Press any key to exit', img)
        cv2.waitKey(0)
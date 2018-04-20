import cv2
import time
import numpy as np
import json
import requests
import sys
global refPt
global refPtOld
global refPtAll
global IP
global lVolt
global rVolt
global kP
global kI
refAngleOld = 0
refPt = [(0,0)]
refPtOld = [(0,0)]
refPtAll = []


WINDOW_NAME = 'GreenBallTracker' 

def main():
    # print command line arguments
    n = 0
    for arg in sys.argv[1:]:
        if n == 0:
            IP = arg
	elif n == 1:
	    lVolt = int(arg)
	elif n == 2:
	    rVolt = int(arg)
	elif n == 3:
	    kP = float(arg)
	elif n == 4:
	    kI = float(arg)
        n = n+1

def sendAddressData(angle,IP,lfv,rfv,kp,ki,kd):
    url = 'http://' + str(IP)+ '/angleFromAddress?angle=' + str(angle) + '&lfv=' + str(lfv) + '&rfv=' + str(rfv) + '&kp=' + str(kp) + '&ki=' + str(ki) + '&kd=' + str(kd)
    response = requests.get(url)

def sendAddressStart(IP):
    url = 'http://' + str(IP)+ '/startYourEngines'
    response = requests.get(url)

def sendAddressStop(IP):
    url = 'http://' + str(IP)+ '/stopYourEngines'           
    response = requests.get(url)


def click(event, x, y, flags, param):
	global refPt
	# if the left mouse button was clicked, record the starting
	# (x, y) coordinates
	if event == cv2.EVENT_LBUTTONDOWN:
		refPt =[(x, y)]


def track(image,lower_color,upper_color):

    '''Accepts BGR image as Numpy array
       Returns: (x,y) coordinates of centroid if found
                (-1,-1) if no centroid was found
                None if user hit ESC
    '''

    # Blur the image to reduce noise
    blur = cv2.GaussianBlur(image, (5,5),0)

    # Convert BGR to HSV
    hsv = cv2.cvtColor(blur, cv2.COLOR_BGR2HSV)

    # Threshold the HSV image for only green colors
    # lower_green = lower_color # np.array([40,70,70])
    # upper_green = upper_color # np.array([80,200,200])

    # Threshold the HSV image to get only green colors
    mask = cv2.inRange(hsv, lower_color, upper_color)
    
    # Blur the mask
    bmask = cv2.GaussianBlur(mask, (5,5),0)

    # Take the moments to get the centroid
    moments = cv2.moments(bmask)
    m00 = moments['m00']
    centroid_x, centroid_y = None, None
    if m00 != 0:
        centroid_x = int(moments['m10']/m00)
        centroid_y = int(moments['m01']/m00)

    # Assume no centroid
    ctr = (-1,-1)

    # Use centroid if it exists
    if centroid_x != None and centroid_y != None:

        ctr = (centroid_x, centroid_y)

        # Put Red circle in at centroid in image
        # cv2.circle(image, ctr, 8, (0,100,100), 5)

    # Display full-color image
    # cv2.imshow(WINDOW_NAME, bmask)
    # cv2.imshow(WINDOW_NAME, image)

    # Force image display, setting centroid to None on ESC key input
    if cv2.waitKey(1) & 0xFF == 27:
        ctr = None
    
    # Return coordinates of centroid
    return ctr

# Test with input from camera
if __name__ == '__main__':
    IP = sys.argv[1]
    lVoltdef = sys.argv[2]
    rVoltdef = sys.argv[3]
    kPt   = float(sys.argv[4])
    kIt   = float(sys.argv[5])
    kDt   = float(sys.argv[6])
    green_lower = np.array([40,70,70])
    green_upper = np.array([80,200,200])
    red_upper   = np.array([250,200,200])
    red_lower   = np.array([160,50,50])
    #blue_upper  = np.array([120,200,200])
    #blue_lower  = np.array([100,70,70])
    blue_upper  = np.array([120,150,150])
    blue_lower  = np.array([100,100,100])
    capture = cv2.VideoCapture(0)
    capture.set(3,500)
    capture.set(4,500)

    print(IP)
    print(lVoltdef)
    print(rVoltdef)
    print(kPt)
    print(kIt)
    print(kDt)

    img = np.zeros((512,512,3), np.uint8)
    cv2.namedWindow('image')
    cv2.setMouseCallback("image", click)
    okay, image = capture.read()

    cv2.imshow("image",image)
    while True:
	# display the image and wait for a keypress
	# cv2.imshow("image", image)
	key = cv2.waitKey(1) & 0xFF
        
	if refPt != refPtOld:
	    refPtOld = refPt
	    refPtAll.append(refPt[0])
	    print(refPtAll) 
	    cv2.circle(image,refPt[0],4, (255,255,255),10)
	    if len(refPtAll) > 1:
		locLine = len(refPtAll)
		cv2.line(image,refPtAll[locLine-2],refPtAll[locLine-1],(512,512,3),5)
	    cv2.imshow("image",image)
	# if the 'r' key is pressed, reset the cropping region
	if key == ord("r"):
		image = clone.copy()
 
	# if the 'c' key is pressed, break from the loop
	elif key == ord("c"):
		break

    t = time.time()
    ## Track bot
    sendAddressStart(IP)
    while len(refPtAll) > 0:
	okay, image = capture.read()
        if okay:
	    key = cv2.waitKey(1) & 0xFF
	    if key == ord("c"):
		sendAddressStop(IP)
                break
            #locR = track(image,red_lower,red_upper)
	    locB = track(image,blue_lower,blue_upper)
	    locG = track(image,green_lower,green_upper)
            diff = ( locB[0] - locG[0], locB[1] - locG[1])
	    frontBot = locB
	    center   = ((locB[0]+locG[0])/2,(locB[1]+locG[1])/2)
	    closestPt= refPtAll[0]
	    diff2= (center[0]-closestPt[0], center[1]-closestPt[1])
	    for n in range(0,len(refPtAll)):
		cv2.circle(image,refPtAll[n],4, (255,255,255),10)
		if n > 0:
		    cv2.line(image,refPtAll[n-1],refPtAll[n],(512,512,3),5)
	    if ((center[0]-closestPt[0])**2+(center[1]-closestPt[1])**2)<2500:
		refPtAll.pop(0)
		if len(refPtAll) == 0:
		   sendAddressStop(IP)
		   break
	    cv2.circle(image,center,4,(0,0,0),10)
            botAngle = np.angle(diff[0]+diff[1]*1j, deg=True)
	    nearAngle= np.angle(diff2[0]+diff2[1]*1j, deg=True)
	    cv2.line(image,locB,locG,(255,255,255),4)
	    cv2.line(image,center,refPtAll[0],(260,50,50),4)
	    cv2.imshow('image',image)
	    refAngle = botAngle-nearAngle
	    if refAngle < -180:
		refAngle = refAngle + 360
	    if refAngle > 180:
		refAngle = refAngle - 360
	    if abs(refAngle) > 100:
		refAngle = 100*np.sign(refAngle)
	    #if abs(refAngle) > 80:
		#kP = kPh
	    #elif abs(refAngle) > 60:
		#kP = (int(kPh)+int(kPl))*2/6
	    #elif abs(refAngle) > 40:
		#kP = (int(kPh)+int(kPl))/6
	    #else:
		#kP = kPl 
	    if abs(refAngle) > 80:
	    	lVolt = 300
	    	rVolt = 280
	        kP = 3
		kI = 0
	        kD = 0
	    elif abs(refAngle) > 25:
		lVolt = 600
		rVolt = 470
		kP = kPt
		kI = kIt
		kD = kDt
	    else:
	        lVolt = 1023
	        rVolt = 795
		kP    = 10
		kI    = 0
		kD    = 0
	    sendAddressData(refAngle,IP,lVolt,rVolt,kP,kI,kD)#refAngle,IP)
	    print(-refAngle)
	    refAngleOld = refAngle
	   
        else:

           print('Capture failed')
           break

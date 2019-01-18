import serial
import sys
import time
import RPi.GPIO as GPIO
import cv2
import numpy as np
import imutils
from imutils.video import VideoStream

trigger_pin1 = 23
echo_pin1 = 24
CONTROL_PIN = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(echo_pin1, GPIO.IN)
GPIO.setup(trigger_pin1, GPIO.OUT)
GPIO.setup(CONTROL_PIN, GPIO.OUT)
GPIO.setwarnings(False)
PWM_FREQ = 50
pwm = GPIO.PWM(CONTROL_PIN, PWM_FREQ)
pwm.start(0)

def send_trigger_pulse(pin):
	GPIO.output(pin, True)
	time.sleep(0.001)
	GPIO.output(pin, False)

def wait_for_echo(value, timeout, pin):
	count = timeout
	while GPIO.input(pin) != value and count > 0:
		count = count - 1

def get_distance(trigger, echo):
	send_trigger_pulse(trigger)
	wait_for_echo(True, 10000, echo)
	start = time.time()
	wait_for_echo(False, 10000, echo)
	finish = time.time()
	pulse_len = finish - start
	distance_cm = pulse_len * 340 *100 /2
	return (distance_cm)
def distance(trigger, echo):
	print("count distance")
	distance = np.zeros((10,))
	count = 0
	while(count < 10):
		d = get_distance(trigger, echo)
		if( d <400 and d >= 1):
			distance[count] = d
			count += 1
	d = np.sort(distance)[1:9]
	return np.mean(d)
def angle_to_duty_cycle(angle=0):
    duty_cycle = (0.05 * PWM_FREQ) + (0.19 * PWM_FREQ * angle / 180)
    return duty_cycle
def angle(p1, p2, p3):
	v1 = p1 - p2
	v2 = p3 - p2
	v1 = v1.astype(float)
	v2 = v2.astype(float)
	v1 = v1 / np.sqrt(np.dot(v1, v1))
	v2 = v2 / np.sqrt(np.dot(v2, v2))
	return np.degrees(np.arccos(np.dot(v1, v2)))
def turn_right():
	angle = 100 #right pwm1 bottom
	dc = angle_to_duty_cycle(angle)
	pwm.ChangeDutyCycle(dc)
	time.sleep(0.5)
	return  get_distance(trigger_pin1,echo_pin1)

def turn_left():
	angle = 180 #left
	dc = angle_to_duty_cycle(angle)
	pwm.ChangeDutyCycle(dc)
	time.sleep(0.5)
	return  get_distance(trigger_pin1,echo_pin1)

def turn_middle():
	angle = 150 #middle
	dc = angle_to_duty_cycle(angle)
	pwm.ChangeDutyCycle(dc)
	time.sleep(0.5)
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
def imgproc(frame):
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	blur = cv2.GaussianBlur(gray, (5,5),0)
	edge = cv2.Canny(blur, 30, 200)
	edge = cv2.GaussianBlur(edge, (5,5),0)
	thresh1, thresh = cv2.threshold(edge, 50, 255, cv2.THRESH_BINARY)
	_,contours, hry = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
	cpframe = frame.copy()
	contours = [ctr for ctr in contours if cv2.contourArea(ctr) > 40]
	contours = [cv2.approxPolyDP(ctr, epsilon = 5, closed = True) for ctr in contours]
	heptagons = [ctr for ctr in contours if len(ctr) ==7]
	#circle = [ctr for ctr in contours if cv2.isContourConvex(ctr)]
	#circles = [ctr for ctr in circle if is_circle(ctr)]
	arrows = [hepta for hepta in heptagons if isArrow(hepta)]
	if(len(arrows) == 1):
		if(direction(arrows[0])== True):
			return 1
		else:
			return 2
	else:
		return 0

port = "/dev/rfcomm0"
serial = serial.Serial(port,9600,timeout=1)
vs = VideoStream(usePiCamera=True).start()
time.sleep(1.0)
count = 0 
num_1 = 0
num_2 = 0
num_0 = 0
real_arr = 0
try:
	while 1:
		d1 = distance(trigger_pin1,echo_pin1)
		print("distance = %d cm" % d1)
		frame = vs.read()
		if(d1 < 50):
			serial.write("$0,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.1)
			#back first
			serial.write("$2,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.25)
			serial.write("$0,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.1)
			while(count < 30):
				serial.write("$0,0,0,0,0,0,0,0,0,0#")
				time.sleep(0.1)
				frame = imutils.resize(frame, width=480)
				arr = imgproc(frame)
				count +=1
				print("now arrow %d toatl arrows %d"%(arr,real_arr))
				if  (arr == 1):
					num_1 +=1
				elif(arr == 2) :
					num_2 +=1
				else :
					num_0 +=1
				if(count ==30):
					if num_0 > 25 :
						real_arr = 0
					elif num_1 > num_2:
						real_arr = 1
					else:
						real_arr = 2
					num_0 = 0 
					num_1 = 0
					num_2 = 0
			count = 0
			# to decide which way to go
			if(real_arr == 0):
				right = turn_right()
				left = turn_left()
				turn_middle()
				if right > left:
					# turn right
					print("right = %d cm > left = %d cm" % (right,left))
					serial.write("$4,0,0,0,0,0,0,0,0,0#")
					time.sleep(0.85)
					
				else:
					#turn left
					print("left = %d cm > right = %d cm" % (left,right))
					serial.write("$3,0,0,0,0,0,0,0,0,0#")
					time.sleep(0.85)
				
			else:
				if(real_arr == 1):
					real_arr = 0
					# turn right
					serial.write("$4,0,0,0,0,0,0,0,0,0#")
					time.sleep(0.85)
					
				else:
					#turn left
					real_arr = 0
					serial.write("$3,0,0,0,0,0,0,0,0,0#")
					time.sleep(0.85)

		serial.write("$0,0,0,0,0,0,0,0,0,0#")
		time.sleep(0.1)
		#go straight
		serial.write("$1,0,0,0,0,0,0,0,0,0#")

except KeyboardInterrupt:
	pwm.stop()
	GPIO.cleanup()       # clean up GPIO on CTRL+C exit
	serial.close()

GPIO.cleanup()           # clean up GPIO on normal exit
serial.close()

	# if a == "u":
	# 	serial.write("$1,0,0,0,0,0,0,0,0,0#")
	# elif a == "d":
	# 	serial.write("$2,0,0,0,0,0,0,0,0,0#")
	# elif a == "l":
	# 	serial.write("$3,0,0,0,0,0,0,0,0,0#")
	# elif a == "r":
	# 	serial.write("$4,0,0,0,0,0,0,0,0,0#")
	# elif a == "ls":
	# 	serial.write("$0,1,0,0,0,0,0,0,0,0#")
	# elif a == "rs":
	# 	serial.write("$0,2,0,0,0,0,0,0,0,0#")
	#time.sleep(0.5)
	# serial.write("$0,0,0,0,0,0,0,0,0,0#")
	# time.sleep(1)
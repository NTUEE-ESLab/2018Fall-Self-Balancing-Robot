import serial
import sys
import time
import RPi.GPIO as GPIO

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

def angle_to_duty_cycle(angle=0):
    duty_cycle = (0.05 * PWM_FREQ) + (0.19 * PWM_FREQ * angle / 180)
    return duty_cycle

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

port = "/dev/rfcomm0"
serial = serial.Serial(port,9600,timeout=1)

try:
	while 1:
		d1 = get_distance(trigger_pin1,echo_pin1)
		print("distance = %d cm" % d1)
		if(d1 < 30):
			serial.write("$0,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.1)
			#back first
			serial.write("$2,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.3)
			serial.write("$0,0,0,0,0,0,0,0,0,0#")
			time.sleep(0.1)
			# to decide which way to go
			right = turn_right()
			left = turn_left()
			turn_middle()
			if right > left:
				# turn right
				print("right = %d cm > left = %d cm" % (right,left))
				serial.write("$4,0,0,0,0,0,0,0,0,0#")
				time.sleep(0.5)
				
			else:
				#turn left
				print("left = %d cm > right = %d cm" % (left,right))
				serial.write("$3,0,0,0,0,0,0,0,0,0#")
				time.sleep(0.5)
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
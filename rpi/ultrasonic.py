import RPi.GPIO as GPIO
import time
trigger_pin1 = 23
echo_pin1 = 24
# trigger_pin2 = 27
# echo_pin2 = 22

GPIO.setmode(GPIO.BCM)
GPIO.setup(trigger_pin1, GPIO.OUT)
GPIO.setup(echo_pin1, GPIO.IN)
# GPIO.setup(trigger_pin2, GPIO.OUT)
# GPIO.setup(echo_pin2, GPIO.IN)

def send_trigger_pulse(pin):
	GPIO.output(pin, True)
	time.sleep(0.001)
	GPIO.output(pin, False)

def wait_for_echo(value, timeout,pin):
	count = timeout
	while GPIO.input(pin) != value and count > 0:
		count = count - 1

def get_distance(trigger, echo):
	send_trigger_pulse(trigger)
	wait_for_echo(True, 5000,echo)
	start = time.time()
	wait_for_echo(False, 5000,echo)
	finish = time.time()
	pulse_len = finish - start
	distance_cm = pulse_len * 340 *100 /2
	return (distance_cm)

while True:
	print("1 cm=%f" % get_distance(trigger_pin1,echo_pin1))
	#print("2 cm=%f" % get_distance(trigger_pin1,echo_pin1))
	# time.sleep(1)
GPIO.cleanup()
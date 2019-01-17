import RPi.GPIO as GPIO
import time
 
CONTROL_PIN = 17
PWM_FREQ = 50
STEP=30
# CONTROL_PIN2 = 27
# trigger_pin1 = 23
# echo_pin1 = 24
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(CONTROL_PIN, GPIO.OUT)
# GPIO.setup(CONTROL_PIN2, GPIO.OUT)
# GPIO.setup(trigger_pin1, GPIO.OUT)
# GPIO.setup(echo_pin1, GPIO.IN)
 
pwm = GPIO.PWM(CONTROL_PIN, PWM_FREQ)
# pwm2 = GPIO.PWM(CONTROL_PIN2, PWM_FREQ)
pwm.start(0)
# pwm2.start(0)
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
def angle_to_duty_cycle(angle=0):
    duty_cycle = (0.05 * PWM_FREQ) + (0.19 * PWM_FREQ * angle / 180)
    return duty_cycle
 

print('press Ctrl-C to shut down')
angle = 100 #right pwm1 bottom
dc = angle_to_duty_cycle(angle)
pwm.ChangeDutyCycle(dc)
time.sleep(1)
angle = 180 #left
dc = angle_to_duty_cycle(angle)
pwm.ChangeDutyCycle(dc)
time.sleep(1)
angle = 150 #middle
dc = angle_to_duty_cycle(angle)
pwm.ChangeDutyCycle(dc)
time.sleep(1)
# angle = 85
# dc = angle_to_duty_cycle(angle)
# pwm2.ChangeDutyCycle(dc)
# time.sleep(2)
# angle = 180 #down
# dc = angle_to_duty_cycle(angle)
# pwm2.ChangeDutyCycle(dc)
# time.sleep(2)
# angle = 100 # middle
# dc = angle_to_duty_cycle(angle)
# pwm2.ChangeDutyCycle(dc)
# pwm.stop()
    # pwm2.ChangeDutyCycle(dc)
    # pwm2.ChangeDutyCycle(dc)
    # pwm2.ChangeDutyCycle(dc)
# for angle in range(0, 181, STEP):
#     dc = angle_to_duty_cycle(angle)
#     pwm.ChangeDutyCycle(dc)
#     # pwm2.ChangeDutyCycle(dc)
#     #print("1 cm=%f" % get_distance(trigger_pin1,echo_pin1))
#     print('angle={: >3}, period={:.2f}'.format(angle, dc))
#     time.sleep(2)
# for angle in range(180, -1, -STEP):
#     dc = angle_to_duty_cycle(angle)
#     print('angle={: >3}, period={:.2f}'.format(angle, dc))
#     pwm.ChangeDutyCycle(dc)
#     #print("1 cm=%f" % get_distance(trigger_pin1,echo_pin1))
#     # pwm2.ChangeDutyCycle(dc)
#     time.sleep(2)
# pwm.ChangeDutyCycle(angle_to_duty_cycle(90))
# pwm2.ChangeDutyCycle(angle_to_duty_cycle(90))
GPIO.cleanup()
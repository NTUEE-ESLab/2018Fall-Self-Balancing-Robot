import cv2
import imutils
from imutils.video import VideoStream
def webcam_gui(filter_func, video_src="0"):
	key_code = -1
	if(video_src == "0"):
		vs = VideoStream(src=0).start()
		while(key_code == -1):
			# read a frame
			frame = vs.read()
			frame = imutils.resize(frame, width=480)
			# run filter with the arguments
			frame_out = filter_func(frame)
			# show the image
			cv2.imshow('Press any key to exit', frame_out)
			# wait for the key
			key_code = cv2.waitKey(10)
		cap.release()
		cv2.destroyAllWindows()
	else:
		cap = cv2.imread(video_src)
		while(key_code == -1):
			# run filter with the arguments
			frame_out = filter_func(cap)
			# show the image
			#cv2.imshow('Press any key to exit', frame_out)
			# wait for the key
			key_code = cv2.waitKey(10)
		cv2.destroyAllWindows()

def edge_filter(frame_in):
	# convert into gray scale
	frame_gray = cv2.cvtColor(frame_in, cv2.COLOR_BGR2GRAY)
	# blur the image to reduce noise
	frame_blur = cv2.blur(frame_gray, (3,3))
	# Canny edge detection
	frame_out = cv2.Canny(frame_blur, 30, 120)
	
	return frame_out

	
	
def gray_filter(frame_in):
	# convert into gray scale
	frame_out = cv2.cvtColor(frame_in, cv2.COLOR_BGR2GRAY)
	
	return frame_out
	
	
	
if __name__ == "__main__":
	# cv2.VideoCapture.set(CV_CAP_PROP_FPS, 10) 
	webcam_gui(edge_filter,"m.jpg")
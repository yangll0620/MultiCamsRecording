import cv2

path = "video_20211227_120714_camera0.avi"
cap = cv2.VideoCapture(path)
framen = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
print(path + ": framen = " + str(framen))
cap.release()
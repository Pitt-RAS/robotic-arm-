import cv2


def display_img(img):
    cv2.imshow("Frame", img)
    if cv2.waitKey(1) == ord('k'):
        exit(0)
    

cam = cv2.VideoCapture(0)

while True:
    ret, frame = cam.read()
    # cv2.line(img=frame, pt1=(10, 10), pt2=(100, 100), color=(255, 0, 0), thickness=20, lineType=8, shift=0)


    if ret != True:
        exit(0)

    frame[frame>150] = 255
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)
    thresh,frame = cv2.threshold(frame, 0, 255, cv2.THRESH_OTSU)

    print(thresh)
    display_img(frame)
    

cam.release()
cv2.destroyAllWindows()
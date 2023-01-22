from tqdm import tqdm
import numpy as np
import cv2
size = 500
k = np.random.random((size,size))

def display_img(img):
    cv2.imshow("Frame", img)
    cv2.waitKey(0)

k[k < .5] = 1

display_img(k)
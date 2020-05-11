# Map visualizer using OpenCV
# Usage:
# 	$ py visualizer.py <FILE_NAME>

import numpy as np
import sys
import cv2
from PIL import Image

color_map = {0: (0, 0, 0), 1: (255, 255, 255), 2: (0, 255, 0), 3: (0, 0, 255), 4: (255, 0, 0)}

f = open(sys.argv[1], "r")
grid = []

startX, startY = map(int, f.readline().split())
startX -= 1
startY -= 1
fuel = int(f.readline())
dimX, dimY = map(int, f.readline().split())
for _ in range(dimX):
	grid.append(f.readline().split())

grid[startX][startY] = '4'

env = np.zeros((dimX, dimY, 3), dtype=np.uint8)
for i in range(dimX):
	for j in range(dimY):
		env[i][j] = color_map[ord(grid[i][j]) - 48]
img = Image.fromarray(env, 'RGB')
img = img.resize((1000, 1000))
cv2.imshow("image", np.array(img)) 
cv2.waitKey()
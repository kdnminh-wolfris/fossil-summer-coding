import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import pickle
from matplotlib import style
import time
import copy

style.use("ggplot")
f = open("map_100_100.txt", "r")
init_grid = []
TANK_ID = 4
color_map = {0: (0, 0, 0), 1: (255, 255, 255), 2: (0, 255, 0), 3: (0, 0, 255), 4: (255, 0, 0)}
move_x = [-1, 0, 1, 0]
move_y = [0, -1, 0, 1]

startX, startY = map(int, f.readline().split())
startX -= 1
startY -= 1
fuel = int(f.readline())
dimX, dimY = map(int, f.readline().split())
for _ in range(dimX):
	init_grid.append(f.readline().split())

TOTAL_EPISODES = 20000
MOVE_PENALTY = -1
WALL_PENALTY = -1000
OUT_OF_FUEL_PENALTY = -1000
STAR_REWARD = 50
FUEL_REWARD = 50

epsilon = 1
START_EPSILON_DECAYING = 1
END_EPSILON_DECAYING = TOTAL_EPISODES//2
epsilon_decay_value = epsilon/(END_EPSILON_DECAYING - START_EPSILON_DECAYING)
EPS_DECAY = 0.99975

DISPLAY_EVERY = 1000
INF = dimX * dimY + 5
TOTAL_STARS = 0
FUEL_CAP = fuel
BUCKET = 5
tankX, tankY = startX, startY

for i in range(dimX):
	for j in range(dimY):
		TOTAL_STARS += init_grid[i][j] == '3'

LEARNING_RATE = 0.1
DISCOUNT = 0.95

LOAD_EXISTING_QTABLE = None #Load pre-trained Q-learning table, NONE or "filename"

if LOAD_EXISTING_QTABLE is None:
	q_table = {}
	for mask in range(1<<8):
		for x in range(-dimX//BUCKET, dimX//BUCKET+1):
			for y in range(-dimY//BUCKET, dimY//BUCKET+1):
				q_table[(mask, (x, y))] = [np.random.uniform(-5, 0) for i in range(4)]
else:
	with open(LOAD_EXISTING_QTABLE, "rb") as f:
		q_table = pickle.load(f)

print("Finish initializing...")

def move(action):
	global tankX, tankY, fuel
	stupid_move = False
	tankX += move_x[action]
	tankY += move_y[action]
	if (tankX >= 0 and tankX < dimX) and (tankY >= 0 and tankY < dimY): 
		fuel -= 1
	else:
		stupid_move = True
	tankX = max(min(tankX, dimX - 1), 0)
	tankY = max(min(tankY, dimY - 1), 0)
	return stupid_move

def relativeDist(x1, y1, x2, y2):
	return ((x1 - x2) // BUCKET, (y1 - y2) // BUCKET), abs(x1 - x2) + abs(y1 - y2)

def isWall(x, y):
	if x<0 or x>=dimX or y<0 or y>=dimY or grid[x][y] == '0':
		return 1
	else:
		return 0

def getState(grid):
	mask = 0
	dist_gas_or_star = INF
	relative_to_gas_or_star = (0, 0)
	for i in range(-1, dimX + 1):
		for j in range(-1, dimY + 1):
			relative_dist, d = relativeDist(i, j, tankX, tankY)
			if grid[tankX][tankY] == '2' or grid[tankX][tankY] == '3':
				if d < dist_gas_or_star:
					dist_gas_or_star = d
					relative_to_gas_or_star = relative_dist
	mask = mask | isWall(tankX-1, tankY-1)
	mask = mask | (isWall(tankX, tankY-1) << 1)
	mask = mask | (isWall(tankX+1, tankY-1) << 2)
	mask = mask | (isWall(tankX-1, tankY) << 3)
	mask = mask | (isWall(tankX+1, tankY) << 4)
	mask = mask | (isWall(tankX-1, tankY+1) << 5)
	mask = mask | (isWall(tankX, tankY+1) << 6)
	mask = mask | (isWall(tankX+1, tankY+1) << 7)
	return mask, relative_to_gas_or_star

episode_rewards = []
RANDOM_START = False

for episode in range(TOTAL_EPISODES):
	grid = copy.deepcopy(init_grid)
	fuel = FUEL_CAP
	stars = TOTAL_STARS
	display = False

	if RANDOM_START:
		while True:
			tankX = np.random.randint(0, dimX)
			tankY = np.random.randint(0, dimY)
			if grid[tankX][tankY] == '1':
				break
	else:
		tankX = startX
		tankY = startY

	if episode % DISPLAY_EVERY == 0:
		print(f"On {episode}, epsilon: {epsilon}")
		print(f"{DISPLAY_EVERY} epsilon mean: {np.mean(episode_rewards[-DISPLAY_EVERY:])}")
		display = True
	
	total_reward = 0
	print(episode)
	for _ in range(5000):
		observation = getState(grid)
		if np.random.random() > epsilon:
			action = np.argmax(q_table[observation])
		else:
			action = np.random.randint(0, 4)

		bad = move(action)
		
		if bad:
			reward = WALL_PENALTY
		elif grid[tankX][tankY] == '0': # Hit the wall!
			reward = WALL_PENALTY
		elif grid[tankX][tankY] == '2': # Gas refill
			reward = FUEL_REWARD
			fuel = FUEL_CAP
			grid[tankX][tankY] = '1'
		elif grid[tankX][tankY] == '3': # Collects a star
			reward = STAR_REWARD
			stars -= 1
			grid[tankX][tankY] = '1'
		elif TOTAL_STARS > 0 and fuel == 0: # Runs out of fuel while the tank has not collected all stars
			reward = OUT_OF_FUEL_PENALTY
		else:
			reward = MOVE_PENALTY

		new_observation = getState(grid)
		max_future_q = np.max(q_table[new_observation])
		current_q = q_table[observation][action]

		if reward > 0:
			new_q = reward
		else:
			new_q = (1 - LEARNING_RATE) * current_q + LEARNING_RATE * (reward + DISCOUNT * max_future_q) # MAGIC FORMULA 

		q_table[observation][action] = new_q

		if display:
			env = np.zeros((dimX, dimY, 3), dtype=np.uint8)
			for i in range(dimX):
				for j in range(dimY):
					if i == tankX and j == tankY:
						env[i][j] = color_map[4]
					else:
						env[i][j] = color_map[ord(grid[i][j]) - 48]
			img = Image.fromarray(env, 'RGB')
			img = img.resize((800, 800))
			cv2.imshow("image", np.array(img)) 
			if stars == 0 or reward == WALL_PENALTY or fuel == 0:
				if cv2.waitKey(1000) & 0xFF == ord('q'):
					break
			else:
				if cv2.waitKey(50) & 0xFF == ord('q'):
					break
		
		total_reward += reward
		if stars == 0 or reward == WALL_PENALTY or fuel == 0:
			break
	
	episode_rewards.append(total_reward)
	# if END_EPSILON_DECAYING >= episode >= START_EPSILON_DECAYING:
	# 	epsilon -= epsilon_decay_value
	if epsilon > 0.1: 
		epsilon -= (1/TOTAL_EPISODES)

linear_avg = np.convolve(episode_rewards, np.ones((DISPLAY_EVERY,)) / DISPLAY_EVERY, mode='valid')

plt.plot([i for i in range(len(linear_avg))], linear_avg)
plt.ylabel(f"Reward {DISPLAY_EVERY}ma")
plt.xlabel("episode #")
plt.show()

with open(f"qtable-{int(time.time())}.pickle", "wb") as f:
    pickle.dump(q_table, f)






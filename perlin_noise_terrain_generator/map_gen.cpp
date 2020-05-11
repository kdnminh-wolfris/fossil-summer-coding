/*
	Map generator using Perlin noise
	
	Compile this source code (c++11): 
		$ g++ -std=c++11 -o map_gen map_gen.cpp PerlinNoise.cpp

	Usage: 
		Run this program by the following command-line format:
		$ map_gen <MAP_HEIGHT> <MAP_WIDTH> <MAP_NAME>

		For example:
		$ map_gen 100 100 map_square_100
*/

#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>
#include "PerlinNoise.h"

void generateMap(int height, int width, std::string fileName) {
	#define FREQUENCY 2.0       // Frequency of those black islands
	#define Z_NOISE 0.8         // Put whatever you want :)
	#define SMOOTHNESS 20.0     // The smaller smoothness, the bigger chunk of walls

	std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
	std::ofstream file(fileName);
	std::vector<std::vector<double>> elevation(height, std::vector<double>(width));
	std::vector<std::vector<int>> finalMap(height, std::vector<int>(width, 1));
	std::vector<double> elevation_list;
	PerlinNoise pn(rng() % 333);

	for (int x = 0; x < height; ++x) {
		for (int y = 0; y < width; ++y) {
			double nx = x / (double) height;
			double ny = y / (double) width;
			elevation[x][y] = FREQUENCY * pn.noise(nx * SMOOTHNESS, ny * SMOOTHNESS, Z_NOISE);
			elevation_list.push_back(elevation[x][y]);
		}
	}

	std::sort(elevation_list.begin(), elevation_list.end());

	double threshold = elevation_list[3 * (int)elevation_list.size() / 5];
	for (int x = 0; x < height; ++x) { // normalization
		for (int y = 0; y < width; ++y) {
			finalMap[x][y] = (elevation[x][y] >= threshold) ? 0 : 1;
		}
	}

	for (int i = 0; i < (height + width) / 2; ++i) {
		int blobX, blobY;
		while (true) {
			blobX = rng() % height;
			blobY = rng() % width;
			if (finalMap[blobX][blobY] == 1) {
				finalMap[blobX][blobY] = 2;
				break;
			}
		}
		while (true) {
			blobX = rng() % height;
			blobY = rng() % width;
			if (finalMap[blobX][blobY] == 1) {
				finalMap[blobX][blobY] = 3;
				break;
			}
		}
	}

	int startX, startY;
	while (true) {
		startX = rng() % height;
		startY = rng() % width;
		if (finalMap[startX][startY] == 1){
			break;
		}
	}

	// int fuelCap = rng() % (height + width);
	int fuelCap = height * width;

	file << startX << ' ' << startY << std::endl;
	file << fuelCap << std::endl;
	file << height << ' ' << width << std::endl;
	for (int x = 0; x < height; ++x) {
		for (int y = 0; y < width; ++y) {
			file << finalMap[x][y] << ' ';
		}
		file << std::endl;
	}

	file.close();
	#undef FREQUENCY
	#undef Z_NOISE
	#undef SMOOTHNESS
}

int main(int argc, char *argv[]) {
	try {
		if (argc != 4) {
			throw std::runtime_error("insufficient number of generator arguments!");
		}
		int mapHeight = strtol(argv[1], NULL, 10);
		int mapWidth = strtol(argv[2], NULL, 10);
		std::string mapName = argv[3];
		mapName += ".txt";
		generateMap(mapHeight, mapWidth, mapName);
	}
	catch (std::runtime_error& except){
		std::cerr << except.what() << std::endl;
	}
	return 0;
}
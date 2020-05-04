#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

class Map {
	int size_m, size_n;
	int max_gas_unit;
	int init_x, init_y;
	vector <vector <int>> data;

	public:
		void resize(int _m, int _n);
		void load();
		void display();
		int star_count();
};

int main() {
	freopen("map.txt", "r", stdin);
	freopen("path.txt", "w", stdout);

	Map a;
	a.load();
	cout << a.star_count();
}

void Map::resize(int _m, int _n) {
	size_m = _m; size_n = _n;
	data.clear();
	data.resize(_m, vector <int>(_n));
}

void Map::load() {
	cin >> init_x >> init_y;
	cin >> max_gas_unit;
	cin >> size_m >> size_n;
	resize(size_m, size_n);
	for (int i = 0; i < size_m; ++i)
		for (int j = 0; j < size_n; ++j)
			cin >> data[i][j];
}

void Map::display() {
	for (int i = 0; i < size_m; ++i)
		for (int j = 0; j < size_n; ++j)
			cout << data[i][j] << " \n"[j == size_n - 1];
}

int Map::star_count() {
	int ret = 0;
	for (int i = 0; i < size_m; ++i)
		for (int j = 0; j < size_n; ++j)
			if (data[i][j] == 3) ++ret;
	return ret;
}

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <tbb/tbb.h>

using namespace tbb;
using namespace std;

void addvec(const vector<int> &a, const vector<int> &b, vector<int> &c, int &k) {
	if (a.size() != b.size() || a.size() != c.size()) {
		cerr << "error: vector size mismatch\n";
		exit(0);
	}

	parallel_for(blocked_range<unsigned int>(0, a.size()),
		[&](const blocked_range<unsigned int> &range) {
		for (unsigned int i = range.begin(); i != range.end(); ++i)
		{
			c[i] = a[i] + b[i];
			k = k + 1;
			cout << c[i] << endl;
		}
	});

}

const int N = 5000;

int main() {
	int k = 0;

	auto x = vector<int>(N);
	auto y = vector<int>(N);
	auto z = vector<int>(N);

	for (int j = 0; j < N; j++) {
		x[j] = j;
		y[j] = j + 1;
	}

	addvec(x, y, z, k);

	for (unsigned int i = 0; i < z.size(); i++) {
		cout << i << " -> " << z[i] << endl;
	}

	cout << endl << "k=" << k << endl;
}
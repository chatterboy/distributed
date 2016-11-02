#include <bits/stdc++.h>
using namespace std;

void f1(int & n) {
	for (int i = 0; i < 5; ++i) {
		cout << "Thread 1 executing\n";
		++n;
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void f2(int & n) {
	for (int i = 0; i < 5; ++i) {
		cout << "Thread 2 executing\n";
		++n;
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

int main() {
	int n = 0;
	thread t1;
	thread t2(f1, ref(n));
	thread t3(f2, ref(n));
	thread t4(move(t3, ref(n)));
	t2.join();
	t4.join();
	cout << "Final value of n is " << n << endl;
}

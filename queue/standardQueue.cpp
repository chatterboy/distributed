#include <bits/stdc++.h>
using namespace std;

const int maxn = 1000000;

struct Queue { 
	int arr[maxn];
	int head;
	int tail;
	pthread_mutex_t lock;
	Queue() : head(-1), tail(-1) {}
} q;

vector<int> operations1;
vector<int> operations2;

int generateOperations(vector<int> & operations) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> uid(0, 1);
	int added = 0;
	puts("start generating the operations for queue.");
	for (int nenq = 0; nenq < maxn;) {
		if (added < 1) {
			++added;
			operations.push_back(1);
			++nenq;
		} else {
			int tmp = uid(gen);
			operations.push_back(tmp);
			added += tmp ? 1 : -1;
			nenq += tmp ? 1 : 0;
		}
	}
	for (int i = 0; i < added; ++i) {
		operations.push_back(0);
	}
	puts("end generating the operations for queue.");
	printf("# of operations are generated for queue is %d.\n", (int)operations.size());
	return 0;
}

int isempty() {
	int ret = q.head - q.tail;
	return ret < 1 ? 1 : 0;
}

int enqueue() {
	q.arr[++q.head] = 1;
}

int dequeue() {
	q.arr[q.tail++] = 0;
}

void mythread(vector<int> & operations, string name) {
	printf("%s is running.\n", name.c_str());
	for (int i = 0; i < (int)operations.size(); ++i) {
		pthread_mutex_lock(&q.lock);
		if (operations[i]) {
			enqueue();
		} else {
			if (isempty()) {
				dequeue();
			}
		}
		pthread_mutex_unlock(&q.lock);
	}
	while (!isempty()) {
		pthread_mutex_lock(&q.lock);
		dequeue();
		pthread_mutex_unlock(&q.lock);
	}
}

int main() {
	generateOperations(operations1);
	generateOperations(operations2);
//	for (int i = 0; i < (int)operations.size(); ++i) {
//		printf("%d\n", operations[i]);
//	}
	pthread_mutex_init(&q.lock, NULL);
	auto t1 = chrono::high_resolution_clock::now();
	thread th1(mythread, ref(operations1), string("operations1"));
	thread th2(mythread, ref(operations2), string("operations2"));
	th1.join();
	th2.join();
	auto t2 = chrono::high_resolution_clock::now();
	cout << "the standard queue took " << chrono::duration_cast<chrono::milliseconds>(t2-t1).count() << " milliseconds" << endl;
	return 0;
}

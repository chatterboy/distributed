#include <bits/stdc++.h>
using namespace std;

const int maxn = 1000000;

struct Queue { // for standard queue
	int arr[maxn];
	int head;
	int tail;
	pthread_mutex_t lock;
	Queue() : head(-1), tail(-1) {}
} q;

typedef struct __node_t { // for cocurrent queue
	int value;
	struct __node_t * next;
} node_t;

typedef struct __queue_t { // for concurrent queue
	node_t * head;
	node_t * tail;
	pthread_mutex_t headLock;
	pthread_mutex_t tailLock;
} queue_t;


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

void Queue_Init(queue_t * q) {
	node_t * tmp = (node_t *)malloc(sizeof(node_t));
	tmp->next = NULL;
	q->head = q->tail = tmp;
	pthread_mutex_init(&q->headLock, NULL);
	pthread_mutex_init(&q->tailLock, NULL);
}

void Queue_Enqueue(queue_t * q, int value) {
	node_t * tmp = (node_t *)malloc(sizeof(node_t));
	assert(tmp != NULL && "the error was occurred in Queue_Enqueue");
	tmp->value = value;
	tmp->next = NULL;

	pthread_mutex_lock(&q->tailLock);
	q->tail->next = tmp;
	q->tail = tmp;
	pthread_mutex_unlock(&q->tailLock);
}

int Queue_Dequeue(queue_t * q, int * value) {
	pthread_mutex_lock(&q->headLock);
	node_t * tmp = q->head;
	node_t * newHead = tmp->next;
	if (newHead == NULL) {
		pthread_mutex_unlock(&q->headLock);
		return -1; // queue was empty
	}
	*value = newHead->value;
	q->head = newHead;
	pthread_mutex_unlock(&q->headLock);
	free(tmp);
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

void mythread(vector<int> & operations, string name) { // for standard queue
	printf("%s is running in standard queue.\n", name.c_str());
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

void mythread2(vector<int> & operations, string name) { // for concurrent queue
	printf("%s is running in concurrent queue.\n", name.c_str());
	int val;
	queue_t cq;
	Queue_Init(&cq);
	for (int i = 0; i < (int)operations.size(); ++i) {
		if (operations[i]) {
			Queue_Enqueue(&cq, 1);	
		} else {
			Queue_Dequeue(&cq, &val);
		}
	}
	while (Queue_Dequeue(&cq, &val) != -1);
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
	t1 = chrono::high_resolution_clock::now();
	th1 = thread(mythread2, ref(operations1), string("operations1"));
	th2 = thread(mythread2, ref(operations2), string("operations2"));
	th1.join();
	th2.join();
	t2 = chrono::high_resolution_clock::now();
	cout << "the Michael and Scott concurrent queue took " << chrono::duration_cast<chrono::milliseconds>(t2-t1).count() << " milliseconds" << endl;
	return 0;
}

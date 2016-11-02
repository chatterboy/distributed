#include <bits/stdc++.h>
using namespace std;

// basic node structure
typedef struct __node_t {
	int			key;
	struct __node_t		*next;
} node_t;

// basic list structure (one used per list)
typedef struct __list_t {
	node_t			*head;
	pthread_mutex_t		lock;
} list_t;

void List_Init(list_t * L) {
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

int List_Insert(list_t *L, int key) {
	pthread_mutex_lock(&L->lock);
	node_t *nw = (node_t *)malloc(sizeof(node_t));
	if (nw == NULL) {
		perror("malloc");
		pthread_mutex_unlock(&L->lock);
		return -1;
	}
	nw->key = key;
	nw->next = L->head;
	pthread_mutex_unlock(&L->lock);
	return 0;
}

int List_Lookup(list_t *L, int key) {
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	while (curr) {
		if (curr->key == key) {
			pthread_mutex_unlock(&L->lock);
			return 0;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return -1;
}

int main() {
	auto t1 = chrono::high_resolution_clock::now();
	list_t lst;
	List_Init(&lst);
	int n = 10000;
	for (int i = 0; i < n; ++i) List_Insert(&lst, i);
	for (int i = 1; i <= n; ++i) {
		printf("%d\n", List_Lookup(&lst, i));
	}
	auto t2 = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::milliseconds>(t2-t1).count() << endl;
}

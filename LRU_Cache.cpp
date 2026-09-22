#include <iostream>
#include <limits>
#include <unordered_map>

// LRU Cache using a hashmap + doubly linked list
// idea: hashmap gives O(1) lookup, linked list gives O(1) reorder/evict
class LRUCache {
private:
    struct Node {
        int key, value;
        Node* prev;
        Node* next;
        Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    std::unordered_map<int, Node*> cache; // key -> node pointer

    // dummy head/tail so I don't have to handle empty-list edge cases separately
    Node* head;
    Node* tail;

    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // always insert right before tail -> that's the "most recently used" slot
    void insertAtMRU(Node* node) {
        node->prev = tail->prev;
        node->next = tail;
        tail->prev->next = node;
        tail->prev = node;
    }

public:
    LRUCache(int cap) {
        capacity = cap;
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    int get(int key) {
        auto it = cache.find(key);
        if (it == cache.end()) return -1;

        Node* node = it->second;
        removeNode(node);
        insertAtMRU(node);

        return node->value;
    }

    // returns the evicted key, or -1 if nothing was evicted
    int put(int key, int value) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            Node* node = it->second;
            node->value = value;
            removeNode(node);
            insertAtMRU(node);
            return -1;
        }

        Node* newNode = new Node(key, value);
        cache[key] = newNode;
        insertAtMRU(newNode);

        if (cache.size() > static_cast<size_t>(capacity)) {
            Node* lru = head->next;
            int evictedKey = lru->key;
            removeNode(lru);
            cache.erase(lru->key);
            delete lru;
            return evictedKey;
        }
        return -1;
    }

    void display() {
        std::cout << "  cache: ";
        if (cache.empty()) {
            std::cout << "(empty)\n";
            return;
        }

        Node* cur = head->next;
        while (cur != tail) {
            std::cout << "[" << cur->key << ":" << cur->value << "]";
            if (cur->next != tail) std::cout << " -> ";
            cur = cur->next;
        }
        std::cout << "   (oldest -> newest)\n";
    }

    ~LRUCache() {
        Node* cur = head;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }
};

void printMenu() {
    std::cout << "\n----------------------------------------\n";
    std::cout << "1. put   2. get   3. show cache   4. quit\n";
    std::cout << "----------------------------------------\n> ";
}

int main() {

    std::cout << "              LRU CACHE DEMO             \n";
    std::cout << "--------------------------------------------\n";
    std::cout << "cache size: ";
    int capacity;
    std::cin >> capacity;

    if (capacity <= 0) {
        std::cout << "capacity has to be more than 0, exiting.\n";
        return 0;
    }

    LRUCache cache(capacity);
    int choice;

    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "not a valid option, try again\n";
            continue;
        }

        if (choice == 1) {
            int k, v;
            std::cout << "key: ";
            std::cin >> k;
            std::cout << "value: ";
            std::cin >> v;

            int evicted = cache.put(k, v);

            std::cout << "\n>> put(" << k << ", " << v << ")\n";
            if (evicted != -1)
                std::cout << "   evicted key " << evicted << " (cache was full)\n";
            cache.display();
        }
        else if (choice == 2) {
            int k;
            std::cout << "key: ";
            std::cin >> k;

            int res = cache.get(k);

            std::cout << "\n>> get(" << k << ")\n";
            if (res == -1)
                std::cout << "   result: not found\n";
            else
                std::cout << "   result: " << res << "\n";
            cache.display();
        }
        else if (choice == 3) {
            std::cout << "\n>> current state\n";
            cache.display();
        }
        else if (choice == 4) {
            std::cout << "\nbye!\n";
            break;
        }
        else {
            std::cout << "\nnot a valid option, try again\n";
        }
    }

    return 0;
}
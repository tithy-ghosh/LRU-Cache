#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>

using namespace std;



class LRUCache {

private:

    struct Node {

        string key;
        int value;

        // Expiration time in epoch seconds.
        // -1 means the item never expires.
        long long expiry;

        Node* prev;
        Node* next;

        Node(string k, int v, long long exp)
            : key(k), value(v), expiry(exp),
              prev(nullptr), next(nullptr) {}
    };

    int capacity;

    // Key -> Node
    unordered_map<string, Node*> cache;

    // Dummy head and tail nodes.
    Node* head;
    Node* tail;


    // Get current time in seconds.
    static long long nowSeconds() {

        return chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }


    // Check whether a node has expired.
    static bool isExpired(Node* node) {

        return node->expiry >= 0 &&
               nowSeconds() >= node->expiry;
    }


    // Remove a node from the doubly linked list.
    void removeNode(Node* node) {

        node->prev->next = node->next;
        node->next->prev = node->prev;
    }


    // Insert a node at the MRU position.
    void insertAtMRU(Node* node) {

        node->prev = tail->prev;
        node->next = tail;

        tail->prev->next = node;
        tail->prev = node;
    }


    // Completely remove a node.
    void purge(Node* node) {

        removeNode(node);
        cache.erase(node->key);
        delete node;
    }


    // Remove all expired items.
    void removeExpiredItems() {

        Node* current = head->next;

        while (current != tail) {

            Node* nextNode = current->next;

            if (isExpired(current)) {

                cout << "Expired item removed: "
                     << current->key
                     << " = "
                     << current->value
                     << "\n";

                purge(current);
            }

            current = nextNode;
        }
    }


public:

    struct Eviction {

        bool evicted;
        string key;
        int value;
    };


    // Constructor.
    explicit LRUCache(int cap) {

        capacity = cap;

        head = new Node("", 0, -1);
        tail = new Node("", 0, -1);

        head->next = tail;
        tail->prev = head;
    }


    // Prevent copying because the class manages dynamic memory.
    LRUCache(const LRUCache&) = delete;

    LRUCache& operator=(const LRUCache&) = delete;


    // GET operation.
    // Average time complexity: O(1).
    int get(const string& key) {

        auto it = cache.find(key);

        // Key does not exist.
        if (it == cache.end()) {

            return -1;
        }

        Node* node = it->second;

        // Check expiration.
        if (isExpired(node)) {

            purge(node);

            return -1;
        }

        // Accessed item becomes MRU.
        removeNode(node);
        insertAtMRU(node);

        return node->value;
    }


    // PUT operation with optional TTL.
    // Average time complexity: O(1).
    Eviction put(
        const string& key,
        int value,
        long long ttlSeconds
    ) {

        long long expiry;

        if (ttlSeconds > 0) {

            expiry = nowSeconds() + ttlSeconds;
        }
        else {

            expiry = -1;
        }


        // Check whether key already exists.
        auto it = cache.find(key);

        if (it != cache.end()) {

            Node* node = it->second;

            // Update value and expiration.
            node->value = value;
            node->expiry = expiry;

            // Updated item becomes MRU.
            removeNode(node);
            insertAtMRU(node);

            return {false, "", 0};
        }


        // Create a new node.
        Node* node =
            new Node(key, value, expiry);

        // Add to HashMap.
        cache[key] = node;

        // Add to MRU position.
        insertAtMRU(node);


        // Check whether capacity is exceeded.
        if (cache.size() >
            static_cast<size_t>(capacity)) {

            // First real node is the LRU node.
            Node* lru = head->next;

            string evictedKey = lru->key;
            int evictedValue = lru->value;

            purge(lru);

            return {
                true,
                evictedKey,
                evictedValue
            };
        }

        return {false, "", 0};
    }


    // Display cache from LRU to MRU.
    void display() {

        // Remove expired items first.
        removeExpiredItems();

        if (cache.empty()) {

            cout << "\nCache is empty.\n\n";

            return;
        }

        cout << "\nLRU -> ";

        Node* current = head->next;

        bool first = true;

        while (current != tail) {

            if (!first) {

                cout << " -> ";
            }

            cout << "["
                 << current->key
                 << " : "
                 << current->value
                 << "]";

            first = false;

            current = current->next;
        }

        cout << " <- MRU\n\n";
    }


    // Destructor.
    ~LRUCache() {

        Node* current = head;

        while (current != nullptr) {

            Node* nextNode = current->next;

            delete current;

            current = nextNode;
        }
    }
};


// Print program banner.
void printBanner() {


    cout << "LRU CACHE SYSTEM\n";
    cout << "---------------------\n";
}


// Print menu.
void printMenu() {

    
    cout << "---MENU\n";
  

    cout << "1. Put\n";
    cout << "2. Get\n";
    cout << "3. Display Cache\n";
    cout << "4. Exit\n";


    cout << "Enter your choice: ";
}


int main() {

    printBanner();

    int capacity;

    cout << "Enter cache capacity: ";


    // Validate capacity input.
    if (!(cin >> capacity)) {

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        cout << "Invalid input.\n";

        return 0;
    }


    if (capacity <= 0) {

        cout << "Capacity must be greater than 0.\n";

        return 0;
    }


    cout << "\n";


    LRUCache cache(capacity);

    int choice;


    while (true) {

        printMenu();


        // Validate menu input.
        if (!(cin >> choice)) {

            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "\nInvalid choice. Please try again.\n\n";

            continue;
        }


        

        if (choice == 1) {

            string key;
            int value;
            int ttlFlag;

            long long ttlSeconds = 0;


            cout << "\nEnter key: ";

            if (!(cin >> key)) {

                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                cout << "Invalid key.\n\n";

                continue;
            }


            cout << "Enter value: ";

            if (!(cin >> value)) {

                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                cout << "Invalid value.\n\n";

                continue;
            }


            cout << "Do you want TTL/expiration? "
                 << "(1 = Yes, 0 = No): ";

            cin >> ttlFlag;


            // TTL enabled.
            if (ttlFlag == 1) {

                cout << "Enter TTL in seconds: ";

                cin >> ttlSeconds;


                if (ttlSeconds <= 0) {

                    cout << "TTL must be greater than 0.\n\n";

                    continue;
                }
            }


            // Invalid TTL option.
            else if (ttlFlag != 0) {

                cout << "Invalid TTL option.\n\n";

                continue;
            }


            LRUCache::Eviction ev =
                cache.put(
                    key,
                    value,
                    ttlSeconds
                );


            if (ev.evicted) {

                cout << "LRU item removed: "
                     << ev.key
                     << " = "
                     << ev.value
                     << "\n";
            }


            if (ttlFlag == 1) {

                cout << "Item inserted with TTL = "
                     << ttlSeconds
                     << " seconds.\n";
            }

            else {

                cout << "Item inserted without expiration.\n";
            }


            cache.display();
        }



        else if (choice == 2) {

            string key;

            cout << "\nEnter key: ";

            if (!(cin >> key)) {

                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                cout << "Invalid key.\n\n";

                continue;
            }


            int result = cache.get(key);


            if (result == -1) {

                cout << "Key not found or item has expired.\n";
            }

            else {

                cout << "Value = "
                     << result
                     << "\n";
            }


            cache.display();
        }


       

        else if (choice == 3) {

            cache.display();
        }


        

        else if (choice == 4) {

            cout << "\nThank you for using LRU Cache.\n";

            break;
        }


   

        else {

            cout << "\nInvalid choice. Please try again.\n\n";
        }
    }


    return 0;
}
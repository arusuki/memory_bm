#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mimalloc.h>

void *operator new(std::size_t size) { return mi_malloc(size); }

void operator delete(void *ptr) noexcept { mi_free(ptr); }

using random_t = uint64_t;
random_t next(random_t x) {
  x ^= x >> 30;
  x *= 0Xbf58476d1ce4e5b9ULL;
  x ^= x >> 27;
  x *= 0X94d049bb133111ebULL;
  x ^= x >> 31;
  return x;
}

double next_double(random_t x) { return std::scalbn((double)next(x), -64); }

int constexpr MAX_PAYLOAD_SIZE = 50;
int constexpr INITIAL_NODE_COUNT = 10000;
int constexpr MUTATION_COUNT = 1000000L;
int constexpr MAX_MUTATION_SIZE = 200;

struct Node;

struct Node {
  int64_t id;
  int size;
  std::byte *payload;
  Node *previous;
  Node *next;

  Node(int64_t id) : id(id) {
    auto s = static_cast<int>(next_double(id) * MAX_PAYLOAD_SIZE);
    this->size = s;
    this->payload = new std::byte[s];
    for (int i = 0; s > i; ++i) {
      this->payload[i] = static_cast<std::byte>(i);
    }
  }

  Node(Node const &) = delete;

  static void circle2(Node *a, Node *b) {
    a->previous = b;
    a->next = b;
    b->previous = a;
    b->next = a;
  }

  ~Node() {
    this->previous->next = this->next;
    this->next->previous = this->previous;
    delete[] this->payload;
  }

  void insert(Node *n) {
    n->next = this->next;
    n->previous = this;
    this->next->previous = n;
    this->next = n;
  }
};

void main0() {
  auto ttt0 = std::chrono::high_resolution_clock::now();
  int64_t node_id = 0;
  random_t random_state = 0;
  auto head = new Node(node_id++);
  Node::circle2(head, new Node(node_id++));
  for (int i = 2; INITIAL_NODE_COUNT > i; ++i) {
    head->insert(new Node(node_id++));
  }
  int64_t node_count = INITIAL_NODE_COUNT;
  for (int64_t i = 0; MUTATION_COUNT > i; ++i) {
    int delete_count =
        static_cast<int>(next_double(random_state++) * MAX_MUTATION_SIZE);
    if (delete_count > (node_count - 2)) {
      delete_count = static_cast<int>(node_count - 2);
    }
    for (int j = 0; j < delete_count; j++) {
      Node *to_delete = head;
      head = head->previous;
      delete to_delete;
    }
    node_count -= delete_count;
    int insert_count =
        static_cast<int>(next_double(random_state++) * MAX_MUTATION_SIZE);
    for (int j = 0; insert_count > j; ++j) {
      head->insert(new Node(node_id++));
      head = head->next;
    }
    node_count += insert_count;
  }
  int64_t checksum = 0;
  Node *n = head;
  do {
    checksum += n->id + n->size;
    if (n->size > 0) {
      checksum += static_cast<int>(n->payload[0]);
      checksum += static_cast<int>(n->payload[n->size - 1]);
    }
  } while ((n = n->next) != head);

  delete head->next;
  delete head;
  auto sss0 =
      std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
          std::chrono::high_resolution_clock::now() - ttt0);
  std::cout << "Elapsed time: " << sss0.count() << "ms" << std::endl;
  std::cout << "Node count: " << node_count << std::endl;
  std::cout << "Checksum:" << std::hex << std::setw(16) << std::setfill('0')
            << checksum << std::endl;
  std::cout << std::dec;
}

int main() {
  for (int i = 0; 10 > i; ++i) {
    main0();
    std::cout << std::endl;
  }
}
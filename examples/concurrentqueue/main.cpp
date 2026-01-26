#include "concurrentqueue.h"

int main() {
  moodycamel::ConcurrentQueue<int> q;
  q.enqueue(25);
  assert(q.size_approx() == 1);

  int item;
  bool found = q.try_dequeue(item);
  assert(found && item == 25);

  assert(q.size_approx() == 0);
}
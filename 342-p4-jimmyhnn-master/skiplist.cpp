#include "skiplist.h"
#include "random.h"
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// operator<< for printing can access val directly
ostream &operator<<(ostream &out, const SkipList &skip) {
  for (int i = skip.levels - 1; i >= 0; i--) {
    out << "[level: " << i + 1 << "] ";
    SNode *curr = skip.head->next[i];
    while (curr != nullptr) {
      out << curr->val << "-->";
      curr = curr->next[i];
    }
    out << "nullptr" << endl;
  }
  return out;
}

// constructor
SNode::SNode(int val) : val{val} { next.resize(3); }

// copy constructor
SNode::SNode(const SNode &other) {
  val = other.val;
  next = other.next;
}

// checks if the value needs to be inserted at a higher level
// return true p% of time,
// for p = 50, each node has a 50% chance of being at higher level
bool SkipList::shouldInsertAtHigherLevel() const {
  return probability >= Random::random() % 100;
}

// collect all SNode* objects that come before this value at each level
// used as a helper funnction to add and remove
vector<SNode *> SkipList::getBeforeNodes(int val) const {
  vector<SNode *> nodeBefore;
  SNode *curr;
  for (int i = 0; i < levels; i++) {
    curr = head;
    while (curr != nullptr && curr->next[i] != nullptr &&
           curr->next[i]->val < val) {
      curr = curr->next[i];
    }
    nodeBefore.push_back(curr);
  }
  return nodeBefore;
}

// default SkipList has Depth of 1, just one doubly-linked list
SkipList::SkipList(int levels, int probability) {
  this->levels = levels;
  this->probability = probability;
  head = new SNode;
  for (int i = 0; i < levels; i++) {
    head->next.push_back(nullptr);
  }
}

// copy constructor
SkipList::SkipList(const SkipList &other) {
  levels = other.levels;
  probability = other.probability;
  head = new SNode(other.head->val);
  for (int i = 0; i < levels; i++) {
    head->next.push_back(nullptr);
    SNode *temp = other.head->next[i];
    SNode *newHead = head;
    while (temp != nullptr) {
      SNode *curr = new SNode(temp->val);
      newHead->next[i] = curr;
      temp = temp->next[i];
      newHead = newHead->next[i];
    }
  }
}

// destructor
SkipList::~SkipList() {
  // for (int i = 0; i < levels; i++) {
  SNode *curr = head; //->next[i];
  while (curr != nullptr) {
    SNode *temp = curr;
    curr = curr->next[0];
    delete temp;
  }
  //}
  head = nullptr;
  delete head;
}

// Add to list, assume no duplicates
void SkipList::add(int val) {
  SNode *inVal = new SNode(val);
  vector<SNode *> prev = getBeforeNodes(val);
  bool insereted = false;
  for (int i = 0; i < levels; i++) {
    SNode *start = prev[i];
    inVal->next[i] = start->next[i];
    start->next[i] = inVal;
    start = start->next[i];
    if (start == inVal) {
      insereted = true;
    }
    while (i < levels - 1 && shouldInsertAtHigherLevel()) {
      i += 1;
      SNode *curr = prev[i];
      if (curr->val == prev[i]->val && curr->next[i] != nullptr) {
        inVal->next[i] = curr->next[i];
        curr->next[i] = start;
      } else if (curr->val == prev[i]->val && curr->next[i] == nullptr) {
        curr->next[i] = start;
      }
    }
    return;
  }
  if (!insereted) {
    delete inVal;
  }
}

// Add to list, assume no duplicates
void SkipList::add(const vector<int> &values) {
  for (auto value : values) {
    add(value);
  }
}

// return true if successfully removed
bool SkipList::remove(int val) {
  bool result = false;
  if (!contains(val)) {
    return result;
  }
  SNode *curr;
  SNode *remove = nullptr;
  vector<SNode *> prevN = getBeforeNodes(val);
  for (int i = levels - 1; i >= 0; i--) {
    curr = prevN[i];
    if (curr->next[i]->val == val) {
      remove = curr->next[i];
      curr->next[i] = curr->next[i]->next[i];
      result = true;
    } else {
      curr = curr->next[i];
    }
  }
  delete remove;
  return result;
}

// return true if found in SkipList
bool SkipList::contains(int val) const {
  for (int i = levels - 1; i >= 0; i--) {
    SNode *curr = head;
    curr = curr->next[i];
    while (curr != nullptr && curr->val < val) {
      curr = curr->next[i];
    }
    if (curr != nullptr && curr->val == val) {
      return true;
    }
  }
  return false;
}

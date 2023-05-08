#ifndef ACM_TICKETSYSTEM_BPT
#define ACM_TICKETSYSTEM_BPT

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <fstream>
#include "list.h"
#include "hashmap.h"
#include "vector.h"
#include "MemPool.h"

const int headSize = 4 * (int)sizeof(int);
//const int CacheSize = 1000;

template<class Key_Type, class Value_Type, class Hash_Func = std::hash<int>>
class BPTree {

public:

  class myPair {
  public:
    Key_Type key;
    int pos;
    myPair() :pos(0) {}
    myPair(Key_Type _key, int _pos) :key(_key), pos(_pos) {}
    myPair(const myPair& other) {
      key = other.key; pos = other.pos;
    }
    ~myPair() {}
    bool operator <(const myPair& other)const {
      return key < other.key;
    }
    bool operator >(const myPair& other)const {
      return key > other.key;
    }
    bool operator ==(const myPair& other)const {
      return key == other.key;
    }
    bool operator !=(const myPair& other)const {
      return !((*this) == other);
    }
    bool operator <=(const myPair& other)const {
      return (*this) < other || (*this) == other;
    }
    bool operator >=(const myPair& other)const {
      return (*this) > other || (*this) == other;
    }
  };

  const static int maxNodeSize = 4096 / (sizeof(myPair) + sizeof(int));
  const static int minNodeSize = maxNodeSize / 2 - 1;

  class Node {
  public:
    myPair _array[maxNodeSize];
    int pre, nxt;
    int index, fa;
    int NodeSize;
    bool is_leaf;
    Node() :index(0), pre(-1), nxt(-1), fa(-1), NodeSize(0), is_leaf(1) {}
    Node(int _index) :index(_index), pre(-1), nxt(-1), fa(-1), NodeSize(0), is_leaf(1) {}
    ~Node() {}
    int LowerBoundKey(const Key_Type& _key)const {
      int l = 0, r = NodeSize - 1, _index = NodeSize;
      while (l <= r) {
        int mid = (l + r) >> 1;
        if (_array[mid].key >= _key) {
          r = mid - 1; _index = mid;
        }
        else l = mid + 1;
      }
      return _index;
    }
  };

  const static int CacheSize = (1 << 20) / sizeof(Node);

  void readNode(int pos, Node& _node) {
    index_file.seekg(headSize + pos * sizeof(Node));
    index_file.read(reinterpret_cast<char*>(&_node), sizeof(Node));
  }

  void writeNode(int pos, Node& _node) {
    index_file.seekp(headSize + pos * sizeof(Node));
    index_file.write(reinterpret_cast<char*>(&_node), sizeof(Node));
  }

  void readVal(int pos, Value_Type& _val) {
    value_file.seekg(headSize + pos * sizeof(Value_Type));
    value_file.read(reinterpret_cast<char*>(&_val), sizeof(Value_Type));
  }

  void writeVal(int pos, Value_Type& _val) {
    value_file.seekp(headSize + pos * sizeof(Value_Type));
    value_file.write(reinterpret_cast<char*>(&_val), sizeof(Value_Type));
  }

  class LRUCacheManager {
    friend class BPTree;
  private:

    BPTree<Key_Type, Value_Type, Hash_Func>* BPT;
    list<std::pair<int, Node>> ls;
    HashMap<int, typename list<std::pair<int, Node>>::iterator, Hash_Func> map;
    int capacity;

  public:

    LRUCacheManager(BPTree<Key_Type, Value_Type, Hash_Func>* _BPT) :BPT(_BPT), capacity(_BPT->CacheSize) {}

    ~LRUCacheManager() {}

    void dump() {
      typename list<std::pair<int, Node>>::iterator it = ls.front();
      while (it != ls.end()) {
        BPT->writeNode((*it).first, (*it).second);
        it++;
      }
    }
    //清空缓存将数据全部写入外存

    void insert(const int _key, const Node& _val) {
      if (!map.find(_key)) {
        if (ls.size() == capacity) {
          BPT->writeNode((*ls.front()).first, (*ls.front()).second);
          //当数据被弹出缓存时将其写入外存
          map.erase((*ls.front()).first);
          ls.del(ls.front());
        }
      }
      else ls.del(map[_key]);
      map[_key] = ls.insert(std::make_pair(_key, _val));
    }

    void find(const int _key, Node& _node) {
      if (!map.find(_key)) {
        BPT->readNode(_key, _node); insert(_key, _node);
        return;
      }
      _node = (*map[_key]).second;
      ls.del(map[_key]);
      map[_key] = ls.insert(std::make_pair(_key, _node));
    }

    void clear() {
      map.clear();
      ls.clear();
    }

  };

  //read 和 write 由 LRU 接管

  int nodeCnt;
  int valCnt;
  int root_index;
  int siz;
  std::fstream index_file, value_file;
  LRUCacheManager Cache;
  MemPool<int>Mem;
  Node null;

  int alloc_Node() {
    if (Mem.empty()) return ++nodeCnt;
    else return Mem.back();
  }

public:

  BPTree(const std::string& file_name) :Cache(this), Mem(file_name) {
    index_file.open(file_name + "_Node.db", std::ios::binary | std::ios::out | std::ios::in);
    value_file.open(file_name + "_Val.db", std::ios::binary | std::ios::out | std::ios::in);
    if (index_file.good()) {
      index_file.seekg(0);
      index_file.read(reinterpret_cast<char*>(&nodeCnt), sizeof(int));
      index_file.read(reinterpret_cast<char*>(&valCnt), sizeof(int));
      index_file.read(reinterpret_cast<char*>(&siz), sizeof(int));
      index_file.read(reinterpret_cast<char*>(&root_index), sizeof(int));
    }
    else {
      std::fstream create;
      create.open(file_name + "_Node.db", std::ios::out); create.close();
      index_file.open(file_name + "_Node.db", std::ios::binary | std::ios::out | std::ios::in);
      create.open(file_name + "_Val.db", std::ios::out); create.close();
      value_file.open(file_name + "_Val.db", std::ios::binary | std::ios::out | std::ios::in);
      valCnt = 0;
      nodeCnt = -1;
      Node _root(++nodeCnt);
      Cache.insert(root_index = _root.index, _root);
      siz = 0;
    }
  }
  ~BPTree() {
    index_file.seekp(0);
    index_file.write(reinterpret_cast<char*>(&nodeCnt), sizeof(int));
    index_file.write(reinterpret_cast<char*>(&valCnt), sizeof(int));
    index_file.write(reinterpret_cast<char*>(&siz), sizeof(int));
    index_file.write(reinterpret_cast<char*>(&root_index), sizeof(int));
    Cache.dump();
    index_file.close();
    value_file.close();
  }

  bool empty() {
    return siz == 0;
  }
  int size() {
    return siz;
  }

public:

  class iterator {
    BPTree<Key_Type, Value_Type, Hash_Func>* BPT;
    Node curNode;
    int pos;
  public:
    iterator(BPTree<Key_Type, Value_Type, Hash_Func>* _BPT, const Node& _Node, const int& _pos) {
      BPT = _BPT; curNode = _Node; pos = _pos;
    }
    iterator(const iterator& other) {
      BPT = other.BPT; curNode = other.curNode; pos = other.pos;
    }
    ~iterator() {}

    iterator& operator =(const iterator& other) {
      if (this == &other) return (*this);
      BPT = other.BPT; curNode = other.curNode; pos = other.pos;
      return (*this);
    }

    Value_Type operator *() {
      Value_Type tmp;
      BPT->readVal(curNode._array[pos].pos, tmp);
      return tmp;
    }

    Value_Type get_val() {
      Value_Type tmp;
      BPT->readVal(curNode._array[pos].pos, tmp);
      return tmp;
    }

    Key_Type get_key() {
      return curNode._array[pos].key;
    }

    bool operator ==(const iterator& other) {
      if (BPT != other.BPT) throw(exceptions("Not In The Same BPT"));
      if (curNode.index != other.curNode.index) return false;
      if (pos != other.pos) return false;
      return true;
    }
    bool operator !=(const iterator& other) {
      return !((*this) == other);
    }
    void get_nxt() {
      if (pos == -1) throw(exceptions("Iterator Out Of Bound"));
      if (pos == curNode.NodeSize - 1) {
        if (curNode.nxt != -1) {
          BPT->Cache.find(curNode.nxt, curNode);
          pos = 0;
        }
        else {
          curNode = BPT->null;
          pos = -1;
        }
      }
      else pos++;
    }
    void get_pre() {
      if (pos == 0) {
        if (curNode.pre != -1) {
          BPT->Cache.find(curNode.pre, curNode);
          pos = curNode.NodeSize - 1;
        }
        else throw(exceptions("Iterator Out Of Bound"));
      }
      else pos--;
    }
    iterator& operator ++() {
      this->get_nxt();
      return (*this);
    }
    iterator& operator --() {
      this->get_pre();
      return (*this);
    }
    iterator operator ++(int x) {
      iterator tmp((*this));
      this->get_nxt();
      return tmp;
    }
    iterator operator --(int x) {
      iterator tmp((*this));
      this->get_pre();
      return tmp;
    }
  };

  iterator begin() {
    Node now; Cache.find(root_index, now);
    while (!now.is_leaf)
      Cache.find(now._array[0].pos, now);
    return iterator(this, now, 0);
  }

  iterator end() {
    return iterator(this, null, -1);
  }

private:

  void split(Node& now, Node& tar) {
    now.is_leaf = tar.is_leaf; now.fa = tar.fa;
    Node tmp;
    for (int i = tar.NodeSize / 2; i < tar.NodeSize; i++) {
      now._array[now.NodeSize++] = tar._array[i];
      if (!tar.is_leaf) {
        Cache.find(tar._array[i].pos, tmp);
        tmp.fa = now.index;
        Cache.insert(tar._array[i].pos, tmp);
      }
    }
    tar.NodeSize /= 2;
  }

  void merge(Node& now, Node& tar) {
    Node tmp;
    for (int i = 0; i < now.NodeSize; i++) {
      tar._array[tar.NodeSize++] = now._array[i];
      if (!now.is_leaf) {
        Cache.find(now._array[i].pos, tmp);
        tmp.fa = tar.index;
        Cache.insert(now._array[i].pos, tmp);
      }
    }
    Mem.insert(now.index);
  }

  Node findNodebyKey(const Key_Type& _key) {
    Node now; Cache.find(root_index, now);
    while (!now.is_leaf) {
      int l = 0, r = now.NodeSize - 1, _index = 0;
      while (l <= r) {
        int mid = (l + r) >> 1;
        if (now._array[mid].key > _key) r = mid - 1;
        else {
          l = mid + 1;
          _index = mid;
        }
      }
      Cache.find(now._array[_index].pos, now);
    }
    return now;
  }

  void splitNode(int nowPos) {
    Node now; Cache.find(nowPos, now);
    Node newNode(alloc_Node());
    split(newNode, now);
    if (now.index != root_index) {
      if (now.nxt != -1) {
        Node nxtNode; Cache.find(now.nxt, nxtNode);
        nxtNode.pre = newNode.index;
        Cache.insert(nxtNode.index, nxtNode);
      }
      newNode.nxt = now.nxt;
      now.nxt = newNode.index; newNode.pre = now.index;
      Cache.insert(now.index, now);
      Cache.insert(newNode.index, newNode);
      Node fa; Cache.find(now.fa, fa);
      int pos = fa.LowerBoundKey(newNode._array[0].key);
      for (int i = fa.NodeSize; i > pos; i--) fa._array[i] = fa._array[i - 1];
      fa._array[pos].key = newNode._array[0].key;
      fa._array[pos].pos = newNode.index; fa.NodeSize++;
      Cache.insert(fa.index, fa);
      if (fa.NodeSize >= maxNodeSize) splitNode(fa.index);
    }
    else {
      Node _root(alloc_Node());
      _root._array[0].key = now._array[0].key;
      _root._array[0].pos = now.index;
      _root._array[1].key = newNode._array[0].key;
      _root._array[1].pos = newNode.index;
      now.nxt = newNode.index; newNode.pre = now.index;
      now.fa = newNode.fa = _root.index;
      _root.NodeSize = 2; _root.is_leaf = 0;
      Cache.insert(_root.index, _root);
      Cache.insert(now.index, now); Cache.insert(newNode.index, newNode);
      root_index = _root.index;
    }
  }

  void mergeNode(int nowPos) {
    Node now; Cache.find(nowPos, now);
    if (now.index == root_index) {
      if (now.NodeSize == 1 && !now.is_leaf) {
        Node tmp; Cache.find(now._array[0].pos, tmp);
        tmp.fa = -1; root_index = tmp.index;
        Cache.insert(tmp.index, tmp);
      }
      return;
    }
    Node fa, other; Cache.find(now.fa, fa);
    if (now.index == fa._array[0].pos)
      Cache.find(now.nxt, other);
    else {
      Cache.find(now.pre, other);
      std::swap(other, now);
    }
    merge(other, now);
    if (other.nxt != -1) {
      Node nxtNode; Cache.find(other.nxt, nxtNode);
      nxtNode.pre = now.index;
      Cache.insert(nxtNode.index, nxtNode);
    }
    now.nxt = other.nxt;
    Cache.insert(now.index, now);
    int pos = fa.LowerBoundKey(other._array[0].key);
    for (int i = pos; i < fa.NodeSize; i++) fa._array[i] = fa._array[i + 1];
    fa.NodeSize--;
    Cache.insert(fa.index, fa);
    if (fa.NodeSize < minNodeSize) maintainNode(fa.index);
  }

  void update(Node now, Key_Type _key) {
    Node fa; Cache.find(now.fa, fa);
    while (now.index == fa._array[0].pos) {
      fa._array[0].key = now._array[0].key;
      Cache.insert(fa.index, fa); now = fa;
      if (fa.index == root_index) return;
      Cache.find(now.fa, fa);
    }
    int pos = fa.LowerBoundKey(_key);
    fa._array[pos].key = now._array[0].key;
    Cache.insert(fa.index, fa);
  }

  void maintainNode(int nowPos) {
    Node now; Cache.find(nowPos, now);
    if (now.index == root_index) {
      if (now.NodeSize == 1 && !now.is_leaf) {
        Node tmp; Cache.find(now._array[0].pos, tmp);
        tmp.fa = -1; root_index = tmp.index;
        Cache.insert(tmp.index, tmp);
      }
      return;
    }
    Node fa; Cache.find(now.fa, fa);
    if (now.index == fa._array[0].pos) {
      Node nxtNode; Cache.find(now.nxt, nxtNode);
      if (nxtNode.NodeSize > minNodeSize) {
        int pos = fa.LowerBoundKey(nxtNode._array[0].key);
        now._array[now.NodeSize++] = nxtNode._array[0];
        if (!now.is_leaf) {
          Node tmp; Cache.find(nxtNode._array[0].pos, tmp);
          tmp.fa = now.index; Cache.insert(tmp.index, tmp);
        }
        for (int i = 0; i < nxtNode.NodeSize; i++) nxtNode._array[i] = nxtNode._array[i + 1];
        nxtNode.NodeSize--;
        Cache.insert(now.index, now); Cache.insert(nxtNode.index, nxtNode);
        fa._array[pos].key = nxtNode._array[0].key;
        Cache.insert(fa.index, fa);
      }
      else mergeNode(nowPos);
    }
    else {
      Node preNode; Cache.find(now.pre, preNode);
      if (preNode.NodeSize > minNodeSize) {
        int pos = fa.LowerBoundKey(now._array[0].key);
        for (int i = now.NodeSize; i > 0; i--) now._array[i] = now._array[i - 1];
        now._array[0] = preNode._array[--preNode.NodeSize]; now.NodeSize++;
        if (!now.is_leaf) {
          Node tmp; Cache.find(now._array[0].pos, tmp);
          tmp.fa = now.index; Cache.insert(tmp.index, tmp);
        }
        Cache.insert(now.index, now); Cache.insert(preNode.index, preNode);
        fa._array[pos].key = now._array[0].key;
        Cache.insert(fa.index, fa);
      }
      else mergeNode(nowPos);
    }
  }

public:

  void insert(Key_Type _key, Value_Type _val) {
    Node now = findNodebyKey(_key);
    int _index = now.LowerBoundKey(_key);
    for (int i = now.NodeSize; i > _index; i--) now._array[i] = now._array[i - 1];
    now._array[_index].key = _key;
    writeVal(++valCnt, _val);
    now._array[_index].pos = valCnt;
    siz++; now.NodeSize++;
    Cache.insert(now.index, now);
    if (!_index && now.fa != -1) update(now, now._array[1].key);
    if (now.NodeSize >= maxNodeSize) splitNode(now.index);
  }

  void del(Key_Type _key) {
    Node now = findNodebyKey(_key);
    int _index = now.LowerBoundKey(_key);
    if (_index == now.NodeSize || now._array[_index].key != _key) return;
    for (int i = _index; i < now.NodeSize; i++) now._array[i] = now._array[i + 1];
    now.NodeSize--; siz--;
    Cache.insert(now.index, now);
    if (!now.NodeSize) return;
    if (!_index && now.fa != -1) update(now, _key);
    if (now.NodeSize < minNodeSize) maintainNode(now.index);
  }

  Value_Type findVal(const Key_Type& _key) {
    Node now = findNodebyKey(_key);
    int pos = now.LowerBoundKey(_key);
    Value_Type tmp;
    while (true) {
      for (int i = pos; i < now.NodeSize; i++) {
        if (now._array[i].key > _key) return Value_Type();
        if (now._array[i].key == _key) {
          readVal(now._array[i].pos, tmp);
          return tmp;
        }
      }
      pos = 0;
      if (now.nxt == -1) return Value_Type();
      Cache.find(now.nxt, now);
    }
  }

  int findPos(const Key_Type& _key) {
    Node now = findNodebyKey(_key);
    int pos = now.LowerBoundKey(_key);
    vector<int> vec;
    while (true) {
      for (int i = pos; i < now.NodeSize; i++) {
        if (now._array[i].key > _key) return 0;
        if (now._array[i].key == _key) now._array[i].pos;
      }
      pos = 0;
      if (now.nxt == -1) return 0;
      Cache.find(now.nxt, now);
    }
  }

  int count(const Key_Type& _key) {
    Node now = findNodebyKey(_key);
    int pos = now.LowerBoundKey(_key);
    int cnt = 0;
    while (true) {
      for (int i = pos; i < now.NodeSize; i++) {
        if (now._array[i].key > _key) return 0;
        if (now._array[i].key == _key) return 1;
      }
      pos = 0;
      if (now.nxt == -1) return 0;
      Cache.find(now.nxt, now);
    }
  }

  iterator lower_bound(const Key_Type& _key) {
    Node now = findNodebyKey(_key);
    int pos = now.LowerBoundKey(_key);
    while (true) {
      for (int i = pos; i < now.NodeSize; i++) {
        if (now._array[i].key >= _key) return iterator(this, now, i);
      }
      pos = 0;
      if (now.nxt == -1) return end();
      Cache.find(now.nxt, now);
    }
  }

  void clear() {
    Mem.clear();
    nodeCnt = -1;
    Node _root(++nodeCnt);
    Cache.insert(root_index = _root.index, _root);
    siz = 0;
  }

  //The rest function are for debug

  void debug(int pos) {
    Node now; Cache.find(pos, now);
    std::cout << "Node " << now.index << std::endl;
    std::cout << "NodeSize " << now.NodeSize << std::endl;
    std::cout << "Isleaf " << now.is_leaf << std::endl;
    std::cout << "fa " << now.fa << std::endl;
    std::cout << "pre " << now.pre << std::endl;
    std::cout << "nxt " << now.nxt << std::endl;
    if (now.is_leaf) {
      std::cout << "Data" << std::endl;
      for (int i = 0; i < now.NodeSize; i++)
        std::cout << now._array[i].key << std::endl;
      std::cout << "End" << std::endl;
    }
    else {
      std::cout << "Son Node" << std::endl;
      for (int i = 0; i < now.NodeSize; i++) {
        std::cout << now._array[i].key << " " << now._array[i].pos << std::endl;
      }
      std::cout << "End" << std::endl;
    }
    if (!now.is_leaf) {
      for (int i = 0; i < now.NodeSize; i++)
        debug(now._array[i].pos);
    }
  }

};

#endif
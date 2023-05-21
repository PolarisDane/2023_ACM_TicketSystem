#ifndef ACM_TICKETSYSTEM_BPT
#define ACM_TICKETSYSTEM_BPT

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <fstream>
#include "exceptions.h"
#include "list.h"
#include "hashmap.h"
#include "MemPool.h"

const int headSize = 4 * (int)sizeof(int);

template<class Key_Type, class Value_Type, class Hash_Func = std::hash<int>>
class BPTree {

public:

  class myPair {
  public:
    Key_Type key;
    int pos;
    myPair() {}
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
  const static int minNodeSize = maxNodeSize / 3 - 1;

  class Node {
  public:
    myPair _array[maxNodeSize];
    int pre, nxt;
    int index;
    int NodeSize;
    bool is_leaf;
    Node() :index(-1), pre(-1), nxt(-1), NodeSize(0), is_leaf(1) {}
    Node(int _index) :index(_index), pre(-1), nxt(-1), NodeSize(0), is_leaf(1) {}
    ~Node() {}
    void insert(const myPair& data, int pos) {
      for (int i = NodeSize; i >= pos; i--) _array[i] = _array[i - 1];
      NodeSize++; _array[pos] = data;
    }
    void del(int pos) {
      NodeSize--;
      for (int i = pos; i <= NodeSize - 1; i++) _array[i] = _array[i + 1];
    }
    int LowerBoundKey(const Key_Type& _key)const {
      int l = 0, r = NodeSize - 1, _index = NodeSize;
      while (l <= r) {
        int mid = (l + r) >> 1;
        if (_array[mid].key < _key) l = mid + 1;
        else {
          r = mid - 1; _index = mid;
        }
      }
      return _index;
    }
    int UpperBoundKey(const Key_Type& _key)const {
      int l = 0, r = NodeSize - 1, _index = NodeSize;
      while (l <= r) {
        int mid = (l + r) >> 1;
        if (_array[mid].key <= _key) l = mid + 1;
        else {
          r = mid - 1; _index = mid;
        }
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

    void insert(const int _key, Node& _val) {
      //BPT->writeNode(_key, _val);
      if (!map.find(_key)) {
        if (ls.size() == capacity) {
          BPT->writeNode((*ls.front()).first, (*ls.front()).second);
          //当数据被弹出缓存时将其写入外存
          map.erase((*ls.front()).first);
          ls.del(ls.front());
        }
        map[_key] = ls.insert(std::make_pair(_key, _val));
      }
      else {
        (*map[_key]).second = _val;
        ls.move(map[_key]);
      }
    }

    void find(const int _key, Node& _node) {
      //BPT->readNode(_key, _node);
      if (!map.find(_key)) {
        BPT->readNode(_key, _node); insert(_key, _node);
        return;
      }
      _node = (*map[_key]).second;
      ls.move(map[_key]);
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
    return ++nodeCnt;
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

  void exit() {
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
      if (curNode.index == -1) throw(exceptions("Iterator Out Of Bound"));
      if (pos == curNode.NodeSize - 1) {
        if (curNode.nxt != -1) BPT->Cache.find(curNode.nxt, curNode);
        else curNode = BPT->null;
        pos = 0;
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
    return iterator(this, null, 0);
  }

private:

  void split(Node& now, Node& tar) {
    now.is_leaf = tar.is_leaf;
    for (int i = tar.NodeSize / 2; i < tar.NodeSize; i++)
      now._array[now.NodeSize++] = tar._array[i];
    tar.NodeSize /= 2;
  }

  void merge(Node& now, Node& tar) {
    for (int i = 0; i < now.NodeSize; i++)
      tar._array[tar.NodeSize++] = now._array[i];
    Mem.insert(now.index);
  }

  void splitNode(Node& now, Node& fa, const int& now_pos) {
    Node newNode(alloc_Node());
    split(newNode, now);
    newNode.nxt = now.nxt; newNode.pre = now.index;
    now.nxt = newNode.index;
    Cache.insert(now.index, now); Cache.insert(newNode.index, newNode);
    if (now.index == root_index) {
      Node _root(alloc_Node());
      _root._array[0].key = now._array[now.NodeSize - 1].key;
      _root._array[0].pos = now.index;
      _root._array[1].key = newNode._array[newNode.NodeSize - 1].key;
      _root._array[1].pos = newNode.index;
      _root.NodeSize = 2; _root.is_leaf = 0;
      Cache.insert(_root.index, _root);
      root_index = _root.index;
    }
    else {
      if (newNode.nxt != -1) {
        Node nxtNode; Cache.find(newNode.nxt, nxtNode);
        nxtNode.pre = newNode.index;
        Cache.insert(nxtNode.index, nxtNode);
      }
      fa._array[now_pos].key = now._array[now.NodeSize - 1].key;
      fa.insert(myPair(newNode._array[newNode.NodeSize - 1].key, newNode.index), now_pos + 1);
    }
  }

  void maintainNode(Node& now, Node& fa, int now_pos) {
    Node other;
    if (!now_pos) Cache.find(now.nxt, other);
    else {
      Cache.find(now.pre, other);
      std::swap(now, other); now_pos--;
    }
    if (now.NodeSize <= minNodeSize && other.NodeSize <= minNodeSize) {
      merge(other, now);
      now.nxt = other.nxt;
      Cache.insert(now.index, now);
      if (now.nxt != -1) {
        Node nxtNode; Cache.find(now.nxt, nxtNode);
        nxtNode.pre = now.index; Cache.insert(nxtNode.index, nxtNode);
      }
      if (fa.index == root_index && fa.NodeSize == 2) {
        Mem.insert(root_index); root_index = now.index;
      }
      else {
        fa._array[now_pos].key = now._array[now.NodeSize - 1].key;
        fa.del(now_pos + 1);
      }
    }
    else {
      if (now.NodeSize > minNodeSize) {
        other.insert(now._array[now.NodeSize - 1], 0);
        now.del(now.NodeSize - 1);
      }
      else {
        now.insert(other._array[0], now.NodeSize);
        other.del(0);
      }
      fa._array[now_pos].key = now._array[now.NodeSize - 1].key;
      Cache.insert(now.index, now); Cache.insert(other.index, other);
    }
  }

  void insert(const Key_Type& _key, const int& _val, Node& now, Node& fa, const int& now_pos) {
    int pos = now.LowerBoundKey(_key);
    if (pos == now.NodeSize && fa.index != -1) fa._array[now_pos].key = _key;
    if (now.is_leaf) now.insert(myPair(_key, _val), pos);
    else {
      if (pos == now.NodeSize) pos--;
      Node nxt_now; Cache.find(now._array[pos].pos, nxt_now);
      insert(_key, _val, nxt_now, now, pos);
    }
    if (now.NodeSize >= maxNodeSize) splitNode(now, fa, now_pos);
    else Cache.insert(now.index, now);
  }

  void del(const Key_Type& _key, Node& now, Node& fa, const int& now_pos) {
    int pos = now.LowerBoundKey(_key);
    if (pos == now.NodeSize) return;//Can't be found
    if (now.is_leaf && now._array[pos].key != _key) return;//Not Found
    bool flag = now.index == root_index;
    if (now.is_leaf) {
      now.del(pos); siz--;
    }
    else {
      Node nxt_now; Cache.find(now._array[pos].pos, nxt_now);
      del(_key, nxt_now, now, pos);
    }
    if (fa.index != -1) fa._array[now_pos].key = now._array[now.NodeSize - 1].key;
    if (!flag && now.NodeSize < minNodeSize) maintainNode(now, fa, now_pos);
    else Cache.insert(now.index, now);
  }

public:

  void insert(const Key_Type& _key, Value_Type _val) {
    int pos = ++valCnt; siz++;
    Node root; Cache.find(root_index, root);
    insert(_key, pos, root, null, 0);
    writeVal(pos, _val);
  }

  void del(const Key_Type& _key) {
    Node root; Cache.find(root_index, root);
    del(_key, root, null, 0);
  }//没有解引用

  iterator lower_bound(const Key_Type& _key) {
    Node now; Cache.find(root_index, now);
    while (1) {
      int pos = now.LowerBoundKey(_key);
      if (pos == now.NodeSize) return end();
      if (now.is_leaf) return iterator(this, now, pos);
      Cache.find(now._array[pos].pos, now);
    }
  }

  bool count(const Key_Type& _key) {
    Node now; Cache.find(root_index, now);
    while (1) {
      int pos = now.LowerBoundKey(_key);
      if (pos == now.NodeSize) return false;
      if (now.is_leaf) {
        if (now._array[pos].key == _key) return true;
        else return false;
      }
      Cache.find(now._array[pos].pos, now);
    }
  }

  int find(const Key_Type& _key) {
    Node now; Cache.find(root_index, now);
    while (1) {
      int pos = now.LowerBoundKey(_key);
      if (pos == now.NodeSize) return 0;
      if (now.is_leaf) {
        if (now._array[pos].key == _key) return now._array[pos].pos;
        else return 0;
      }
      Cache.find(now._array[pos].pos, now);
    }
  }

  void clear() {
    Mem.clear();
    Cache.clear();
    nodeCnt = -1;
    valCnt = 0;
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
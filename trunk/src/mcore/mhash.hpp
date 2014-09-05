#ifndef MHASH_HPP
#define MHASH_HPP

#include <map>

using namespace std;

template<class K, class T>
class MHash : public map<K, T>
{
public:
    bool contains(K key) {
        class MHash<K, T>::iterator iter = find(key);
        return iter != this->end();
    }
};

#endif // MHASH_HPP

#ifndef MHASH_HPP
#define MHASH_HPP

#include <map>

using namespace std;

template<class K, class T>
class MHash : public map<K, T>
{
public:
    bool contains(K key) const {
        class MHash<K, T>::const_iterator iter = find(key);
        return iter != this->end();
    }
};

#endif // MHASH_HPP

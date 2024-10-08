#ifndef BITCOIN_LIMITEDMAP_H
#define BITCOIN_LIMITEDMAP_H

#include <map>
#include <deque>


template <typename K, typename V> class limitedmap
{
public:
    typedef K key_type;
    typedef V mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef typename std::map<K, V>::const_iterator const_iterator;
    typedef typename std::map<K, V>::size_type size_type;

protected:
    std::map<K, V> map;
    typedef typename std::map<K, V>::iterator iterator;
    std::multimap<V, iterator> rmap;
    typedef typename std::multimap<V, iterator>::iterator rmap_iterator;
    size_type nMaxSize;

public:
    limitedmap(size_type nMaxSizeIn = 0) { nMaxSize = nMaxSizeIn; }
    const_iterator begin() const { return map.begin(); }
    const_iterator end() const { return map.end(); }
    size_type size() const { return map.size(); }
    bool empty() const { return map.empty(); }
    const_iterator find(const key_type& k) const { return map.find(k); }
    size_type count(const key_type& k) const { return map.count(k); }
    void insert(const value_type& x)
    {
        std::pair<iterator, bool> ret = map.insert(x);
        if (ret.second)
        {
            if (nMaxSize && map.size() == nMaxSize)
            {
                map.erase(rmap.begin()->second);
                rmap.erase(rmap.begin());
            }
            rmap.insert(make_pair(x.second, ret.first));
        }
        return;
    }
    void erase(const key_type& k)
    {
        iterator itTarget = map.find(k);
        if (itTarget == map.end())
            return;
        std::pair<rmap_iterator, rmap_iterator> itPair = rmap.equal_range(itTarget->second);
        for (rmap_iterator it = itPair.first; it != itPair.second; ++it)
            if (it->second == itTarget)
            {
                rmap.erase(it);
                map.erase(itTarget);
                return;
            }

        assert(0); 
        map.erase(itTarget);
    }
    void update(const_iterator itIn, const mapped_type& v)
    {

        iterator itTarget = map.find(itIn->first);
        if (itTarget == map.end())
            return;
        std::pair<rmap_iterator, rmap_iterator> itPair = rmap.equal_range(itTarget->second);
        for (rmap_iterator it = itPair.first; it != itPair.second; ++it)
            if (it->second == itTarget)
            {
                rmap.erase(it);
                itTarget->second = v;
                rmap.insert(make_pair(v, itTarget));
                return;
            }

        assert(0); 
        itTarget->second = v;
        rmap.insert(make_pair(v, itTarget));
    }
    size_type max_size() const { return nMaxSize; }
    size_type max_size(size_type s)
    {
        if (s)
            while (map.size() > s)
            {
                map.erase(rmap.begin()->second);
                rmap.erase(rmap.begin());
            }
        nMaxSize = s;
        return nMaxSize;
    }
};

#endif

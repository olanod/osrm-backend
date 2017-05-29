#ifndef OSRM_UTIL_VECTOR_ACCESS_HISTOGRAM_HPP
#define OSRM_UTIL_VECTOR_ACCESS_HISTOGRAM_HPP

#include <mutex>
#include <vector>

namespace osrm
{
namespace util
{

template<typename VectorT, std::size_t BinSize = 1000>
class vector_access_histogram : public VectorT
{
    using Base = VectorT;
    using Base::Base;
    using reference = typename Base::reference;
    using const_reference = typename Base::const_reference;
    using size_type = typename Base::size_type;

    reference operator[](size_type pos)
    {
        count(pos);
        return Base::operator[](pos);
    }

    const_reference operator[](size_type pos) const
    {
        count(pos);
        return Base::operator[](pos);
    }

    void count(size_type pos) const
    {
        if (pos / BinSize >= counters.size())
        {
            std::lock_guard<std::mutex> guard(counters_lock);
            counters.resize(pos / BinSize + 1);
        }

        counters[pos / BinSize]++;
    }

    mutable std::mutex counters_lock;
    mutable std::vector<std::uint32_t> counters;
};

}
}

#endif

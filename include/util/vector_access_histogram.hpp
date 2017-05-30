#ifndef OSRM_UTIL_VECTOR_ACCESS_HISTOGRAM_HPP
#define OSRM_UTIL_VECTOR_ACCESS_HISTOGRAM_HPP

#include "util/integer_range.hpp"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <sstream>
#include <vector>

namespace osrm
{
namespace util
{
namespace detail
{
extern std::atomic_uint operation;
}

template <typename VectorT, std::size_t OperationBinSize = 1000, std::size_t IndexBinSize = 1000>
class vector_access_histogram : public VectorT
{
    using Base = VectorT;
    using Base::Base;
    using reference = typename Base::reference;
    using const_reference = typename Base::const_reference;
    using size_type = typename Base::size_type;

  public:
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

    std::string dump() const
    {
        std::stringstream out;

        frame_offsets.push_back(frame_counters.size());

        for (const auto frame_index : irange<std::size_t>(0, frame_offsets.size() - 1))
        {
            auto begin = frame_counters.begin() + frame_offsets[frame_index];
            auto end = frame_counters.begin() + frame_offsets[frame_index + 1];

            auto bin_index = 0;
            std::for_each(begin, end, [&](const auto count) {
                if (count >= 0)
                {
                    out << (frame_index * OperationBinSize) << "," << (bin_index * IndexBinSize)
                        << "," << count << std::endl;
                }
                bin_index++;
            });
        }

        return out.str();
    }

  private:
    void count(size_type pos) const
    {
        std::lock_guard<std::mutex> guard(frames_lock);
        auto frame_index = detail::operation++ / OperationBinSize;

        while (frame_offsets.size() <= frame_index)
        {
            frame_offsets.push_back(frame_counters.size());
        }

        auto frame_offset = frame_offsets.back();
        auto counter_index = frame_offset + pos / IndexBinSize;

        if (counter_index >= frame_counters.size())
        {
            auto old_size = frame_counters.size();
            frame_counters.resize(counter_index + 1);
            std::fill(frame_counters.begin() + old_size, frame_counters.end(), 0);
        }

        frame_counters[counter_index]++;
    }

    mutable std::mutex frames_lock;
    mutable std::vector<std::uint32_t> frame_offsets;
    mutable std::vector<std::uint32_t> frame_counters;
};
}
}

#endif

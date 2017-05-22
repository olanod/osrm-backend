#ifndef OSRM_PARTITION_RENUMBER_HPP
#define OSRM_PARTITION_RENUMBER_HPP

#include "partition/bisection_to_partition.hpp"
#include "partition/edge_based_graph.hpp"

namespace osrm
{
namespace partition
{
std::vector<std::uint32_t> makePermutation(const DynamicEdgeBasedGraph &graph, const std::vector<Partition> &partitions);

inline void renumber(DynamicEdgeBasedGraph& graph, const std::vector<std::uint32_t> &permutation)
{
    // Graph has own specilization
    graph.Renumber(permutation);
}

inline void renumber(std::vector<Partition> &partitions, const std::vector<std::uint32_t> &permutation)
{
    for (auto &partition : partitions)
    {
        util::inplacePermutation(partition.begin(), partition.end(), permutation);
    }
}

}
}

#endif

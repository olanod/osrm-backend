#ifndef OSRM_PARTITION_RENUMBER_HPP
#define OSRM_PARTITION_RENUMBER_HPP

#include "extractor/edge_based_node_segment.hpp"

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

inline void renumber(util::vector_view<extractor::EdgeBasedNodeSegment> &segments, const std::vector<std::uint32_t> &permutation)
{
    for (auto &segment : segments)
    {
        segment.forward_segment_id.id = permutation[segment.forward_segment_id.id];
        segment.reverse_segment_id.id = permutation[segment.reverse_segment_id.id];
    }
}

}
}

#endif

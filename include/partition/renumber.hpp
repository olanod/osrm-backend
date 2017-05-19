#ifndef OSRM_PARTITION_RENUMBER_HPP
#define OSRM_PARTITION_RENUMBER_HPP

#include "partition/bisection_to_partition.hpp"
#include "partition/edge_based_graph.hpp"

namespace osrm
{
namespace partition
{
std::vector<std::uint32_t> makePermutation(const DynamicEdgeBasedGraph &graph, const std::vector<Partition> &partitions);

void renumber(std::vector<std::uint32_t> &permutation, DynamicEdgeBasedGraph &graph);

}
}

#endif

#include "partition/renumber.hpp"

#include <tbb/parallel_sort.h>

namespace osrm
{
namespace partition
{
namespace
{
// Returns a vector that is indexed by node ID marking all border edges of graph
std::vector<bool> markBorderNodes(const DynamicEdgeBasedGraph &graph,
                                  const std::vector<Partition> &partitions)
{
    std::vector<bool> is_border_nodes(graph.GetNumberOfNodes(), false);

    // Since this is a multi-level partition every node that is a border node
    // on a higher level, is also a border node on all levels below.
    const auto first_level_partition = partitions.front();

    for (auto node : util::irange<NodeID>(0, graph.GetNumberOfNodes()))
    {
        for (auto edge : graph.GetAdjacentEdgeRange(node))
        {
            auto target = graph.GetTarget(edge);
            if (first_level_partition[node] != first_level_partition[target])
            {
                is_border_nodes[node] = true;
                is_border_nodes[target] = true;
            }
        }
    }

    return is_border_nodes;
}
}

std::vector<std::uint32_t> computePartitionPermutation(const DynamicEdgeBasedGraph &graph,
                                                       const std::vector<Partition> &partitions)
{
    std::vector<std::uint32_t> permutation(graph.GetNumberOfNodes());
    std::iota(permutation.begin(), permutation.end(), 0);

    for (const auto &partition : partitions)
    {
        std::stable_sort(
            permutation.begin(), permutation.end(), [&partition](const auto lhs, const auto rhs) {
                return partition[lhs] < partition[rhs];
            });
    }

    auto is_border_nodes = markBorderNodes(graph, partitions);
    std::stable_partition(permutation.begin(),
                          permutation.end(),
                          [&is_border_nodes](const auto node) { return is_border_nodes[node]; });

    return permutation;
}
}
}

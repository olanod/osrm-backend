#include "partition/renumber.hpp"

#include <tbb/parallel_sort.h>

namespace osrm
{
namespace partition
{
namespace
{
// Returns a vector that is indexed by node ID marking the level at which it is a border node
std::vector<LevelID> getHighestBorderLevel(const DynamicEdgeBasedGraph &graph,
                                           const std::vector<Partition> &partitions)
{
    std::vector<LevelID> border_level(graph.GetNumberOfNodes(), 0);

    for (const auto level : util::irange<LevelID>(1, partitions.size() + 1))
    {
        const auto &partition = partitions[level - 1];
        for (auto node : util::irange<NodeID>(0, graph.GetNumberOfNodes()))
        {
            for (auto edge : graph.GetAdjacentEdgeRange(node))
            {
                auto target = graph.GetTarget(edge);
                if (partition[node] != partition[target])
                {
                    border_level[node] = level;
                    border_level[target] = level;
                }
            }
        }
    }

    return border_level;
}
}

std::vector<std::uint32_t> makePermutation(const DynamicEdgeBasedGraph &graph,
                                           const std::vector<Partition> &partitions)
{
    std::vector<std::uint32_t> ordering(graph.GetNumberOfNodes());
    std::iota(ordering.begin(), ordering.end(), 0);

    for (const auto &partition : partitions)
    {
        std::stable_sort(
            ordering.begin(), ordering.end(), [&partition](const auto lhs, const auto rhs) {
                return partition[lhs] < partition[rhs];
            });
    }

    auto border_level = getHighestBorderLevel(graph, partitions);
    std::stable_sort(
        ordering.begin(), ordering.end(), [&border_level](const auto lhs, const auto rhs) {
            return border_level[lhs] > border_level[rhs];
        });

    std::vector<std::uint32_t> permutation(ordering.size());
    for (auto index : util::irange<std::uint32_t>(0, ordering.size()))
        permutation[ordering[index]] = index;

    return permutation;
}
}
}

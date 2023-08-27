// Author : snowapril

#ifndef VOXEL_FLOW_HASH_UTIL_HPP
#define VOXEL_FLOW_HASH_UTIL_HPP

#include <functional>
#include <cstdint>

namespace VoxFlow
{

template <class Type>
inline void hash_combine(uint32_t& seed, const Type& v)
{
    std::hash<Type> hasher;
    seed ^= static_cast<uint32_t>(hasher(v)) + 0x9e3779b9 + (seed << 6) +
            (seed >> 2);
}

}  // namespace VoxFlow

#endif
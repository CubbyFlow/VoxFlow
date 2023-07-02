// Author : snowapril

#ifndef VOXEL_FLOW_BITWISE_OPERATORS_HPP
#define VOXEL_FLOW_BITWISE_OPERATORS_HPP

namespace VoxFlow
{
#define IMPL_BITWISE_OPERATORS(enumType, enumBase)          \
    inline enumType operator|(enumType lhs, enumType rhs)   \
    {                                                       \
        return (enumType)((enumBase)lhs | (enumBase)rhs);   \
    }                                                       \
    inline enumType operator&(enumType lhs, enumType rhs)   \
    {                                                       \
        return (enumType)((enumBase)lhs & (enumBase)rhs);   \
    }                                                       \
    inline enumType operator^(enumType lhs, enumType rhs)   \
    {                                                       \
        return (enumType)((enumBase)lhs ^ (enumBase)rhs);   \
    }                                                       \
    inline enumType operator|=(enumType& lhs, enumType rhs) \
    {                                                       \
        lhs = (enumType)((enumBase)lhs | (enumBase)rhs);    \
        return lhs;                                         \
    }                                                       \
    inline enumType operator&=(enumType& lhs, enumType rhs) \
    {                                                       \
        lhs = (enumType)((enumBase)lhs & (enumBase)rhs);    \
        return lhs;                                         \
    }                                                       \
    inline enumType operator^=(enumType& lhs, enumType rhs) \
    {                                                       \
        lhs = (enumType)((enumBase)lhs ^ (enumBase)rhs);    \
        return lhs;                                         \
    }

}  // namespace VoxFlow

#endif
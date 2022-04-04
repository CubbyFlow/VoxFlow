// Author : snowapril

#ifndef VOXEL_FLOW_INSTANCE_HPP
#define VOXEL_FLOW_INSTANCE_HPP

#include <VoxFlow/Core/Utils/pch.hpp>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
    class Instance : NonCopyable
    {
     public:
        Instance(const Context& ctx);
        ~Instance() override;
        Instance(Instance&& instance) noexcept;
        Instance& operator=(Instance&& instance) noexcept;

        VkInstance get() const
        {
            return _instance;
        }
    protected:
        void release() const;

    private:
        VkInstance _instance{ VK_NULL_HANDLE };
    };
}

#endif
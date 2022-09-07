#include <vulkan/vulkan.h>

#include <unordered_set>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

struct VulkanQueueFamilyIndices {
  std::optional<uint32_t> compute_family;
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool is_complete() const {
    return compute_family.has_value();
  }

  bool is_complete_for_ui() {
    return graphics_family.has_value() && present_family.has_value();
  }
};

VulkanQueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
  VulkanQueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());

  // TODO: What the heck is this?
  constexpr VkQueueFlags kFlagMask =
      (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT));

  // first try and find a queue that has just the compute bit set
  for (int i = 0; i < (int)queue_family_count; ++i) {
    const VkQueueFlags masked_flags = kFlagMask & queue_families[i].queueFlags;
    if ((masked_flags & VK_QUEUE_COMPUTE_BIT) &&
        !(masked_flags & VK_QUEUE_GRAPHICS_BIT)) {
      indices.compute_family = i;
    }
    if (masked_flags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }

    if (indices.is_complete() && indices.is_complete_for_ui()) {
      return indices;
    }
  }

  // lastly get any queue that will work
  for (int i = 0; i < (int)queue_family_count; ++i) {
    const VkQueueFlags masked_flags = kFlagMask & queue_families[i].queueFlags;
    if (masked_flags & VK_QUEUE_COMPUTE_BIT) {
      indices.compute_family = i;
    }
    if (indices.is_complete()) {
      return indices;
    }
  }
  return indices;
}

class VulkanProgram {
public:
    
    void cleanup() {
        vkDestroyDevice(device_, nullptr);
        vkDestroyInstance(instance_, nullptr);
    }
    
    void create_vulkan_instance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }
   
    void pick_physical_device() {
        uint32_t physDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance_, &physDeviceCount, nullptr);

        if (physDeviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> phys_devices(physDeviceCount);
        vkEnumeratePhysicalDevices(instance_, &physDeviceCount, phys_devices.data());

        for (const auto& phys_device : phys_devices) {
            physical_device_ = phys_device;
            break;
        }

        if (physical_device_ == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        queue_family_indices_ = find_queue_families(physical_device_);
    }

    void create_logical_device() {
      std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
      std::unordered_set<uint32_t> unique_families;

      if (queue_family_indices_.compute_family.has_value()) {
        unique_families.insert(queue_family_indices_.compute_family.value());
      }
      if (queue_family_indices_.graphics_family.has_value()) {
        unique_families.insert(queue_family_indices_.graphics_family.value());
      }

      float queue_priority = 1.0f;
      for (uint32_t queue_family : unique_families) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queue_family;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queueCreateInfo);
      }

      VkDeviceCreateInfo create_info{};
      create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      create_info.pQueueCreateInfos = queue_create_infos.data();
      create_info.queueCreateInfoCount = queue_create_infos.size();

      create_info.enabledExtensionCount = 0;
      create_info.enabledLayerCount = 0;
      
      if(vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
      }

      if (queue_family_indices_.compute_family.has_value()) {
        vkGetDeviceQueue(device_, queue_family_indices_.compute_family.value(), 0,
                     &compute_queue_);
      }
      if (queue_family_indices_.graphics_family.has_value()) {
        vkGetDeviceQueue(device_, queue_family_indices_.graphics_family.value(), 0,
                     &graphics_queue_);
      }
    }

    void create_command_pool() {
        // Command Pool
        VkCommandPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = queue_family_indices_.compute_family.value();
        vkCreateCommandPool(device_, &info, nullptr, &pool_);
    }
    
    void create_command_buffer() {
        // Command Buffer
        VkCommandBufferAllocateInfo info{};
        
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;

        info.commandPool = pool_;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        
        // vkDevice
        vkAllocateCommandBuffers(device_, &info, &cmd_buffer_);
    }

    void create_query_pool() {
      VkQueryPoolCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
      info.pNext = nullptr;
      info.queryCount = 2;
      info.queryType = VK_QUERY_TYPE_TIMESTAMP;

      vkCreateQueryPool(device_, &info, nullptr, &query_pool_);
    }

    void record_commands() {
      VkCommandBufferBeginInfo info{};
      info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      info.pNext = nullptr;
      info.pInheritanceInfo = nullptr;
      info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
      
      vkBeginCommandBuffer(cmd_buffer_, &info);
      //vkCmdResetQueryPool(cmd_buffer_, query_pool_, 0, 2);
      //vkCmdWriteTimestamp(cmd_buffer_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, query_pool_, 0);

      //vkCmdWriteTimestamp(cmd_buffer_, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,query_pool_, 1);
      vkEndCommandBuffer(cmd_buffer_);
    }

    void submit_commands() {
      VkSubmitInfo submit_info{};
      submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submit_info.commandBufferCount = 1;
      submit_info.pCommandBuffers = &cmd_buffer_;

      vkQueueSubmit(compute_queue_, /*submitCount=*/1, &submit_info, nullptr);
      vkQueueWaitIdle(compute_queue_);
    }

private:
    // Instance
    VkInstance instance_;

    // Physical Device
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VulkanQueueFamilyIndices queue_family_indices_;
    
    // Logical Device
    VkQueue graphics_queue_;
    VkQueue compute_queue_;
    VkDevice device_;

    // Commands
    VkCommandPool pool_;
    VkCommandBuffer cmd_buffer_;

    // Query Pool
    VkQueryPool query_pool_;

};

int main() {
    VulkanProgram prog;
    prog.create_vulkan_instance();
    prog.pick_physical_device();
    prog.create_logical_device();
    
    prog.create_command_pool();
    prog.create_command_buffer();
    
    prog.create_query_pool();
    prog.record_commands();
    prog.submit_commands();

    prog.cleanup();
    
    std::cout << "Success!!" << std::endl;

    return 0;
}

#include "Queue.h"

#include "CommandBuffer.h"
#include "Device.h"
#include "Fence.h"
#include "Frame.h"
#include "Semaphore.h"
#include "Swapchain.h"

VkPresentInfoKHR Queue::getPresentInfo(const Swapchain& swapchain, uint32_t imageIndex,
  const Semaphore& wait) const {
  VkPresentInfoKHR info {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  //info.pNext = nullptr;

  info.pWaitSemaphores = &wait.handle;
  info.waitSemaphoreCount = 1;

  info.pSwapchains = &swapchain.handle;
  info.swapchainCount = 1;

  info.pImageIndices = new uint32_t(imageIndex);

  //info.pResults = nullptr;

  return info;
}

Queue::Queue(Device* device, uint32_t familyIndex) {
  this->familyIndex = familyIndex;
  vkGetDeviceQueue(device->handle, familyIndex, 0, &handle);
}

void Queue::present(const Swapchain& swapchain, uint32_t imageIndex, const Semaphore& wait) const {
  VkPresentInfoKHR presentInfo = getPresentInfo(swapchain, imageIndex, wait);
  VK_CHECK(vkQueuePresentKHR(handle, &presentInfo));
}

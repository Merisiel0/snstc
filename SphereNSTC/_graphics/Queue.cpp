#include "Queue.h"

#include "Device.h"
#include "Frame.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Fence.h"
#include "Swapchain.h"

Queue::Queue(Device* device, uint32_t familyIndex) {
  this->familyIndex = familyIndex;
  vkGetDeviceQueue(device->handle, familyIndex, 0, &handle);
}

void Queue::present(Swapchain* swapchain, uint32_t imageIndex, Semaphore* wait) const {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;

  presentInfo.pWaitSemaphores = &wait->handle;
  presentInfo.waitSemaphoreCount = 1;

  presentInfo.pSwapchains = &swapchain->handle;
  presentInfo.swapchainCount = 1;

  presentInfo.pImageIndices = &imageIndex;

  presentInfo.pResults = nullptr;

  VK_CHECK(vkQueuePresentKHR(handle, &presentInfo));
}

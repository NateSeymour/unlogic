#include "VulkanRenderer.h"
#include "VulkanWindow.h"

using namespace ui;

void VulkanRenderer::initResources()
{
    this->device_functions_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());
}

void VulkanRenderer::startNextFrame()
{
    if(!this->window_->scene)
    {
        this->window_->frameReady();
        return;
    }

    unlogic::Color background = this->window_->scene->background;

    VkClearColorValue clearColor = {
        background.r,
        background.g,
        background.b,
        background.a,
    };
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = this->window_->defaultRenderPass();
    rpBeginInfo.framebuffer = this->window_->currentFramebuffer();
    const QSize sz = this->window_->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuf = this->window_->currentCommandBuffer();
    this->device_functions_->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Do nothing else. We will just clear to green, changing the component on
    // every invocation. This also helps verifying the rate to which the thread
    // is throttled to. (The elapsed time between startNextFrame calls should
    // typically be around 16 ms. Note that rendering is 2 frames ahead of what
    // is displayed.)

    this->device_functions_->vkCmdEndRenderPass(cmdBuf);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}

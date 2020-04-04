// Copyright 2014 GitHub, Inc.
// Copyright 2017 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the MIT license.

#ifndef SRC_NODE_INTEGRATION_H_
#define SRC_NODE_INTEGRATION_H_

#include <functional>

#include "node/deps/uv/include/uv.h"
#include "node/src/node.h"
#include "node/src/util.h"

namespace yode {

class NodeIntegration {
 public:
  static NodeIntegration* Create();

  virtual ~NodeIntegration();

  // Prepare for message loop integration.
  void Init();

  // Run the libuv loop for once.
  void UvRunOnce();

  // Handle the nextTick callbacks.
  void CallNextTick();

  // Release our ref to uv handle, so active handles count become 0.
  void ReleaseHandleRef();

 protected:
  NodeIntegration();

  // Called to poll events in new thread.
  virtual void PollEvents() = 0;

  // Called to post a task to the main thread, must be thread-safe.
  virtual void PostTask(const std::function<void()>& task) = 0;

  // Make the main thread run libuv loop.
  void WakeupMainThread();

  // Interrupt the PollEvents.
  void WakeupEmbedThread();

  // Main thread's libuv loop.
  uv_loop_t* uv_loop_;

 private:
  NodeIntegration(const NodeIntegration&) = delete;
  NodeIntegration& operator=(const NodeIntegration&) = delete;

  // Thread to poll uv events.
  static void EmbedThreadRunner(void* arg);

  // Handle nextTick callbacks.
  static void OnCallNextTick(uv_async_t* handle);

  // Whether the libuv loop has ended.
  bool embed_closed_;

  // Handle to wake up uv's loop.
  uv_async_t wakeup_handle_;

  // Handle to call nextTick callbacks.
  uv_async_t next_tick_handle_;

  // Thread for polling events.
  uv_thread_t embed_thread_;

  // Semaphore to wait for main loop in the embed thread.
  uv_sem_t embed_sem_;
};

}  // namespace yode

#endif  // SRC_NODE_INTEGRATION_H_

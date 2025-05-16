/*
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2022 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { worker, MessageEvents, ErrorEvent } from '@kit.ArkTS';
import libHippy from 'libhippy.so';
import { HippyWModuleContext } from '../HippyWModuleContext';
import { HippyWModuleManagerImpl } from '../HippyWModuleManagerImpl';

let ctx = new HippyWModuleContext();
let manager = new HippyWModuleManagerImpl(ctx, false);
libHippy.RegisterWModules(manager, manager.getModuleNames());

// 创建worker线程中与宿主线程通信的对象
const workerPort = worker.workerPort

// worker线程接收宿主线程信息
workerPort.onmessage = (e: MessageEvents): void => {
  // data：宿主线程发送的信息
  let data: number = e.data;
  // 往收到的buffer里写入数据
  const view = new Int8Array(data).fill(3);
  // worker线程向宿主线程发送信息
  workerPort.postMessage(view);
}

// worker线程发生error的回调
workerPort.onerror = (err: ErrorEvent) => {
  console.log("HippyModuleWorker.ets onerror" + err.message);
}

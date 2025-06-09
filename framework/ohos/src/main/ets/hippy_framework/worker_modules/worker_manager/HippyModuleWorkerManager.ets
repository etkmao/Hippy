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
import { ErrorEvent, MessageEvents, worker } from "@kit.ArkTS";
import { LogUtils } from "../../../support/utils/LogUtils";

// 模块Worker由多个HippyEngine共享，因为：
// 1 为了节省线程资源，本身Worker里模块功能独立，没必要存在多份；
// 2 js调用时，从name到模块的映射不区分HippyEngine。
export class HippyModuleWorkerManager {
  private static mRefCounter: number = 0;
  private static mModuleThreadWorker: worker.ThreadWorker | null = null;

  public static useNativeModuleWorker() {
    if (HippyModuleWorkerManager.mRefCounter == 0) {
      HippyModuleWorkerManager.startNativeModuleWorker();
    }
    ++HippyModuleWorkerManager.mRefCounter;
  }

  public static releaseNativeModuleWorker() {
    if (HippyModuleWorkerManager.mRefCounter > 0) {
      --HippyModuleWorkerManager.mRefCounter;
      if (HippyModuleWorkerManager.mRefCounter == 0) {
        HippyModuleWorkerManager.stopNativeModuleWorker();
      }
    }
  }

  private static startNativeModuleWorker() {
    HippyModuleWorkerManager.mModuleThreadWorker = new worker.ThreadWorker("@hippy/ets/hippy_framework/worker_modules/worker/HippyModuleWorker.ets");
    if (!HippyModuleWorkerManager.mModuleThreadWorker) {
      LogUtils.e('hippy', 'create module thread worker failed.');
      return;
    }
    HippyModuleWorkerManager.mModuleThreadWorker.onexit = (code) => {
      LogUtils.w('hippy', 'module thread worker terminated: ' + code);
    }
    HippyModuleWorkerManager.mModuleThreadWorker.onerror = (err: ErrorEvent) => {
      LogUtils.e('hippy', 'module thread worker error: ' + err.message);
    }
    HippyModuleWorkerManager.mModuleThreadWorker.onmessage = (e: MessageEvents): void => {
      // data：worker线程发送的信息
      let data: string = e.data;
      if (data == 'WorkerReady') {
        LogUtils.i('hippy', 'module thread worker is ready.');
        // TODO(hot):
      }
    }
  }

  private static stopNativeModuleWorker() {
    if (HippyModuleWorkerManager.mModuleThreadWorker) {
      HippyModuleWorkerManager.mModuleThreadWorker.postMessage('DoExit');
      HippyModuleWorkerManager.mModuleThreadWorker.terminate();
    }
  }

}

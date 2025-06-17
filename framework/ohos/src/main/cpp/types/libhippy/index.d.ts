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
import resmgr from "@ohos.resourceManager";
import { FrameNode } from '@kit.ArkUI';
import { NodeContent } from '@ohos.arkui.node';

export interface HippyRenderViewInfo {
  tag: number
  viewName: string
}

export type CallResultCallback = (result: HippyAny) => void
export type EventListenerCallback = (event: string, params: object) => void

export const Devtools_OnCreateDevtools: (dataDir: string, wsUrl: string) => number;
export const Devtools_OnDestroyDevtools: (devtoolsId: number, isReload: boolean) => void;
export const Devtools_OnBindDevtools: (devtoolsId: number, driverId: number, domId: number, renderId: number) => void;
export const Devtools_OnAttachToRoot: (devtoolsId: number, rootId: number) => void;

export const DomManager_SetRenderManager: (instanceId: number, renderId: number) => void;
export const DomManager_CreateDomManager: (domManagerNum: number) => number;
export const DomManager_DestroyDomManager: (domManagerId: number) => void;
export const DomManager_CreateRoot: (rootId: number, density: number, layoutEngineType: HippyLayoutEngineType) => void;
export const DomManager_DestroyRoot: (rootId: number) => void;
export const DomManager_ReleaseRootResources: (rootId: number) => void;
export const DomManager_SetDomManager: (rootId: number, instanceId: number) => void;

export const JsDriver_CreateJsDriver: (
  jsDriver: JsDriver,
globalConfig: string,
useLowMemoryMode: boolean,
enableV8Serialization: boolean,
isDevModule: boolean,
callback: NativeCallback,
groupId: number,
domManagerId: number,
v8ParamArray: Array<number> | null,
vfsId: number,
devtoolsId: number,
isReload: boolean
) => number;
export const JsDriver_DestroyJsDriver: (instanceId: number, useLowMemoryMode: boolean, isReload: boolean, callback: NativeCallback) => void;
export const JsDriver_LoadInstance: (instanceId: number, buffer: ArrayBuffer) => void;
export const JsDriver_UnloadInstance: (instanceId: number,buffer: ArrayBuffer) => void;
export const JsDriver_RunScriptFromUri: (
  instanceId: number,
uri: string,
isRawfile: boolean,
assetManager: resmgr.ResourceManager,
resModuleName: string,
canUseCodeCache: boolean,
codeCacheDir: string,
vfsId: number,
callback: NativeCallback
) => boolean;
export const JsDriver_SetRootNode: (instanceId: number, rootId: number) => void;
export const JsDriver_OnNativeInitEnd: (instanceId: number, startTime: bigint, endTime: bigint) => void;
export const JsDriver_OnResourceLoadEnd: (
  instanceId: number,
uri: string,
startTime: bigint,
endTime: bigint,
retCode: number,
errorMsg: string) => void;
export const JsDriver_CallFunction: (instanceId: number, action: string, callback: NativeCallback, buffer: ArrayBuffer) => void;

export const NativeRenderer_CreateNativeRenderManager: (enableArkCApi: boolean, renderProvider: NativeRenderProvider,
customRenderViews: Array<string> | null, customMeasureRenderViews: Array<string> | null, mappingRenderViews: Array<string> | null,
bundlePath: string, density: number, densityScale: number, fontSizeScale: number, fontWeightScale: number,
isRawfile: boolean, resModuleName: string) => number;
export const NativeRenderer_DestroyNativeRenderManager: (instanceId: number) => void;
export const NativeRenderer_SetBundlePath: (instanceId: number, bundlePath: string) => void;
export const NativeRenderer_InitRendererParams: (statusBarHeight: number) => void;
export const NativeRenderer_RegisterFontPaths: (instanceId: number, fontFamilyAndPaths: Array<string>) => void;
export const NativeRenderer_KeyboardHeightChange: (height: number) => void;

export const NativeRenderProvider_UpdateRootSize: (instanceId: number, rootId: number, width: number, height: number) => void;
export const NativeRenderProvider_UpdateNodeSize: (instanceId: number, rootId: number, nodeId: number, width: number, height: number, isSync: boolean) => void;
export const NativeRenderProvider_OnReceivedEvent: (instanceId: number, rootId: number, nodeId: number, eventName: string, buffer: ArrayBuffer | null, useCapture: boolean, useBubble: boolean) => void;
export const NativeRenderProvider_DoCallBack: (instanceId: number, result: number, functionName: string, rootId: number, nodeId: number, callbackId: number, buffer: ArrayBuffer | null) => void;
export const NativeRenderProvider_DoMeasureText: (instanceId: number, props:string[] | null, width: number, widthMode: number, height: number, heightMode: number) => number[];

export const NativeRenderProvider_BindNativeRoot: (content: NodeContent, instanceId: number, rootId: number, nodeId: number) => void;
export const NativeRenderProvider_UnbindNativeRoot: (instanceId: number, rootId: number, nodeId: number) => void;
export const NativeRenderProvider_DestroyRoot: (instanceId: number, rootId: number) => void;
export const NativeRenderProvider_DoCallbackForCallCustomTsView: (instanceId: number, rootId: number, nodeId: number, callbackId: number, result: HippyAny) => void;
export const NativeRenderProvider_GetViewParent: (instanceId: number, rootId: number, nodeId: number) => HippyRenderViewInfo | null;
export const NativeRenderProvider_GetViewChildren: (instanceId: number, rootId: number, nodeId: number) => Array<HippyRenderViewInfo> | null;
export const NativeRenderProvider_CallViewMethod: (instanceId: number, rootId: number, nodeId: number, method: string, params: HippyArray, callback: CallResultCallback | null) => void;
export const NativeRenderProvider_SetViewEventListener: (instanceId: number, rootId: number, nodeId: number, callback: EventListenerCallback | null) => void;
export const NativeRenderProvider_GetViewFrameInRoot: (instanceId: number, rootId: number, nodeId: number) => HRRect | null;
export const NativeRenderProvider_AddBizViewInRoot: (instanceId: number, rootId: number, biz_view_id: number, frameNode: FrameNode, x: number, y: number) => void;
export const NativeRenderProvider_RemoveBizViewInRoot: (instanceId: number, rootId: number, biz_view_id: number) => void;

export const VfsManager_CreateVfs: (vfsDelegate: VfsManager) => number;
export const VfsManager_DestroyVfs: (instanceId: number) => void;
export const VfsManager_OnJniDelegateCallback: (holder: ResourceDataHolder) => void;
export const VfsManager_OnJniDelegateInvokeProgress: (callbackId: number, total: number, loaded: number) => void;
export const VfsManager_OnJniDelegateInvokeAsync: (id: number, holder: ResourceDataHolder, callback: FetchResourceCallback) => void;
export const VfsManager_OnJniDelegateInvokeSync: (id: number, holder: ResourceDataHolder) => void;

export const TurboModuleManager_Install: (turbo: TurboModuleManager, v8RuntimeId: number) => void;

export const Setting_SetFlags: (enableUpdateAnimLog: boolean) => void;
export const Setting_SetDensity: (density: number) => void;
export const Setting_SetDensityScale: (densityScale: number) => void;
export const Setting_SetFontSizeScale: (fontSizeScale: number) => void;
export const Setting_SetFontWeightScale: (fontWeightScale: number) => void;

// main线程使用
export const WorkerManager_InitWModuleManager: (notifyMainThreadFunction: (
  workerName: string) => void) => void;

// worker线程使用
export const Worker_RegisterWModules: (workerName: string, callNativesFunction: (
  scopeId: number,
  moduleName: string,
  moduleFunc: string,
  callId: string,
  buffer: ArrayBuffer) => void, moduleNames: HippyArray) => void;
export const Worker_UnregisterWModules: (moduleNames: HippyArray) => void;
export const Worker_CallJsFunction: (instanceId: number, action: string, buffer: ArrayBuffer) => void;

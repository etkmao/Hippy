/* Tencent is pleased to support the open source community by making Hippy available.
 * Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.tencent.renderer;

import static com.tencent.mtt.hippy.dom.node.NodeProps.PADDING_BOTTOM;
import static com.tencent.mtt.hippy.dom.node.NodeProps.PADDING_LEFT;
import static com.tencent.mtt.hippy.dom.node.NodeProps.PADDING_RIGHT;
import static com.tencent.mtt.hippy.dom.node.NodeProps.PADDING_TOP;
import static com.tencent.renderer.NativeRenderException.ExceptionCode.UI_TASK_QUEUE_ADD_ERR;
import static com.tencent.renderer.NativeRenderException.ExceptionCode.INVALID_NODE_DATA_ERR;
import static com.tencent.renderer.NativeRenderException.ExceptionCode.UI_TASK_QUEUE_UNAVAILABLE_ERR;

import android.content.Context;
import android.graphics.Rect;
import android.text.Layout;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.MainThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.tencent.mtt.hippy.common.Callback;
import com.tencent.mtt.hippy.serialization.nio.reader.BinaryReader;
import com.tencent.mtt.hippy.serialization.nio.reader.SafeHeapReader;
import com.tencent.mtt.hippy.serialization.nio.writer.SafeHeapWriter;
import com.tencent.mtt.hippy.serialization.string.InternalizedStringTable;
import com.tencent.mtt.hippy.utils.PixelUtil;
import com.tencent.mtt.hippy.utils.UIThreadUtils;
import com.tencent.mtt.hippy.views.image.HippyImageViewController;
import com.tencent.mtt.hippy.views.text.HippyTextViewController;
import com.tencent.renderer.component.image.ImageLoader;
import com.tencent.renderer.component.image.ImageLoaderAdapter;
import com.tencent.renderer.component.text.FontAdapter;
import com.tencent.renderer.component.text.TextRenderSupplier;
import com.tencent.renderer.node.ListItemRenderNode;
import com.tencent.renderer.node.RenderNode;
import com.tencent.renderer.node.RootRenderNode;
import com.tencent.renderer.node.TextRenderNode;
import com.tencent.renderer.node.VirtualNode;
import com.tencent.renderer.node.VirtualNodeManager;

import com.tencent.renderer.serialization.Deserializer;
import com.tencent.renderer.serialization.Serializer;
import com.tencent.renderer.utils.DisplayUtils;
import com.tencent.renderer.utils.EventUtils.EventType;

import com.tencent.vfs.VfsManager;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.Map.Entry;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;

import com.tencent.mtt.hippy.utils.LogUtils;
import com.tencent.mtt.hippy.HippyInstanceLifecycleEventListener;
import com.tencent.mtt.hippy.HippyRootView;
import com.tencent.mtt.hippy.uimanager.RenderManager;
import com.tencent.renderer.utils.FlexUtils;
import com.tencent.renderer.utils.FlexUtils.FlexMeasureMode;
import java.util.concurrent.atomic.AtomicInteger;

public class NativeRenderer extends Renderer implements NativeRender, NativeRenderProxy,
        NativeRenderDelegate {

    /** This specific ID is used to identify the root view of snapshot restore */
    public static final int SCREEN_SNAPSHOT_ROOT_ID = 1000;
    private static final String TAG = "NativeRenderer";
    private static final String NODE_ID = "id";
    private static final String NODE_PID = "pId";
    private static final String NODE_INDEX = "index";
    private static final String NODE_PROPS = "props";
    private static final String CLASS_NAME = "name";
    private static final String LAYOUT_LEFT = "left";
    private static final String LAYOUT_TOP = "top";
    private static final String LAYOUT_WIDTH = "width";
    private static final String LAYOUT_HEIGHT = "height";
    private static final String EVENT_PREFIX = "on";
    private static final String SNAPSHOT_CREATE_NODE = "createNode";
    private static final String SNAPSHOT_UPDATE_LAYOUT = "updateLayout";
    /** The max capacity of UI task queue */
    private static final int MAX_UI_TASK_QUEUE_CAPACITY = 1000;
    private static final int ROOT_VIEW_ID_INCREMENT = 10;
    private static final AtomicInteger sRootIdCounter = new AtomicInteger(0);
    @Nullable
    private FrameworkProxy mFrameworkProxy;
    @Nullable
    private List<HippyInstanceLifecycleEventListener> mInstanceLifecycleEventListeners;
    @NonNull
    private final NativeRenderProvider mRenderProvider;
    @NonNull
    private final BlockingQueue<UITaskExecutor> mUITaskQueue;
    @NonNull
    private final RenderManager mRenderManager;
    @NonNull
    private final VirtualNodeManager mVirtualNodeManager;
    @Nullable
    private ExecutorService mBackgroundExecutor;
    @Nullable
    private ImageLoaderAdapter mImageLoader;

    public NativeRenderer() {
        mRenderProvider = new NativeRenderProvider(this);
        // Should restrictions the capacity of ui task queue, to avoid js make huge amount of
        // node operation cause OOM.
        mUITaskQueue = new LinkedBlockingQueue<>(MAX_UI_TASK_QUEUE_CAPACITY);
        mRenderManager = new RenderManager(this);
        mVirtualNodeManager = new VirtualNodeManager(this);
    }

    public float getDensity() {
        return PixelUtil.getDensity();
    }

    public NativeRenderProvider getRenderProvider() {
        return mRenderProvider;
    }

    public void setId(int instanceId) {
        mRenderProvider.setInstanceId(instanceId);
    }

    @Override
    public void init(@Nullable List<Class<?>> controllers, @Nullable ViewGroup rootView) {
        mRenderManager.init(controllers);
        if (rootView instanceof HippyRootView) {
            mRenderManager.createRootNode(rootView.getId());
            mRenderManager.addRootView(rootView);
            Context context = rootView.getContext();
            if (context instanceof NativeRenderContext) {
                // Render provider instance id has changed, should reset instance id
                // store in root view context.
                ((NativeRenderContext) context).setInstanceId(mRenderProvider.getInstanceId());
            }
        }
    }

    @Override
    public int getInstanceId() {
        return mRenderProvider.getInstanceId();
    }

    @Override
    @Nullable
    public Object getCustomViewCreator() {
        return (mFrameworkProxy != null) ? mFrameworkProxy.getCustomViewCreator() : null;
    }

    @Override
    @Nullable
    public String getBundlePath() {
        return (mFrameworkProxy != null) ? mFrameworkProxy.getBundlePath() : null;
    }

    @Override
    @Nullable
    public ImageLoaderAdapter getImageLoader() {
        if (mImageLoader == null && getVfsManager() != null) {
            mImageLoader = new ImageLoader(getVfsManager());
        }
        return mImageLoader;
    }

    @Override
    @Nullable
    public VfsManager getVfsManager() {
        return (mFrameworkProxy != null) ? mFrameworkProxy.getVfsManager() : null;
    }

    @Override
    @Nullable
    public FontAdapter getFontAdapter() {
        return (mFrameworkProxy != null) ? mFrameworkProxy.getFontAdapter() : null;
    }

    @Override
    @Nullable
    public Executor getBackgroundExecutor() {
        if (mFrameworkProxy != null && mFrameworkProxy.getBackgroundExecutor() != null) {
            return mFrameworkProxy.getBackgroundExecutor();
        }
        if (mBackgroundExecutor == null) {
            mBackgroundExecutor = Executors.newSingleThreadExecutor();
        }
        return mBackgroundExecutor;
    }

    @MainThread
    @Override
    public void postInvalidateDelayed(int roodId, int id, long delayMilliseconds) {
        if (UIThreadUtils.isOnUiThread()) {
            mRenderManager.postInvalidateDelayed(roodId, id, delayMilliseconds);
        }
    }

    @Override
    public void handleRenderException(@NonNull Exception exception) {
        String msg;
        if (exception instanceof NativeRenderException) {
            msg = "code: " + ((NativeRenderException) exception).mCode + ", message: " + exception
                    .getMessage();
        } else {
            msg = exception.getMessage();
        }
        LogUtils.e(TAG, msg);
        if (mFrameworkProxy != null) {
            mFrameworkProxy.handleNativeException(exception);
        }
    }

    @Override
    public void setFrameworkProxy(@NonNull FrameworkProxy proxy) {
        mFrameworkProxy = proxy;
    }

    @Override
    public void destroy() {
        if (mBackgroundExecutor != null) {
            if (!mBackgroundExecutor.isShutdown()) {
                mBackgroundExecutor.shutdown();
            }
            mBackgroundExecutor = null;
        }
        mRenderProvider.destroy();
        mRenderManager.destroy();
        if (mInstanceLifecycleEventListeners != null) {
            mInstanceLifecycleEventListeners.clear();
        }
        mFrameworkProxy = null;
        NativeRendererManager.removeNativeRendererInstance(mRenderProvider.getInstanceId());
    }

    @Override
    @NonNull
    public View createRootView(@NonNull Context context) {
        int rootId = sRootIdCounter.addAndGet(ROOT_VIEW_ID_INCREMENT);
        HippyRootView rootView = new HippyRootView(context, mRenderProvider.getInstanceId(), rootId);
        mRenderManager.createRootNode(rootId);
        mRenderManager.addRootView(rootView);
        return rootView;
    }

    @Override
    @NonNull
    public RenderManager getRenderManager() {
        return mRenderManager;
    }

    @Override
    @Nullable
    public View getRootView(int rootId) {
        return mRenderManager.getRootView(rootId);
    }

    @Override
    @Nullable
    public View getRootView(@NonNull View view) {
        Context context = view.getContext();
        if (context instanceof NativeRenderContext) {
            int rootId = ((NativeRenderContext) context).getRootId();
            return getRootView(rootId);
        }
        return null;
    }

    @Override
    public void onFirstViewAdded() {
        if (mFrameworkProxy != null) {
            mFrameworkProxy.onFirstViewAdded();
        }
    }

    @Override
    public void onRuntimeInitialized(final int rootId) {
        UIThreadUtils.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                View rootView = getRootView(rootId);
                if (rootView != null) {
                    final int width = rootView.getWidth();
                    final int height = rootView.getHeight();
                    if (width > 0 && height > 0) {
                        onSizeChanged(rootId, width, height);
                    }
                }
            }
        });
    }

    @Override
    public void onSizeChanged(int rootId, int w, int h) {
        mRenderProvider.onSizeChanged(rootId, w, h);
    }

    @Override
    public void onSizeChanged(int rootId, int nodeId, int width, int height, boolean isSync) {
        mRenderProvider.onSizeChanged(rootId, nodeId, width, height, isSync);
    }

    @Override
    public void updateDimension(int width, int height, boolean shouldUseScreenDisplay,
            boolean systemUiVisibilityChanged) {
        if (mFrameworkProxy != null) {
            mFrameworkProxy.updateDimension(width, height, shouldUseScreenDisplay, systemUiVisibilityChanged);
        }
    }

    /**
     * Dispatch UI component event, such as onLayout, onScroll, onInitialListReady.
     *
     * @param rootId root node id
     * @param nodeId target node id
     * @param eventName target event name
     * @param params event extra params object
     * @param eventType event type {@link EventType}
     */
    @Override
    public void dispatchEvent(int rootId, int nodeId, @NonNull String eventName,
            @Nullable Object params, boolean useCapture, boolean useBubble, EventType eventType) {
        // Because the native(C++) DOM use lowercase names, convert to lowercase here before call JNI.
        String lowerCaseEventName = eventName.toLowerCase();
        // Compatible with events prefixed with on in old version
        if (lowerCaseEventName.startsWith(EVENT_PREFIX)) {
            lowerCaseEventName = lowerCaseEventName.substring(EVENT_PREFIX.length());
        }
        if (eventType != EventType.EVENT_TYPE_GESTURE && !mRenderManager.checkRegisteredEvent(
                rootId, nodeId, lowerCaseEventName)) {
            return;
        }
        mRenderProvider.dispatchEvent(rootId, nodeId, lowerCaseEventName, params, useCapture,
                useBubble);
    }

    @Override
    public void onResume() {
        if (mInstanceLifecycleEventListeners != null) {
            for (HippyInstanceLifecycleEventListener listener : mInstanceLifecycleEventListeners) {
                listener.onInstanceResume();
            }
        }
    }

    @Override
    public void onPause() {
        if (mInstanceLifecycleEventListeners != null) {
            for (HippyInstanceLifecycleEventListener listener : mInstanceLifecycleEventListeners) {
                listener.onInstancePause();
            }
        }
    }

    @MainThread
    @Override
    public void destroyRoot(int rootId) {
        if (mInstanceLifecycleEventListeners != null) {
            for (HippyInstanceLifecycleEventListener listener : mInstanceLifecycleEventListeners) {
                listener.onInstanceDestroy(rootId);
            }
        }
        mRenderManager.deleteNode(rootId, rootId);
        mRenderManager.batch(rootId);
    }

    @Override
    public void addInstanceLifecycleEventListener(HippyInstanceLifecycleEventListener listener) {
        if (mInstanceLifecycleEventListeners == null) {
            mInstanceLifecycleEventListeners = new ArrayList<>();
        }
        mInstanceLifecycleEventListeners.add(listener);
    }

    @Override
    public void removeInstanceLifecycleEventListener(HippyInstanceLifecycleEventListener listener) {
        if (mInstanceLifecycleEventListeners != null) {
            mInstanceLifecycleEventListeners.remove(listener);
        }
    }

    private UITaskExecutor getMassTaskExecutor(@NonNull final List<UITaskExecutor> taskList) {
        return new UITaskExecutor() {
            @Override
            public void exec() {
                for (UITaskExecutor task : taskList) {
                    task.exec();
                }
            }
        };
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    @Override
    public void createNode(final int rootId, @NonNull List<Object> nodeList)
            throws NativeRenderException {
        final List<UITaskExecutor> createNodeTaskList = new ArrayList<>(nodeList.size());
        final List<UITaskExecutor> createViewTaskList = new ArrayList<>(nodeList.size());
        for (int i = 0; i < nodeList.size(); i++) {
            Object element = nodeList.get(i);
            if (!(element instanceof Map)) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": createNode: invalid node object");
            }
            final Map node = (Map) element;
            int nodeId;
            int nodePid;
            int nodeIndex;
            String className;
            try {
                nodeId = ((Number) Objects.requireNonNull(node.get(NODE_ID))).intValue();
                nodePid = ((Number) Objects.requireNonNull(node.get(NODE_PID))).intValue();
                nodeIndex = ((Number) Objects.requireNonNull(node.get(NODE_INDEX))).intValue();
                className = (String) Objects.requireNonNull(node.get(CLASS_NAME));
            } catch (NullPointerException e) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR, e);
            }
            // The node id, pid and index should not be negative number.
            if (nodeId < 0 || nodePid < 0 || nodeIndex < 0) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": createNode: id=" + nodeId + ", pId=" + nodePid + ", index="
                                + nodeIndex);
            }
            final int id = nodeId;
            element = node.get(NODE_PROPS);
            final Map<String, Object> props =
                    (element instanceof HashMap) ? (Map) element : new HashMap<String, Object>();
            mVirtualNodeManager.createNode(rootId, nodeId, nodePid, nodeIndex, className, props);
            // If multiple level are nested, the parent is outermost text node.
            VirtualNode parent = mVirtualNodeManager.checkVirtualParent(rootId, nodeId);
            // If restoring snapshots, create node is called directly on the UI thread,
            // and do not need to use the UI task
            if (rootId == SCREEN_SNAPSHOT_ROOT_ID) {
                if (parent == null) {
                    mRenderManager.createNode(rootId, nodeId, nodePid, nodeIndex, className, props);
                }
                continue;
            }
            if (parent != null) {
                final int pid = parent.getId();
                // If the node has a virtual parent, no need to create corresponding render node,
                // but need set the node data to the parent, for render node snapshot.
                createNodeTaskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.onVirtualNodeUpdated(rootId, id, pid, node);
                    }
                });
            } else {
                final int pid = nodePid;
                final int index = nodeIndex;
                final String name = className;
                createNodeTaskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.createNode(rootId, id, pid, index, name, props);
                    }
                });
                // Because image and text may be rendered flat, it is not necessary to pre create a view.
                if (!className.equals(HippyImageViewController.CLASS_NAME) && !className.equals(
                        HippyTextViewController.CLASS_NAME)) {
                    createViewTaskList.add(new UITaskExecutor() {
                        @Override
                        public void exec() {
                            mRenderManager.preCreateView(rootId, id, pid, name, props);
                        }
                    });
                }
            }
        }
        if (!createNodeTaskList.isEmpty()) {
            addUITask(getMassTaskExecutor(createNodeTaskList));
        }
        if (!createViewTaskList.isEmpty()) {
            // The task of creating render nodes will not be executed until batch end,
            // so we can pre create view, reduce render time by creating in parallel.
            final UITaskExecutor task = getMassTaskExecutor(createViewTaskList);
            UIThreadUtils.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    task.exec();
                }
            });
        }
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    @Override
    public void updateNode(final int rootId, @NonNull List<Object> nodeList)
            throws NativeRenderException {
        final List<UITaskExecutor> taskList = new ArrayList<>(nodeList.size());
        for (int i = 0; i < nodeList.size(); i++) {
            Object element = nodeList.get(i);
            if (!(element instanceof Map)) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateNode: invalid node object");
            }
            final Map<String, Object> node = (Map) element;
            int nodeId;
            try {
                nodeId = ((Number) Objects.requireNonNull(node.get(NODE_ID))).intValue();
            } catch (NullPointerException e) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR, e);
            }
            // The node id should not be negative number.
            if (nodeId < 0) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateNode: invalid negative id=" + nodeId);
            }
            final int id = nodeId;
            element = node.get(NODE_PROPS);
            final Map<String, Object> props =
                    (element instanceof HashMap) ? (Map) element : new HashMap<String, Object>();
            mVirtualNodeManager.updateNode(rootId, nodeId, props);
            // If multiple level are nested, the parent is outermost text node.
            VirtualNode parent = mVirtualNodeManager.checkVirtualParent(rootId, nodeId);
            if (parent != null) {
                final int pid = parent.getId();
                taskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.onVirtualNodeUpdated(rootId, id, pid, node);
                    }
                });
            } else {
                taskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.updateNode(rootId, id, props);
                    }
                });
            }
        }
        if (!taskList.isEmpty()) {
            addUITask(getMassTaskExecutor(taskList));
        }
    }

    @Override
    public void deleteNode(final int rootId, @NonNull int[] ids) throws NativeRenderException {
        final List<UITaskExecutor> taskList = new ArrayList<>(ids.length);
        for (final int nodeId : ids) {
            // The node id should not be negative number.
            if (nodeId < 0) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": deleteNode: invalid negative id=" + nodeId);
            }
            // If multiple level are nested, the parent is outermost text node.
            VirtualNode parent = mVirtualNodeManager.checkVirtualParent(rootId, nodeId);
            mVirtualNodeManager.deleteNode(rootId, nodeId);
            if (parent != null) {
                final int pid = parent.getId();
                taskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.onVirtualNodeDeleted(rootId, nodeId, pid);
                    }
                });
            } else {
                taskList.add(new UITaskExecutor() {
                    @Override
                    public void exec() {
                        mRenderManager.deleteNode(rootId, nodeId);
                    }
                });
            }
        }
        if (!taskList.isEmpty()) {
            addUITask(getMassTaskExecutor(taskList));
        }
    }

    @Override
    public void moveNode(final int rootId, final int[] ids, final int newPid, final int oldPid)
            throws NativeRenderException {
        addUITask(new UITaskExecutor() {
            @Override
            public void exec() {
                mRenderManager.moveNode(rootId, ids, newPid, oldPid);
            }
        });
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    @Override
    public void updateLayout(final int rootId, @NonNull List<Object> nodeList)
            throws NativeRenderException {
        final List<UITaskExecutor> taskList = new ArrayList<>(nodeList.size());
        for (int i = 0; i < nodeList.size(); i++) {
            Object element = nodeList.get(i);
            if (!(element instanceof Map)) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateLayout: invalid node object");
            }
            Map<String, Object> layoutInfo = (Map) element;
            int nodeId;
            float layoutLeft;
            float layoutTop;
            float layoutWidth;
            float layoutHeight;
            try {
                nodeId = ((Number) Objects.requireNonNull(layoutInfo.get(NODE_ID))).intValue();
                layoutLeft = ((Number) Objects.requireNonNull(layoutInfo.get(LAYOUT_LEFT)))
                        .floatValue();
                layoutTop = ((Number) Objects.requireNonNull(layoutInfo.get(LAYOUT_TOP)))
                        .floatValue();
                layoutWidth = ((Number) Objects.requireNonNull(layoutInfo.get(LAYOUT_WIDTH)))
                        .floatValue();
                layoutHeight = ((Number) Objects.requireNonNull(layoutInfo.get(LAYOUT_HEIGHT)))
                        .floatValue();
            } catch (NullPointerException e) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR, e);
            }
            // The node id should not be negative number.
            if (nodeId < 0) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateLayout: invalid negative id=" + nodeId);
            }
            if (mVirtualNodeManager.hasVirtualParent(rootId, nodeId)) {
                // If the node has a virtual parent, no corresponding render node exists,
                // so don't add update task to the ui task queue.
                continue;
            }
            final int id = nodeId;
            final int left = Math.round(layoutLeft);
            final int top = Math.round(layoutTop);
            final int width = Math.round(layoutWidth);
            final int height = Math.round(layoutHeight);
            final TextRenderSupplier supplier = mVirtualNodeManager
                    .updateLayout(rootId, nodeId, layoutWidth, layoutInfo);
            // If restoring snapshots, update layout is called directly on the UI thread,
            // and do not need to use the UI task
            if (rootId == SCREEN_SNAPSHOT_ROOT_ID) {
                if (supplier != null) {
                    mRenderManager.updateExtra(rootId, id, supplier);
                }
                mRenderManager.updateLayout(rootId, id, left, top, width, height);
                continue;
            }
            UITaskExecutor task = new UITaskExecutor() {
                @Override
                public void exec() {
                    if (supplier != null) {
                        mRenderManager.updateExtra(rootId, id, supplier);
                    }
                    mRenderManager.updateLayout(rootId, id, left, top, width, height);
                }
            };
            taskList.add(task);
        }
        if (!taskList.isEmpty()) {
            addUITask(getMassTaskExecutor(taskList));
        }
    }

    @SuppressWarnings({"rawtypes", "unchecked"})
    @Override
    public void updateEventListener(final int rootId, @NonNull List<Object> eventList)
            throws NativeRenderException {
        final List<UITaskExecutor> taskList = new ArrayList<>(eventList.size());
        for (int i = 0; i < eventList.size(); i++) {
            Object element = eventList.get(i);
            if (!(element instanceof Map)) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateEventListener: invalid event object");
            }
            Map<String, Object> events = (Map) element;
            Map<String, Object> eventProps;
            int nodeId;
            try {
                nodeId = ((Number) Objects.requireNonNull(events.get(NODE_ID))).intValue();
                eventProps = (Map) Objects.requireNonNull(events.get(NODE_PROPS));
            } catch (NullPointerException e) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR, e);
            }
            // The node id should not be negative number.
            if (nodeId < 0) {
                throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                        TAG + ": updateEventListener: invalid negative id=" + nodeId);
            }
            mVirtualNodeManager.updateEventListener(rootId, nodeId, eventProps);
            final int id = nodeId;
            final Map<String, Object> props = eventProps;
            taskList.add(new UITaskExecutor() {
                @Override
                public void exec() {
                    mRenderManager.updateEventListener(rootId, id, props);
                }
            });
        }
        if (!taskList.isEmpty()) {
            addUITask(getMassTaskExecutor(taskList));
        }
    }

    @Override
    public long measure(int rootId, int nodeId, float width, int widthMode, float height,
            int heightMode) {
        try {
            FlexMeasureMode flexMeasureMode = FlexMeasureMode.fromInt(widthMode);
            return mVirtualNodeManager.measure(rootId, nodeId, width, flexMeasureMode);
        } catch (NativeRenderException e) {
            handleRenderException(e);
        }
        return FlexUtils.makeSizeToLong(width, height);
    }

    @Override
    public void callUIFunction(final int rootId, final int nodeId, final long callbackId,
            @NonNull final String functionName,
            @NonNull final List<Object> params) throws NativeRenderException {
        // The node id should not be negative number.
        if (nodeId < 0) {
            throw new NativeRenderException(INVALID_NODE_DATA_ERR,
                    TAG + ": callUIFunction: invalid negative id=" + nodeId);
        }
        // If callbackId equal to 0 mean this call does not need to callback.
        final UIPromise promise =
                (callbackId == 0) ? null : new UIPromise(callbackId, functionName, rootId, nodeId,
                        mRenderProvider.getInstanceId());
        // Because call ui function will not follow with end batch,
        // can be directly post to the UI thread do execution.
        UIThreadUtils.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mRenderManager.dispatchUIFunction(rootId, nodeId, functionName, params, promise);
            }
        });
    }

    @Override
    public void doPromiseCallBack(int result, long callbackId, @NonNull String functionName,
            int rootId, int nodeId, @Nullable Object params) {
        mRenderProvider.doPromiseCallBack(result, callbackId, functionName, rootId, nodeId, params);
    }

    @Override
    public void endBatch(final int rootId) throws NativeRenderException {
        Map<Integer, Layout> layoutToUpdate = mVirtualNodeManager.endBatch(rootId);
        if (layoutToUpdate != null) {
            for (Entry<Integer, Layout> entry : layoutToUpdate.entrySet()) {
                final int id = entry.getKey();
                final Layout layout = entry.getValue();
                if (rootId == SCREEN_SNAPSHOT_ROOT_ID) {
                    mRenderManager.updateExtra(rootId, id, layout);
                } else {
                    addUITask(new UITaskExecutor() {
                        @Override
                        public void exec() {
                            mRenderManager.updateExtra(rootId, id, layout);
                        }
                    });
                }
            }
        }
        if (rootId == SCREEN_SNAPSHOT_ROOT_ID) {
            mRenderManager.batch(rootId);
        } else {
            addUITask(new UITaskExecutor() {
                @Override
                public void exec() {
                    mRenderManager.batch(rootId);
                }
            });
            executeUITask();
        }
    }

    @Override
    @Nullable
    public VirtualNode createVirtualNode(int rootId, int id, int pid, int index,
            @NonNull String className, @Nullable Map<String, Object> props) {
        return mRenderManager.createVirtualNode(rootId, id, pid, index, className, props);
    }

    private void addUITask(@NonNull UITaskExecutor task) throws NativeRenderException {
        try {
            // It is generally preferable to use add here, just focus the exception
            // when add failed, don't need to handle the return value.
            mUITaskQueue.add(task);
        } catch (ClassCastException | NullPointerException | IllegalArgumentException e) {
            throw new NativeRenderException(UI_TASK_QUEUE_ADD_ERR, e);
        } catch (IllegalStateException e) {
            // If the element cannot be added at this time due to capacity restrictions,
            // the main thread may blocked, serious error!!!
            mUITaskQueue.clear();
            throw new NativeRenderException(UI_TASK_QUEUE_UNAVAILABLE_ERR, e);
        }
    }

    private void executeUITask() {
        final int size = mUITaskQueue.size();
        UIThreadUtils.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                long start = System.currentTimeMillis();
                int count = size;
                while (count > 0) {
                    UITaskExecutor task = mUITaskQueue.poll();
                    if (task != null) {
                        task.exec();
                    }
                    count--;
                }
                LogUtils.d(TAG,
                        "executeUITask: size " + size + ", time " + (System.currentTimeMillis()
                                - start));
            }
        });
    }

    /**
     * Decode snapshot buffer, can be called without waiting for the engine initialization to
     * complete
     *
     * <p>
     * As the decoding time will increase with large number of nodes, it is recommended to call this
     * method in the sub thread
     * <p/>
     *
     * @param buffer the byte array of snapshot that save by host
     * @return the snapshot map {@link HashMap} of deserialize
     */
    @Nullable
    public static Map<String, Object> decodeSnapshot(@NonNull byte[] buffer) {
        try {
            final BinaryReader binaryReader = new SafeHeapReader();
            Deserializer deserializer = new Deserializer(null, new InternalizedStringTable());
            binaryReader.reset(ByteBuffer.wrap(buffer));
            deserializer.setReader(binaryReader);
            deserializer.reset();
            deserializer.readHeader();
            Object paramsObj = deserializer.readValue();
            deserializer.getStringTable().release();
            return (paramsObj instanceof HashMap) ? (HashMap<String, Object>) paramsObj : null;
        } catch (Exception e) {
            LogUtils.e(TAG, "decodeSnapshot: " + e.getMessage());
            return null;
        }
    }

    /**
     * Replay snapshot with map of render node tree.
     *
     * @param context android system {@link Context} that use to create root view
     * @param snapshotMap the render node tree represented by hash map
     * @return the root view replay by snapshot, will return null if the replay failed
     */
    @SuppressWarnings("unchecked")
    @Override
    @Nullable
    public synchronized View replaySnapshot(@NonNull Context context,
            @NonNull Map<String, Object> snapshotMap) {
        View rootView;
        List<Object> nodeList;
        List<Object> layoutList;
        try {
            nodeList = (List<Object>) Objects.requireNonNull(snapshotMap.get(SNAPSHOT_CREATE_NODE));
            layoutList = (List<Object>) Objects.requireNonNull(
                    snapshotMap.get(SNAPSHOT_UPDATE_LAYOUT));
            rootView = new HippyRootView(context, mRenderProvider.getInstanceId(),
                    SCREEN_SNAPSHOT_ROOT_ID);
            mRenderManager.createRootNode(SCREEN_SNAPSHOT_ROOT_ID);
            mRenderManager.addRootView(rootView);
            createNode(SCREEN_SNAPSHOT_ROOT_ID, nodeList);
            updateLayout(SCREEN_SNAPSHOT_ROOT_ID, layoutList);
            endBatch(SCREEN_SNAPSHOT_ROOT_ID);
        } catch (Exception e) {
            LogUtils.e(TAG, "replaySnapshot: " + e.getMessage());
            return null;
        }
        return rootView;
    }

    /**
     * Replay snapshot with byte buffer of render node tree.
     *
     * @param context android system {@link Context} that use to create root view
     * @param buffer the render node tree represented by byte buffer
     * @return the root view replay by snapshot, will return null if the replay failed
     */
    @Override
    @Nullable
    public View replaySnapshot(@NonNull Context context, @NonNull byte[] buffer) {
        final Map<String, Object> snapshotMap = decodeSnapshot(buffer);
        if (snapshotMap != null) {
            return replaySnapshot(context, snapshotMap);
        }
        return null;
    }

    /**
     * Record snapshot to byte buffer.
     *
     * <p>
     * Hippy SDK does not store the buffer generated by recording, because the policy of buffer
     * invalidation needs to be defined by the business itself, the implementation of storage
     * needs to be completed by the host itself
     * <p/>
     *
     * @param rootId the root view id
     * @param callback return the result of record by {@link Callback}
     */
    @Override
    public void recordSnapshot(int rootId, @NonNull final Callback<byte[]> callback) {
        RenderNode rootNode = NativeRendererManager.getRootNode(rootId);
        if (rootNode == null) {
            return;
        }
        List<Map<String, Object>> nodeInfoList = new ArrayList<>(80);
        List<Map<String, Object>> layoutInfoList = new ArrayList<>(80);
        int displayWidth = DisplayUtils.getScreenWidth();
        int displayHeight = DisplayUtils.getScreenHeight();
        View rootView = mRenderManager.getRootView(rootId);
        if (rootView != null && rootView.getWidth() > 0 && rootView.getHeight() > 0) {
            displayWidth = rootView.getWidth();
            displayHeight = rootView.getHeight();
        }
        Rect displayArea = new Rect(0, 0, displayWidth, displayHeight);
        performNodeTreeTraversals(rootNode, 0, 0, displayArea, nodeInfoList, layoutInfoList);
        final Map<String, Object> snapshot = new HashMap<>();
        snapshot.put(SNAPSHOT_CREATE_NODE, nodeInfoList);
        snapshot.put(SNAPSHOT_UPDATE_LAYOUT, layoutInfoList);
        Runnable task = new Runnable() {
            @Override
            public void run() {
                try {
                    ByteBuffer buffer = encodeSnapshot(snapshot);
                    callback.callback(buffer.array(), null);
                } catch (Exception e) {
                    callback.callback(null, e);
                }
            }
        };
        Executor executor = getBackgroundExecutor();
        if (executor == null) {
            task.run();
        } else {
            executor.execute(task);
        }
    }

    private ByteBuffer encodeSnapshot(@NonNull Map<String, Object> snapshot)
            throws NativeRenderException {
        SafeHeapWriter safeHeapWriter = new SafeHeapWriter();
        Serializer serializer = new Serializer();
        serializer.setWriter(safeHeapWriter);
        serializer.reset();
        serializer.writeHeader();
        serializer.writeValue(snapshot);
        return safeHeapWriter.chunked();
    }

    private void performNodeTreeTraversals(@NonNull RenderNode parent, int left, int top,
            Rect displayArea, @NonNull List<Map<String, Object>> nodeInfoList,
            @NonNull List<Map<String, Object>> layoutInfoList) {
        int childCount = parent.getChildCount();
        int pid = (parent instanceof RootRenderNode) ? SCREEN_SNAPSHOT_ROOT_ID : parent.getId();
        for (int i = 0; i < childCount; i++) {
            RenderNode child = parent.getChildAt(i);
            if (child == null) {
                continue;
            }
            // If the parent node display area is no longer in the screen, do not need to
            // traverse the child node
            if (collectNodeInfo(child, pid, left, top, displayArea, nodeInfoList, layoutInfoList)) {
                performNodeTreeTraversals(child, left + child.getX(), top + child.getY(),
                        displayArea, nodeInfoList, layoutInfoList);
            }
        }
    }

    private boolean collectNodeInfo(@NonNull RenderNode child, int pid, int outerLeft, int outerTop,
            Rect displayArea, @NonNull List<Map<String, Object>> nodeInfoList,
            @NonNull List<Map<String, Object>> layoutInfoList) {
        int left = (child instanceof ListItemRenderNode) ? ((ListItemRenderNode) child).getLeft()
                : child.getX();
        int top = (child instanceof ListItemRenderNode) ? ((ListItemRenderNode) child).getTop()
                : child.getY();
        int width = child.getWidth() == 0 ? displayArea.width() : child.getWidth();
        int height = child.getHeight() == 0 ? displayArea.height() : child.getHeight();
        if (!displayArea.intersects(left + outerLeft, top + outerTop,
                left + outerLeft + width, top + outerTop + height)) {
            // If the display area of this node is no longer in the screen,
            // do not need to cache the node information.
            return false;
        }
        Map<String, Object> layoutInfo = new HashMap<>();
        layoutInfo.put(NODE_ID, child.getId());
        layoutInfo.put(LAYOUT_WIDTH, child.getWidth());
        layoutInfo.put(LAYOUT_HEIGHT, child.getHeight());
        layoutInfo.put(LAYOUT_LEFT, left);
        layoutInfo.put(LAYOUT_TOP, top);
        if (child instanceof TextRenderNode) {
            layoutInfo.put(PADDING_LEFT, ((TextRenderNode) child).getPaddingLeft());
            layoutInfo.put(PADDING_RIGHT, ((TextRenderNode) child).getPaddingRight());
            layoutInfo.put(PADDING_TOP, ((TextRenderNode) child).getPaddingTop());
            layoutInfo.put(PADDING_BOTTOM, ((TextRenderNode) child).getPaddingBottom());
        }
        layoutInfoList.add(layoutInfo);

        Map<String, Object> nodeInfo = new HashMap<>();
        nodeInfo.put(NODE_ID, child.getId());
        nodeInfo.put(NODE_PID, pid);
        nodeInfo.put(NODE_INDEX, child.getIndex());
        nodeInfo.put(CLASS_NAME, child.getClassName());
        nodeInfo.put(NODE_PROPS, child.getProps());
        nodeInfoList.add(nodeInfo);
        if (child instanceof TextRenderNode) {
            ((TextRenderNode) child).recordVirtualChildren(nodeInfoList);
        }
        return true;
    }

    private interface UITaskExecutor {

        void exec();
    }
}
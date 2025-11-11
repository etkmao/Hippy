import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { LogUtils } from 'hippy/src/main/ets/support/utils/LogUtils';

export class FloatingWindowManager {
  TAG = 'FloatingWindowManager'
  static windowWidth = 800//440
  static windowHeight = 800//200
  static WINDOW_POS_X = 'WINDOW_POS_X'
  static WINDOW_POS_Y = 'WINDOW_POS_Y'

  static PAGE_NAME = "pages/IndexOfSubWin"

  private static instance: FloatingWindowManager;
  private subWindow: window.Window | null = null;
  private windowStage: window.WindowStage | null = null;
  private imageUrl: string = '';
  private clickCallback: () => void = () => {
  };
  private closeCallback: () => void = () => {
  };
  private opCallback: (state: boolean) => void = (state) => {
  };
  private currentX: number = 200;
  private currentY: number = 400;

  private constructor() {
  }

  public static getInstance(): FloatingWindowManager {
    if (!FloatingWindowManager.instance) {
      FloatingWindowManager.instance = new FloatingWindowManager();
    }
    return FloatingWindowManager.instance;
  }

  // 初始化在鸿蒙侧
  public init(stage: window.WindowStage) {
    this.windowStage = stage;
  }

  public show(imageUrl: string, onClick: () => void, onClose: () => void, onOp: () => void) {
    if (this.subWindow?.isWindowShowing() && this.imageUrl === imageUrl) {
      LogUtils.i(this.TAG, '已经存在窗口，并且图片没有变化，返回')
      return
    }
    this.imageUrl = imageUrl;
    if (onClick) {
      this.clickCallback = onClick; // 点击回调
    }
    if (onClose) {
      this.closeCallback = onClose; // 关闭回调
    }
    if (onOp) {
      this.opCallback = onOp; // 操作回调
    }

    if (!this.windowStage) {
      LogUtils.e(this.TAG, 'windowStage not initialized');
      return;
    }

    // 如果已存在子窗口，直接更新内容并显示
    if (this.subWindow) {
      this.subWindow?.moveWindowTo(this.currentX, this.currentY, () => {
      });
      this.subWindow.setUIContent(FloatingWindowManager.PAGE_NAME, () => {
        this.subWindow?.showWindow((err: BusinessError) => {
          let errCode: number = err.code;
          if (errCode) {
            LogUtils.e(this.TAG, 'Failed to show the window. Cause: ' + JSON.stringify(err));
            return;
          }
          LogUtils.i(this.TAG, 'Succeeded in showing the window.');
        });
      });

      return;
    }

    // 创建新的子窗口
    this.windowStage.createSubWindow('floating_window', (err: BusinessError, win: window.Window) => {
      if (err.code !== 0) {
        LogUtils.e(this.TAG, 'createSubWindow error:' + JSON.stringify(err));
        return;
      }

      this.subWindow = win;
      win.setWindowFocusable(false)
      LogUtils.d(this.TAG, 'currentY:' + this.currentY)
      win.moveWindowTo(this.currentX, this.currentY, () => {
      });
      win.resize(FloatingWindowManager.windowWidth, FloatingWindowManager.windowHeight,
        (err: BusinessError) => {
          let errCode: number = err.code;
          if (errCode) {
            LogUtils.e(this.TAG, 'Failed to change the window size. Cause:' + JSON.stringify(err));
            return;
          }
          LogUtils.i(this.TAG, 'Succeeded in changing the window size.');
        });
      win.setUIContent(FloatingWindowManager.PAGE_NAME, (err: BusinessError) => {
        let errCode: number = err.code;
        if (errCode) {
          LogUtils.e(this.TAG, 'Failed to load the content. Cause:' + JSON.stringify(err));
          return;
        }
        LogUtils.i(this.TAG, 'Succeeded in loading the content.');
        win.setWindowBackgroundColor('#00000000')
        win.showWindow((err: BusinessError) => {
          let errCode: number = err.code;
          if (errCode) {
            LogUtils.e(this.TAG, 'Failed to show the window. Cause: ' + JSON.stringify(err));
            return;
          }
          LogUtils.i(this.TAG, 'Succeeded in showing the window.');
        });
      });

    });
  }

  public close(needNotify: boolean) {
    if (this.subWindow) {
      this.subWindow.destroyWindow(() => {
        this.subWindow = null;
        if (needNotify) { // 某些主动隐藏的场景无需关注关闭回调
          this.closeCallback?.();
        }
      });
    }
  }

  public getX(): number {
    return this.currentX;
  }

  public getY(): number {
    return this.currentY;
  }

  public getImageUrl(): string {
    return this.imageUrl;
  }

  public triggerClick() {
    this.clickCallback?.();
  }

  public triggerOp(playing: boolean) {
    this.opCallback?.(playing);
  }

  public updatePosition(x: number, y: number) {
    this.currentX = x;
    this.currentY = y;
    if (this.subWindow) {
      this.subWindow.moveWindowTo(x, y, (err: BusinessError) => {
        if (err.code !== 0) {
          LogUtils.e(this.TAG, 'moveWindowTo error:' + JSON.stringify(err));
        }
      });
    }
  }

  showing(): boolean {
    return this.subWindow?.isWindowShowing() ?? false;
  }
}

#import <PencilKit/PencilKit.h>
#import "core/platform/darwin/ios/view/EmbeddedHostView.h"

extern NSString *const kTDFPencilCanvasViewKey;

API_AVAILABLE(ios(13.0))
@interface TDFPencilCanvasView : EmbeddedHostView

@property(nonatomic, strong) PKToolPicker *picker;
@property(nonatomic, strong) PKCanvasView *canvasView;

@end

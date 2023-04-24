#import <UIKit/UIKit.h>
#import "core/common/rect.h"
#import "core/common/rrect.h"
#import "core/engine/graphics/path.h"
#import "core/platform/darwin/ios/view/EmbeddedHostView.h"

#include <vector>

@interface TDFChildClippingView : UIView

@property(nonatomic, strong) EmbeddedHostView *embeddedView;

- (instancetype)initWithEmbeddedHostView:(EmbeddedHostView *)embeddedView;

- (void)updateMutators:(const std::vector<std::shared_ptr<tdfcore::Mutator>> &)mutators;

@end

@interface TDFClippingMaskView : UIView
- (void)clipRect:(const tdfcore::TRect &)rect matrix:(const CGAffineTransform &)matrix;
- (void)clipRRect:(const tdfcore::RRect &)rrect matrix:(const CGAffineTransform &)matrix;
- (void)clipPath:(const tdfcore::Path &)path matrix:(const CGAffineTransform &)matrix;

@end

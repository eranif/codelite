#ifdef __WXMAC__
#import <Cocoa/Cocoa.h>

#include "WindowUtils.hpp"

#include <wx/window.h>

namespace {
// Makes the native controls inside the popup draw no background, so the blur
// behind them shows through
void ClearBackgrounds(NSView *view) {
  if ([view isKindOfClass:[NSScrollView class]]) {
    [(NSScrollView *)view setDrawsBackground:NO];
  } else if ([view isKindOfClass:[NSClipView class]]) {
    [(NSClipView *)view setDrawsBackground:NO];
  } else if ([view isKindOfClass:[NSTableView class]]) {
    [(NSTableView *)view setBackgroundColor:[NSColor clearColor]];
  }
  for (NSView *child in [view subviews]) {
    ClearBackgrounds(child);
  }
}
} // namespace

void StylePopupWindow(wxWindow *popup, double radius) {
  if (popup == nullptr) {
    return;
  }

  NSWindow *win = (NSWindow *)popup->MacGetTopLevelWindowRef();
  if (win == nil) {
    return;
  }

  // A transparent window with a shadow; the content view is clipped to the
  // rounded rectangle
  [win setOpaque:NO];
  [win setBackgroundColor:[NSColor clearColor]];
  [win setHasShadow:YES];

  NSView *content = [win contentView];
  content.wantsLayer = YES;
  content.layer.cornerRadius = radius;
  content.layer.masksToBounds = YES;
  content.layer.borderWidth = 1.0;
  content.layer.borderColor = [[NSColor separatorColor] CGColor];

  // Translucent: a blurred view behind all the controls, whose own
  // backgrounds are removed
  NSVisualEffectView *blur =
      [[NSVisualEffectView alloc] initWithFrame:[content bounds]];
  blur.material = NSVisualEffectMaterialHUDWindow;
  blur.blendingMode = NSVisualEffectBlendingModeBehindWindow;
  blur.state = NSVisualEffectStateActive;
  blur.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
  ClearBackgrounds(content);
  [content addSubview:blur positioned:NSWindowBelow relativeTo:nil];

  // Recompute the shadow for the new (rounded) shape
  [win invalidateShadow];
}
#endif

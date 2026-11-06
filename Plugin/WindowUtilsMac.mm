#ifdef __WXMAC__
#import <Cocoa/Cocoa.h>

#include "WindowUtils.hpp"

#include <wx/window.h>

void StylePopupWindow(wxWindow* popup, double radius)
{
    if (popup == nullptr) {
        return;
    }

    NSWindow* win = (NSWindow*)popup->MacGetTopLevelWindowRef();
    if (win == nil) {
        return;
    }

    // A transparent window with a shadow; the content view is clipped to the rounded rectangle
    [win setOpaque:NO];
    [win setBackgroundColor:[NSColor clearColor]];
    [win setHasShadow:YES];

    NSView* content = [win contentView];
    content.wantsLayer = YES;
    content.layer.cornerRadius = radius;
    content.layer.masksToBounds = YES;
    content.layer.borderWidth = 5.0;
    content.layer.borderColor = [[NSColor separatorColor] CGColor];

    // Recompute the shadow for the new (rounded) shape
    [win invalidateShadow];
}
#endif

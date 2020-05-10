#ifndef WXCUSTOMCONTROLS_HPP
#define WXCUSTOMCONTROLS_HPP

#ifdef __WXGTK__

// GTK2 & 3
#define wxUSE_NATIVE_CHOICE 1
#define wxUSE_NATIVE_BUTTON 1
#define wxUSE_NATIVE_SCROLLBAR 0

#elif defined(__WXOSX__)
// macOS
#define wxUSE_NATIVE_CHOICE 1
#define wxUSE_NATIVE_BUTTON 1
#define wxUSE_NATIVE_SCROLLBAR 1

#else
// Windows
#define wxUSE_NATIVE_CHOICE 0
#define wxUSE_NATIVE_BUTTON 0
#define wxUSE_NATIVE_SCROLLBAR 1

#endif
#endif // WXCUSTOMCONTROLS_HPP

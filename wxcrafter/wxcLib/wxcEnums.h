#ifndef __WXC_ENUMS_H__
#define __WXC_ENUMS_H__

namespace wxCrafter
{
    /// Commands supported by wxCrafter
enum eCommandType {
    kCommandTypeInvalid = -1,
    kCommandTypeLoadFile,
    kCommandTypeGenerateCode,
    kCommandTypeShowDesigner,
    kCommandTypeExit,
    kCommandTypeNewWizard,
    kCommandTypeNewDialog,
    kCommandTypeNewFrame,
    kCommandTypeNewPanel,
    kCommandTypeNewPopupWindow,
    kCommandTypeNewImageList,
};

enum eReplyType {
    kReplyTypeInvalid = -1,
    kReplyTypeFilesGenerated,
};
}
#endif // __WXC_ENUMS_H__


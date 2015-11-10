<?php

/**
 * A helper script that maps between Ubuntu Humanity 
 * SVG files into CodeLite SVG files
 */
 
$FAILSAFE_DIR = "C:\\Users\\PC\\Desktop\\Humanity\\ColorsHumanity";
$SOURCE_DIR = "C:\\Users\\PC\\Desktop\\Humanity\\ColorsHumanity";
$TARGET_DIR = "C:\\src\\codelite\\svgs";

$MISSING_FILES = array();

@mkdir("{$TARGET_DIR}/16");
@mkdir("{$TARGET_DIR}/24");

class ImageFile {
    public $srcPath = "";
    public $targetPath = "";
    
    public function __construct($src, $target) {
        global $TARGET_DIR, $SOURCE_DIR, $FAILSAFE_DIR;
        $this->srcPath = "{$SOURCE_DIR}/{$src}";
        if(!file_exists($this->srcPath)) {
            $this->srcPath = "{$FAILSAFE_DIR}/{$src}";
        }
        $this->targetPath = "{$TARGET_DIR}/{$target}";
    }
    
    /**
     * @brief copy source file to destination path
     */
    public function copy() {
        global $MISSING_FILES;
        echo "Copying {$this->srcPath} -> 16/{$this->targetPath}\n";
        if(!file_exists($this->srcPath)) {
            $MISSING_FILES[] = $this->srcPath;
        } else {
            copy($this->srcPath, $this->targetPath);
        }
    }
}

// Unified size icons for both 16 and 24 
// This is because there is no "16" version for these bitmaps
$OnSizeIcons[] = new ImageFile("actions\\24\\edit-undo.svg", "undo.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\edit-redo.svg", "redo.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\preview-file.svg", "find.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\system-run.svg", "execute.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\go-jump.svg", "step_in.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\go-jump-up.svg", "step_out.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\document-open-recent.svg", "history.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\system-run.svg", "executable.svg");
$OnSizeIcons[] = new ImageFile("actions\\24\\system-run.svg", "dll.svg");
$OnSizeIcons[] = new ImageFile("apps\\48\\utilities-terminal.svg", "console.svg");
$OnSizeIcons[] = new ImageFile("mimes\\48\\multipart-encrypted.svg", "binary-file.svg");
$OnSizeIcons[] = new ImageFile("apps\\24\\gnome-session-switch.svg", "diff.svg");
$OnSizeIcons[] = new ImageFile("categories\\24\\applications-engineering.svg", "tools.svg");
$OnSizeIcons[] = new ImageFile("status\\24\\network-offline.svg", "disconnected.svg");
$OnSizeIcons[] = new ImageFile("status\\24\\network-transmit-receive.svg", "connected.svg");
$OnSizeIcons[] = new ImageFile("apps\\24\\checkbox.svg", "checkbox.svg");
$OnSizeIcons[] = new ImageFile("status\\24\\media-playlist-repeat.svg", "repeat.svg");
$OnSizeIcons[] = new ImageFile("categories\\24\\preferences-other.svg", "tools.svg");

// 16x16 icons size
$SmallIcons[] = new ImageFile("actions\\16\\document-new.svg", "16/file_new.svg");
$SmallIcons[] = new ImageFile("actions\\16\\document-open.svg", "16/file_open.svg");
$SmallIcons[] = new ImageFile("actions\\16\\document-revert.svg", "16/file_reload.svg");
$SmallIcons[] = new ImageFile("actions\\16\\document-save.svg", "16/file_save.svg");
$SmallIcons[] = new ImageFile("actions\\16\\window-close.svg", "16/file_close.svg");
$SmallIcons[] = new ImageFile("actions\\16\\edit-cut.svg", "16/cut.svg");
$SmallIcons[] = new ImageFile("actions\\16\\edit-copy.svg", "16/copy.svg");
$SmallIcons[] = new ImageFile("actions\\16\\edit-paste.svg", "16/paste.svg");
$SmallIcons[] = new ImageFile("actions\\16\\edit-clear.svg", "16/clear.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-previous.svg", "16/back.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-next.svg", "16/forward.svg");
$SmallIcons[] = new ImageFile("actions\\16\\bookmark-new.svg", "16/bookmark.svg");
$SmallIcons[] = new ImageFile("actions\\16\\gtk-edit.svg", "16/find_and_replace.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder-saved-search.svg", "16/find_in_files.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder-documents.svg", "16/open_resource.svg");
$SmallIcons[] = new ImageFile("categories\\16\\applications-graphics.svg", "16/mark_word.svg");
$SmallIcons[] = new ImageFile("mimes\\16\\text-x-install.svg", "16/build.svg");
$SmallIcons[] = new ImageFile("actions\\16\\process-stop.svg", "16/stop.svg");
$SmallIcons[] = new ImageFile("places\\16\\user-trash.svg", "16/clean.svg");
$SmallIcons[] = new ImageFile("actions\\16\\process-stop.svg", "16/execute_stop.svg");
$SmallIcons[] = new ImageFile("actions\\16\\media-playback-start.svg", "16/debugger_start.svg");
$SmallIcons[] = new ImageFile("actions\\16\\media-playback-stop.svg", "16/debugger_stop.svg");
$SmallIcons[] = new ImageFile("actions\\16\\media-playback-pause.svg", "16/interrupt.svg");
$SmallIcons[] = new ImageFile("actions\\16\\view-refresh.svg", "16/debugger_restart.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-down.svg", "16/next.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-next.svg", "16/show_current_line.svg");
$SmallIcons[] = new ImageFile("actions\\16\\document-properties.svg", "16/cog.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder-remote.svg", "16/remote-folder.svg");
$SmallIcons[] = new ImageFile("mimes\\16\\package-x-generic.svg", "16/archive.svg");
$SmallIcons[] = new ImageFile("mimes\\16\\text-x-generic.svg", "16/log.svg");
$SmallIcons[] = new ImageFile("apps\\16\\evolution-tasks.svg", "16/tasks.svg");
$SmallIcons[] = new ImageFile("status\\16\\locked.svg", "16/lock.svg");
$SmallIcons[] = new ImageFile("actions\\16\\help-contents.svg", "16/help.svg");
$SmallIcons[] = new ImageFile("actions\\16\\gtk-info.svg", "16/info.svg");
$SmallIcons[] = new ImageFile("actions\\16\\dialog-apply.svg", "16/ok.svg");
$SmallIcons[] = new ImageFile("actions\\16\\list-add.svg", "16/plus.svg");
$SmallIcons[] = new ImageFile("actions\\16\\list-remove.svg", "16/minus.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-down.svg", "16/down.svg");
$SmallIcons[] = new ImageFile("actions\\16\\go-up.svg", "16/up.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder.svg", "16/folder.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder-publicshare.svg", "16/folder-users.svg");
$SmallIcons[] = new ImageFile("apps\\16\\system-file-manager.svg", "16/cxx-workspace.svg");
$SmallIcons[] = new ImageFile("places\\16\\folder-templates.svg", "16/folder-templates.svg");
$SmallIcons[] = new ImageFile("status\\16\\dialog-warning.svg", "16/warning.svg");
$SmallIcons[] = new ImageFile("status\\16\\dialog-error.svg", "16/error.svg");

// 16x16 icons size
$LargeIcons[] = new ImageFile("actions\\24\\document-new.svg", "24/file_new.svg");
$LargeIcons[] = new ImageFile("actions\\24\\document-open.svg", "24/file_open.svg");
$LargeIcons[] = new ImageFile("actions\\24\\document-revert.svg", "24/file_reload.svg");
$LargeIcons[] = new ImageFile("actions\\24\\document-save.svg", "24/file_save.svg");
$LargeIcons[] = new ImageFile("actions\\24\\window-close.svg", "24/file_close.svg");
$LargeIcons[] = new ImageFile("actions\\24\\edit-cut.svg", "24/cut.svg");
$LargeIcons[] = new ImageFile("actions\\24\\edit-copy.svg", "24/copy.svg");
$LargeIcons[] = new ImageFile("actions\\24\\edit-paste.svg", "24/paste.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-previous.svg", "24/back.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-next.svg", "24/forward.svg");
$LargeIcons[] = new ImageFile("actions\\24\\bookmark-new.svg", "24/bookmark.svg");
$LargeIcons[] = new ImageFile("actions\\24\\gtk-edit.svg", "24/find_and_replace.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder-saved-search.svg", "24/find_in_files.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder-documents.svg", "24/open_resource.svg");
$LargeIcons[] = new ImageFile("categories\\24\\applications-graphics.svg", "24/mark_word.svg");
$LargeIcons[] = new ImageFile("mimes\\24\\text-x-install.svg", "24/build.svg");
$LargeIcons[] = new ImageFile("actions\\24\\process-stop.svg", "24/stop.svg");
$LargeIcons[] = new ImageFile("places\\24\\user-trash.svg", "24/clean.svg");
$LargeIcons[] = new ImageFile("actions\\24\\process-stop.svg", "24/execute_stop.svg");
$LargeIcons[] = new ImageFile("actions\\24\\media-playback-start.svg", "24/debugger_start.svg");
$LargeIcons[] = new ImageFile("actions\\24\\media-playback-stop.svg", "24/debugger_stop.svg");
$LargeIcons[] = new ImageFile("actions\\24\\media-playback-pause.svg", "24/interrupt.svg");
$LargeIcons[] = new ImageFile("actions\\24\\view-refresh.svg", "24/debugger_restart.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-down.svg", "24/next.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-next.svg", "24/show_current_line.svg");
$LargeIcons[] = new ImageFile("actions\\24\\document-properties.svg", "24/cog.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder-remote.svg", "24/remote-folder.svg");
$LargeIcons[] = new ImageFile("mimes\\24\\package-x-generic.svg", "24/archive.svg");
$LargeIcons[] = new ImageFile("mimes\\24\\text-x-generic.svg", "24/log.svg");
$LargeIcons[] = new ImageFile("apps\\24\\evolution-tasks.svg", "24/tasks.svg");
$LargeIcons[] = new ImageFile("status\\24\\locked.svg", "24/lock.svg");
$LargeIcons[] = new ImageFile("actions\\24\\help-contents.svg", "24/help.svg");
$LargeIcons[] = new ImageFile("actions\\24\\gtk-info.svg", "24/info.svg");
$LargeIcons[] = new ImageFile("actions\\24\\dialog-apply.svg", "24/ok.svg");
$LargeIcons[] = new ImageFile("actions\\24\\list-add.svg", "24/plus.svg");
$LargeIcons[] = new ImageFile("actions\\24\\list-remove.svg", "24/minus.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-down.svg", "24/down.svg");
$LargeIcons[] = new ImageFile("actions\\24\\go-up.svg", "24/up.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder.svg", "24/folder.svg");
$LargeIcons[] = new ImageFile("actions\\24\\edit-clear.svg", "24/clear.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder-publicshare.svg", "24/folder-users.svg");
$LargeIcons[] = new ImageFile("places\\24\\user-home.svg", "24/cxx-workspace.svg");
$LargeIcons[] = new ImageFile("apps\\24\\system-file-manager.svg", "24/cxx-workspace.svg");
$LargeIcons[] = new ImageFile("places\\24\\folder-templates.svg", "24/folder-templates.svg");
$LargeIcons[] = new ImageFile("status\\24\\dialog-warning.svg", "24/warning.svg");
$LargeIcons[] = new ImageFile("status\\24\\dialog-error.svg", "24/error.svg");

/** @var ImageFile */
foreach($SmallIcons as $file) {
    $file->copy();
}

/** @var ImageFile */
foreach($OnSizeIcons as $file) {
    $file->copy();
}

/** @var ImageFile */
foreach($LargeIcons as $file) {
    $file->copy();
}


if(!empty($MISSING_FILES)) {
    echo "Missing files:\n";
    foreach($MISSING_FILES as $file) {
        echo "{$file}\n";
    }
}

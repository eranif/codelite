<?php

$SOURCE_DIR = "C:\\Users\\PC\\Desktop\\Humanity\\ColorsHumanity";
$TARGET_DIR = "C:\\src\\codelite\\svgs";

class ImageFile {
    public $srcPath = "";
    public $targetPath = "";
    
    public function __construct($src, $target) {
        global $TARGET_DIR, $SOURCE_DIR;
        $this->srcPath = "{$SOURCE_DIR}/{$src}";
        $this->targetPath = "{$TARGET_DIR}/{$target}";
    }
    
    /**
     * @brief copy source file to destination path
     * @return  
     */
    public function copy() {
        echo "Copying {$this->srcPath} -> {$this->targetPath}\n";
        copy($this->srcPath, $this->targetPath);
    }
}

// Tasks: evolution-tasks.svg

$IN_FILES[] = new ImageFile("actions\\24\\document-new.svg", "file_new.svg");
$IN_FILES[] = new ImageFile("actions\\24\\document-open.svg", "file_open.svg");
$IN_FILES[] = new ImageFile("actions\\24\\document-revert.svg", "file_reload.svg");
$IN_FILES[] = new ImageFile("actions\\24\\document-save.svg", "file_save.svg");
$IN_FILES[] = new ImageFile("actions\\24\\window-close.svg", "file_close.svg");
$IN_FILES[] = new ImageFile("actions\\24\\edit-cut.svg", "cut.svg");
$IN_FILES[] = new ImageFile("actions\\24\\edit-copy.svg", "copy.svg");
$IN_FILES[] = new ImageFile("actions\\24\\edit-paste.svg", "paste.svg");
$IN_FILES[] = new ImageFile("actions\\24\\edit-undo.svg", "undo.svg");
$IN_FILES[] = new ImageFile("actions\\24\\edit-redo.svg", "redo.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-previous.svg", "back.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-next.svg", "forward.svg");
$IN_FILES[] = new ImageFile("actions\\24\\bookmark-new.svg", "bookmark.svg");
$IN_FILES[] = new ImageFile("actions\\24\\preview-file.svg", "find.svg");
$IN_FILES[] = new ImageFile("actions\\24\\gtk-edit.svg", "find_and_replace.svg");
$IN_FILES[] = new ImageFile("places\\24\\folder-saved-search.svg", "find_in_files.svg");
$IN_FILES[] = new ImageFile("places\\24\\folder-documents.svg", "open_resource.svg");
$IN_FILES[] = new ImageFile("categories\\24\\applications-graphics.svg", "mark_word.svg");


// Build
$IN_FILES[] = new ImageFile("mimes\\24\\text-x-install.svg", "build.svg");
$IN_FILES[] = new ImageFile("actions\\24\\process-stop.svg", "stop.svg");
$IN_FILES[] = new ImageFile("places\\24\\user-trash.svg", "clean.svg");
$IN_FILES[] = new ImageFile("actions\\24\\system-run.svg", "execute.svg");
$IN_FILES[] = new ImageFile("actions\\24\\process-stop.svg", "execute_stop.svg");

// Debugger
$IN_FILES[] = new ImageFile("actions\\48\\media-playback-start.svg", "debugger_start.svg");
$IN_FILES[] = new ImageFile("actions\\48\\media-playback-stop.svg", "debugger_stop.svg");
$IN_FILES[] = new ImageFile("actions\\48\\media-playback-pause.svg", "interrupt.svg");
$IN_FILES[] = new ImageFile("actions\\48\\view-refresh.svg", "debugger_restart.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-jump.svg", "step_in.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-jump-up.svg", "step_out.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-down.svg", "next.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-next.svg", "show_current_line.svg");

// Others
$IN_FILES[] = new ImageFile("actions\\24\\document-open-recent.svg", "history.svg");
$IN_FILES[] = new ImageFile("actions\\24\\document-properties.svg", "cog.svg");
$IN_FILES[] = new ImageFile("actions\\24\\go-home.svg", "home.svg");
$IN_FILES[] = new ImageFile("places\\24\\folder-remote.svg", "remote-folder.svg");
$IN_FILES[] = new ImageFile("mimes\\16\\package-x-generic.svg", "archive.svg");
$IN_FILES[] = new ImageFile("actions\\24\\system-run.svg", "executable.svg");
$IN_FILES[] = new ImageFile("actions\\24\\system-run.svg", "dll.svg");
$IN_FILES[] = new ImageFile("apps\\24\\utilities-terminal.svg", "console.svg");
$IN_FILES[] = new ImageFile("mimes\\24\\multipart-encrypted.svg", "binary-file.svg");
$IN_FILES[] = new ImageFile("apps\\24\\logviewer.svg", "log.svg");
$IN_FILES[] = new ImageFile("apps\\24\\evolution-tasks.svg", "tasks.svg");
$IN_FILES[] = new ImageFile("apps\\24\\gnome-session-switch.svg", "diff.svg");
$IN_FILES[] = new ImageFile("status\\24\\locked.svg", "lock.svg");
$IN_FILES[] = new ImageFile("actions\\24\\help-contents.svg", "help.svg");
$IN_FILES[] = new ImageFile("actions\\24\\gtk-info.svg", "info.svg");
$IN_FILES[] = new ImageFile("actions\\24\\dialog-apply.svg", "ok.svg");
$IN_FILES[] = new ImageFile("actions\\24\\list-add.svg", "plus.svg");
$IN_FILES[] = new ImageFile("actions\\24\\list-remove.svg", "minus.svg");
$IN_FILES[] = new ImageFile("categories\\24\\applications-engineering.svg", "tools.svg");
$IN_FILES[] = new ImageFile("status\\24\\network-offline.svg", "disconnected.svg");
$IN_FILES[] = new ImageFile("status\\24\\network-transmit-receive.svg", "connected.svg");

// Mime type
$IN_FILES[] = new ImageFile("places\\24\\folder.svg", "folder.svg");

/** @var ImageFile */
foreach($IN_FILES as $file) {
    $file->copy();
}

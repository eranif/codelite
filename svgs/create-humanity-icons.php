<?php

$SOURCE_DIR = "/cygdrive/c/Users/PC/Desktop/Humanity/ColorsHumanity";
$TARGET_DIR = "/cygdrive/c/src/codelite/svgs";      

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
        copy($this->srcPath, $this->targetPath);
    }
}

$IN_FILES[] = new ImageFile("actions/24/document-new.svg", "file_new.svg");
$IN_FILES[] = new ImageFile("actions/24/document-open.svg", "file_open.svg");
$IN_FILES[] = new ImageFile("actions/24/document-revert.svg", "file_reload.svg");
$IN_FILES[] = new ImageFile("actions/24/document-save.svg", "file_save.svg");
$IN_FILES[] = new ImageFile("actions/24/window-close.svg", "file_close.svg");
$IN_FILES[] = new ImageFile("actions/24/edit-cut.svg", "cut.svg");
$IN_FILES[] = new ImageFile("actions/24/edit-copy.svg", "copy.svg");
$IN_FILES[] = new ImageFile("actions/24/edit-paste.svg", "paste.svg");
$IN_FILES[] = new ImageFile("actions/24/edit-undo.svg", "undo.svg");
$IN_FILES[] = new ImageFile("actions/24/edit-redo.svg", "redo.svg");
$IN_FILES[] = new ImageFile("actions/24/go-previous.svg", "back.svg");
$IN_FILES[] = new ImageFile("actions/24/go-next.svg", "forward.svg");
$IN_FILES[] = new ImageFile("actions/24/bookmark-new.svg", "bookmark.svg");
$IN_FILES[] = new ImageFile("actions/24/preview-file.svg", "find.svg");
$IN_FILES[] = new ImageFile("actions/24/gtk-edit.svg", "find_and_replace.svg");
$IN_FILES[] = new ImageFile("places/24/folder-saved-search.svg", "find_in_files.svg");
$IN_FILES[] = new ImageFile("places/24/folder-documents.svg", "open_resource.svg");

/** @var ImageFile */
foreach($IN_FILES as $file) {
    $file->copy();
}

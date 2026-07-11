#!/bin/bash

# Get the latest commit hash from the GitHub API
COMMIT_HASH=$(curl -s https://api.github.com/repos/JetBrains/phpstorm-stubs/commits/master | jq -r '.sha')

ZIP_URL="https://api.github.com/repos/JetBrains/phpstorm-stubs/zipball/$COMMIT_HASH"
ZIP_FILE="phpstorm-stubs.zip"
EXTRACT_DIR="./cc/stubs"
PHPSTORM_STUBS_DIR="cc/phpstorm-stubs"
VERSION_FILE="cc/version"

curl -L -o $ZIP_FILE $ZIP_URL
mkdir -p $EXTRACT_DIR
unzip -o $ZIP_FILE -d $EXTRACT_DIR
rm $ZIP_FILE

mv $EXTRACT_DIR/* $PHPSTORM_STUBS_DIR
rmdir $EXTRACT_DIR
# Delete meta files
find $PHPSTORM_STUBS_DIR -maxdepth 1 -type f -delete

# Save the commit hash
echo $COMMIT_HASH > $VERSION_FILE

# Update archive
rm ../../Runtime/PHP.zip
zip -r ../../Runtime/PHP.zip TestXDebugSettings.php cc/*

# Clean up
rm -rf $PHPSTORM_STUBS_DIR $VERSION_FILE

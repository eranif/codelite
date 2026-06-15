#!/bin/bash

set -e

# Check required environment variables
if [ -z "$APPLE_DEV_ID" ]; then
    echo "Error: APPLE_DEV_ID environment variable is not set"
    exit 1
fi

if [ -z "$APPLE_APP_PASSWORD" ]; then
    echo "Error: APPLE_APP_PASSWORD environment variable is not set"
    exit 1
fi

if [ -z "$APPLE_TEAM" ]; then
    echo "Error: APPLE_TEAM environment variable is not set"
    exit 1
fi

if [ -z "$APPLE_ID" ]; then
    echo "Error: APPLE_ID environment variable is not set"
    exit 1
fi

# Parse arguments
NOTARIZE=false
APP_PATH=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --notarize)
            NOTARIZE=true
            shift
            ;;
        *)
            APP_PATH="$1"
            shift
            ;;
    esac
done

# Check if app path is provided
if [ -z "$APP_PATH" ]; then
    echo "Error: App path argument is required"
    echo "Usage: $0 [--notarize] /path/to/YourApp.app"
    exit 1
fi

# Verify app exists
if [ ! -d "$APP_PATH" ]; then
    echo "Error: App path does not exist: $APP_PATH"
    exit 1
fi

# Get absolute path and change to app directory
APP_PATH=$(cd "$APP_PATH" && pwd)
APP_DIR=$(dirname "$APP_PATH")
APP_NAME=$(basename "$APP_PATH")

cd "$APP_DIR"

# Construct the full signing identity
SIGNING_ID="$APPLE_DEV_ID ($APPLE_TEAM)"

# Sign all nested dylibs and frameworks first with timestamps
echo "Signing nested binaries with timestamps..."
find "$APP_NAME" -type f \( -name "*.dylib" -o -name "*.framework" \) | while read -r binary; do
    codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$binary" 2>/dev/null || true
done

# Also sign any executable binaries in SharedSupport
find "$APP_NAME/Contents/SharedSupport" -type f -perm +111 ! -name "*.dylib" | while read -r binary; do
    codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$binary" 2>/dev/null || true
done

# Sign the main app
echo "Signing app: $APP_NAME"
codesign --deep --force --options runtime --sign "$SIGNING_ID" "$APP_NAME"

# Verify signature
echo "Verifying signature..."
codesign -v -v "$APP_NAME"

echo "✓ App signed successfully"

# Notarization if requested
if [ "$NOTARIZE" = true ]; then
    echo ""
    echo "Starting notarization process..."

    ZIP_FILE="$APP_NAME.zip"

    echo "Creating zip: $ZIP_FILE"
    ditto -c -k --sequesterRsrc "$APP_NAME" "$ZIP_FILE"

    # Submit for notarization
    echo "Submitting to Apple for notarization..."
    xcrun notarytool submit "$ZIP_FILE" \
        --apple-id "$APPLE_ID" \
        --password "$APPLE_APP_PASSWORD" \
        --team-id "$APPLE_TEAM"      \
        --wait

    # Pause to allow Apple's CloudKit sync to catch up
    echo "Waiting 20 seconds for Apple CloudKit ticket propagation..."
    sleep 20

    # Staple the notarization ticket with a retry loop if sync is slow
    echo "Stapling notarization ticket to app..."
    MAX_ATTEMPTS=3
    ATTEMPT=1
    until xcrun stapler staple "$APP_NAME" || [ $ATTEMPT -eq $MAX_ATTEMPTS ]; do
        echo "Staple failed. Retrying in 15 seconds (Attempt $ATTEMPT/$MAX_ATTEMPTS)..."
        sleep 15
        ((ATTEMPT++))
    done

    # Hard verification check before cleanup
    echo "Verifying local Gatekeeper evaluation state..."
    spctl --assess --verbose "$APP_NAME"

    echo "✓ Notarization completed and stapled"

    # Clean up zip file
    rm "$ZIP_FILE"
    echo "✓ Cleaned up zip file"
fi

# All operations completed successfully
echo ""
echo "=========================================="
echo "✓ All operations completed successfully!"
echo "=========================================="

# Create DMG installer (must be after notarization to preserve ticket)
echo ""
echo "Creating TGZ package..."

TGZ_FILE="$APP_NAME.tgz"
# Clean up any existing DMG
[ -f "$TGZ_FILE" ] && rm -f "$TGZ_FILE"

echo tar -czf "$TGZ_FILE" "$APP_NAME/"
tar -czf "$TGZ_FILE" "$APP_NAME/"

echo "$TGZ_FILE"

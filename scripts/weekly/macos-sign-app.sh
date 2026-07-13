#!/bin/bash

set -e

# Check required environment variables
if [ -z "$APPLE_DEV_ID" ]; then
  echo "Error: APPLE_DEV_ID environment variable is not set"
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
APPLE_APP_PASSWORD=""

while [[ $# -gt 0 ]]; do
  case $1 in
  --notarize)
    NOTARIZE=true
    shift
    ;;
  --password)
    APPLE_APP_PASSWORD="$2"
    shift 2
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
  echo "Usage: $0 [--notarize] [--password <password>] /path/to/YourApp.app"
  exit 1
fi

# Check if password is required for notarization
if [ "$NOTARIZE" = true ] && [ -z "$APPLE_APP_PASSWORD" ]; then
  echo "Error: --password is required when using --notarize"
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
APP_BASENAME=${APP_NAME%.app}
NOTARY_ZIP_FILE="$APP_BASENAME-notary.zip"

# Get macOS version and architecture for the final zip filename
OS_VERSION=$(sw_vers -productVersion)
ARCH=$(uname -m)
DIST_ZIP_FILE="$APP_BASENAME-macOS_${OS_VERSION}_${ARCH}.zip"

cd "$APP_DIR"

# Clean up any leftover notary zip files from previous runs
rm -f "$NOTARY_ZIP_FILE"

# Construct the full signing identity
SIGNING_ID="$APPLE_DEV_ID ($APPLE_TEAM)"

# Sign bottom-up: every nested Mach-O must be signed (hardened runtime +
# secure timestamp) BEFORE the enclosing bundle. --deep is deprecated for
# distribution and re-signs nested code without a timestamp, so it is not used.

# 1. Frameworks (bundles — sign the bundle, not the inner files).
echo "Signing frameworks..."
find "$APP_NAME" -type d -name "*.framework" | while read -r fw; do
  codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$fw"
done

# 2. Standalone dylibs / .so bundles.
echo "Signing dylibs..."
find "$APP_NAME" -type f \( -name "*.dylib" -o -name "*.so" \) | while read -r binary; do
  codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$binary"
done

# 2.1 Scripts
echo "Signing dylibs..."
find "$APP_NAME" -type f  -name "*.sh" | while read -r binary; do
  codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$binary"
done

# 3. Any other Mach-O executables (helpers, SharedSupport tools).
echo "Signing nested executables..."
while IFS= read -r -d '' binary; do
  # Only sign actual Mach-O files (skip shell scripts, etc.).
  if file "$binary" | grep -q "Mach-O"; then
    codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$binary"
  fi
done < <(find "$APP_NAME/Contents" -type f -perm +111 ! -name "*.dylib" ! -name "*.so" -print0)

# 4. Finally, the app bundle itself (outer signature) — with a timestamp.
echo "Signing app: $APP_NAME"
codesign --force --options runtime --timestamp --sign "$SIGNING_ID" "$APP_NAME"

# Verify signature (strict, matching Gatekeeper's assessment).
echo "Verifying signature..."
codesign --verify --deep --strict --verbose=2 "$APP_NAME"

echo "✓ App signed successfully"

# Notarization if requested
if [ "$NOTARIZE" = true ]; then
  echo ""
  echo "Starting notarization process..."

  rm -f "$NOTARY_ZIP_FILE"

  echo "Creating notarization zip: $NOTARY_ZIP_FILE"
  ditto -c -k --keepParent --sequesterRsrc "$APP_NAME" "$NOTARY_ZIP_FILE"

  # Submit for notarization
  echo "Submitting to Apple for notarization..."
  xcrun notarytool submit "$NOTARY_ZIP_FILE" \
    --apple-id "$APPLE_ID" \
    --password "$APPLE_APP_PASSWORD" \
    --team-id "$APPLE_TEAM" \
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

  echo "✓ Notarization completed and stapled"

  # Clean up temporary notarization zip file
  rm -f "$NOTARY_ZIP_FILE"
  echo "✓ Cleaned up temporary notarization zip file"
fi

# Verify app is ready for distribution
echo ""
echo "Verifying app is ready for distribution..."
if syspolicy_check distribution "$APP_NAME" 2>&1 | grep -q "passed all pre-distribution checks"; then
  echo "✓ App passed all pre-distribution checks and is ready for distribution"
else
  echo "Error: App failed pre-distribution checks"
  syspolicy_check distribution "$APP_NAME"
  exit 1
fi

# All operations completed successfully
echo ""
echo "=========================================="
echo "✓ All operations completed successfully!"
echo "=========================================="

# Create final ZIP package for distribution. Use ditto with --keepParent so the
# downloaded archive expands to codelite.app instead of a bare Contents/ tree.
echo ""
echo "Creating ZIP package..."

rm -f "$DIST_ZIP_FILE"

echo ditto -c -k --keepParent --sequesterRsrc "$APP_NAME" "$DIST_ZIP_FILE"
ditto -c -k --keepParent --sequesterRsrc "$APP_NAME" "$DIST_ZIP_FILE"

echo "$DIST_ZIP_FILE"

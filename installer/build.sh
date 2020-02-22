#!/bin/bash

set -e

BINARY_DIR="$1"
SIGN_IDENTITY="$2"

DIRNAME="$(dirname $0)"
TARGET_DIR="ROOT/tmp/QhyCfw3Usb_X2"
REVISION="$(git rev-list HEAD --count)"

mkdir -p ${TARGET_DIR}
cp "${DIRNAME}/../filterwheellist QHYCFW3.txt" "${TARGET_DIR}"
cp "${BINARY_DIR}/libqhycfw3_usb.dylib" "${TARGET_DIR}"
cp "${DIRNAME}/../qhycfw3_usb.ui" "${TARGET_DIR}"
pkgbuild \
  --root ROOT \
  --identifier com.scottdw2.QhyCfw3Usb_X2 \
  --sign "${SIGN_IDENTITY}" \
  --version 0.1.${REVISION} \
  --scripts ${DIRNAME}/scripts \
  --timestamp \
  ${BINARY_DIR}/qhycfw3_usb.pkg

pkgutil --check-signature ${BINARY_DIR}/qhycfw3_usb.pkg
rm -rf ROOT

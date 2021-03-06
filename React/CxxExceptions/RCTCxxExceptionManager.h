/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cxxreact/ExceptionManager.h>

namespace facebook {
namespace react {

/**
 * Connector class (from C++ to ObjC++) to allow FabricUIManager to invoke native UI operations/updates.
 * UIKit-related impl doesn't live here, but this class gets passed to the FabricUIManager C++ impl directly.
 */
class RCTCxxExceptionManager : public ExceptionManager {
public:

  virtual void handleSoftException(const std::exception& e) const override;
  virtual void handleFatalException(const std::exception& e) const override;
};

} // namespace react
} // namespace facebook

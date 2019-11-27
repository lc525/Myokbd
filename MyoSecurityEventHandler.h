/* ---------
 * Copyright 2019 Lucian Carata <lucian.carata@cl.cam.ac.uk>
 *
 * This file is part of the Myokbd open-source project: github.com/lc525/myokbd
 * Licensed under the terms of Apache license 2.0, see the LICENSE file at the
 * root of the project for details.
 * ---------
 */
#ifndef _MYO_SECURITY_EVENT_HANDLER_H_
#define _MYO_SECURITY_EVENT_HANDLER_H_

#include <ble/SecurityManager.h>

namespace myokbd {
  class MyoSecurityEventHandler : public ble::SecurityManager::EventHandler {
  };
}

#endif

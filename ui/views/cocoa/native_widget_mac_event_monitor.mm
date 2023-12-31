// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/cocoa/native_widget_mac_event_monitor.h"

#include "ui/views/cocoa/native_widget_mac_ns_window_host.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace views {

NativeWidgetMacEventMonitor::NativeWidgetMacEventMonitor(Client* client)
    : client_(client) {}

NativeWidgetMacEventMonitor::~NativeWidgetMacEventMonitor() = default;

}  // namespace views

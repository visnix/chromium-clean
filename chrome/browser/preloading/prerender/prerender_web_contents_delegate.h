// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PRELOADING_PRERENDER_PRERENDER_WEB_CONTENTS_DELEGATE_H_
#define CHROME_BROWSER_PRELOADING_PRERENDER_PRERENDER_WEB_CONTENTS_DELEGATE_H_

#include "content/public/browser/prerender_web_contents_delegate.h"

// This is an implementation of content::PrerenderWebContentsDelegate. See
// comments on content::PrerenderWebContentsDelegate for details.
class PrerenderWebContentsDelegateImpl
    : public content::PrerenderWebContentsDelegate {
 public:
  PrerenderWebContentsDelegateImpl() = default;
  ~PrerenderWebContentsDelegateImpl() override = default;

  // content::WebContentsDelegate overrides.
  content::PreloadingEligibility IsPrerender2Supported(
      content::WebContents& web_contents) override;
  void AddNewContents(content::WebContents* source,
                      std::unique_ptr<content::WebContents> new_contents,
                      const GURL& target_url,
                      WindowOpenDisposition disposition,
                      const blink::mojom::WindowFeatures& window_features,
                      bool user_gesture,
                      bool* was_blocked) override;
  void ActivateContents(content::WebContents* contents) override;
  bool ShouldSuppressDialogs(content::WebContents* source) override;
  void WebContentsCreated(content::WebContents* source_contents,
                          int opener_render_process_id,
                          int opener_render_frame_id,
                          const std::string& frame_name,
                          const GURL& target_url,
                          content::WebContents* new_contents) override;
  void PortalWebContentsCreated(
      content::WebContents* portal_web_contents) override;
  void WebContentsBecamePortal(
      content::WebContents* portal_web_contents) override;
  std::unique_ptr<content::WebContents> ActivatePortalWebContents(
      content::WebContents* predecessor_contents,
      std::unique_ptr<content::WebContents> portal_contents) override;
  void UpdateInspectedWebContentsIfNecessary(
      content::WebContents* old_contents,
      content::WebContents* new_contents,
      base::OnceCallback<void()> callback) override;

  // TODO(crbug.com/1350676): Investigate if we have to override other
  // functions on WebContentsDelegate.
};

#endif  // CHROME_BROWSER_PRELOADING_PRERENDER_PRERENDER_WEB_CONTENTS_DELEGATE_H_

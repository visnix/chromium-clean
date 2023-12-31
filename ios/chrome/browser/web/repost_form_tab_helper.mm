// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/web/repost_form_tab_helper.h"

#import "base/memory/ptr_util.h"
#import "ios/chrome/browser/web/repost_form_tab_helper_delegate.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

RepostFormTabHelper::RepostFormTabHelper(web::WebState* web_state)
    : web_state_(web_state) {
  web_state_->AddObserver(this);
}

RepostFormTabHelper::~RepostFormTabHelper() {
  DCHECK(!web_state_);
}

void RepostFormTabHelper::DismissReportFormDialog() {
  weak_factory_.InvalidateWeakPtrs();
  if (is_presenting_dialog_)
    [delegate_ repostFormTabHelperDismissRepostFormDialog:this];
  is_presenting_dialog_ = false;
}

void RepostFormTabHelper::OnDialogPresented() {
  DCHECK(is_presenting_dialog_);
  is_presenting_dialog_ = false;
}

void RepostFormTabHelper::PresentDialog(
    CGPoint location,
    base::OnceCallback<void(bool)> callback) {
  DCHECK(!is_presenting_dialog_);
  if (!delegate_) {
    // If there is is no delegate, then assume that we should not continue.
    std::move(callback).Run(/*should_continue*/ false);
    return;
  }

  is_presenting_dialog_ = true;
  base::OnceClosure on_dialog_presented = base::BindOnce(
      &RepostFormTabHelper::OnDialogPresented, weak_factory_.GetWeakPtr());

  callback = std::move(callback).Then(std::move(on_dialog_presented));

  [delegate_ repostFormTabHelper:this
      presentRepostFormDialogForWebState:web_state_
                           dialogAtPoint:location
                       completionHandler:base::CallbackToBlock(
                                             std::move(callback))];
}

void RepostFormTabHelper::SetDelegate(
    id<RepostFormTabHelperDelegate> delegate) {
  delegate_ = delegate;
}

void RepostFormTabHelper::DidStartNavigation(web::WebState* web_state,
                                             web::NavigationContext*) {
  DCHECK_EQ(web_state_, web_state);
  DismissReportFormDialog();
}

void RepostFormTabHelper::WebStateDestroyed(web::WebState* web_state) {
  DCHECK_EQ(web_state_, web_state);
  DismissReportFormDialog();

  web_state_->RemoveObserver(this);
  web_state_ = nullptr;
}

WEB_STATE_USER_DATA_KEY_IMPL(RepostFormTabHelper)

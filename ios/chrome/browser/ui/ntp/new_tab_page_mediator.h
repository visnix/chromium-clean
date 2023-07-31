// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_NTP_NEW_TAB_PAGE_MEDIATOR_H_
#define IOS_CHROME_BROWSER_UI_NTP_NEW_TAB_PAGE_MEDIATOR_H_

#import <UIKit/UIKit.h>

#import "ios/chrome/browser/ui/ntp/feed_management/feed_management_navigation_delegate.h"

namespace signin {
class IdentityManager;
}  // namespace signin

namespace web {
class WebState;
}  // namespace web

class AuthenticationService;
class ChromeAccountManagerService;
@class ContentSuggestionsMediator;
class DiscoverFeedService;
@protocol FeedControlDelegate;
@class FeedMetricsRecorder;
class GURL;
@protocol LogoVendor;
@protocol NewTabPageConsumer;
@protocol NewTabPageHeaderConsumer;
class TemplateURLService;
class UrlLoadingBrowserAgent;
@protocol UserAccountImageUpdateDelegate;

// Mediator for the NTP Home panel, handling the interactions with the
// suggestions.
@interface NewTabPageMediator : NSObject <FeedManagementNavigationDelegate>

- (instancetype)
            initWithWebState:(web::WebState*)webState
          templateURLService:(TemplateURLService*)templateURLService
                   URLLoader:(UrlLoadingBrowserAgent*)URLLoader
                 authService:(AuthenticationService*)authService
             identityManager:(signin::IdentityManager*)identityManager
       accountManagerService:(ChromeAccountManagerService*)accountManagerService
                  logoVendor:(id<LogoVendor>)logoVendor
    identityDiscImageUpdater:(id<UserAccountImageUpdateDelegate>)imageUpdater
                 isIncognito:(BOOL)isIncognito
         discoverFeedService:(DiscoverFeedService*)discoverFeedService
    NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;

// Recorder for the metrics related to the feed.
@property(nonatomic, strong) FeedMetricsRecorder* feedMetricsRecorder;
// Mediator for the ContentSuggestions.
// TODO(crbug.com/1403298): Replace this dependency with a delegate.
@property(nonatomic, weak) ContentSuggestionsMediator* suggestionsMediator;
// Consumer for this mediator.
@property(nonatomic, weak) id<NewTabPageConsumer> consumer;
// Consumer for NTP header model updates.
@property(nonatomic, weak) id<NewTabPageHeaderConsumer> headerConsumer;
// Delegate for controlling the current feed.
@property(nonatomic, weak) id<FeedControlDelegate> feedControlDelegate;
// The web state associated with this NTP.
@property(nonatomic, assign) web::WebState* webState;

// Inits the mediator.
- (void)setUp;

// Cleans the mediator.
- (void)shutdown;

// Save the NTP scroll offset into the last committed navigation item for the
// before navigating away.
- (void)saveContentOffsetForWebState:(web::WebState*)webState;

// Handles the actions following a tap on the "Learn More" item in the Discover
// feed menu.
- (void)handleFeedLearnMoreTapped;

@end

#endif  // IOS_CHROME_BROWSER_UI_NTP_NEW_TAB_PAGE_MEDIATOR_H_

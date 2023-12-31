// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/authentication/history_sync/history_sync_mediator.h"

#import "base/check.h"
#import "base/check_op.h"
#import "components/signin/public/identity_manager/objc/identity_manager_observer_bridge.h"
#import "components/sync/service/sync_service.h"
#import "components/sync/service/sync_user_settings.h"
#import "components/unified_consent/unified_consent_service.h"
#import "ios/chrome/browser/signin/authentication_service.h"
#import "ios/chrome/browser/signin/chrome_account_manager_service.h"
#import "ios/chrome/browser/ui/authentication/history_sync/history_sync_consumer.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@interface HistorySyncMediator () <IdentityManagerObserverBridgeDelegate>
@end

@implementation HistorySyncMediator {
  AuthenticationService* _authenticationService;
  ChromeAccountManagerService* _accountManagerService;
  // Observer for `IdentityManager`.
  std::unique_ptr<signin::IdentityManagerObserverBridge>
      _identityManagerObserver;
  // Sync service.
  syncer::SyncService* _syncService;
}

- (instancetype)
    initWithAuthenticationService:(AuthenticationService*)authenticationService
      chromeAccountManagerService:
          (ChromeAccountManagerService*)chromeAccountManagerService
                  identityManager:(signin::IdentityManager*)identityManager
                      syncService:(syncer::SyncService*)syncService {
  self = [super init];
  if (self) {
    _authenticationService = authenticationService;
    _accountManagerService = chromeAccountManagerService;
    _identityManagerObserver =
        std::make_unique<signin::IdentityManagerObserverBridge>(identityManager,
                                                                self);
    _syncService = syncService;
  }
  return self;
}

- (void)disconnect {
  _identityManagerObserver.reset();
  _authenticationService = nullptr;
  _accountManagerService = nullptr;
  _syncService = nullptr;
}

- (void)enableHistorySyncOptin {
  id<SystemIdentity> identity =
      _authenticationService->GetPrimaryIdentity(signin::ConsentLevel::kSignin);
  CHECK(identity);
  // TODO(crbug.com/1467853): Record the history sync opt-in when the new
  // consent type will be available.
  syncer::SyncUserSettings* syncUserSettings = _syncService->GetUserSettings();
  syncUserSettings->SetSelectedType(syncer::UserSelectableType::kHistory, true);
  syncUserSettings->SetSelectedType(syncer::UserSelectableType::kTabs, true);
}

#pragma mark - Properties

- (void)setConsumer:(id<HistorySyncConsumer>)consumer {
  _consumer = consumer;
  if (!_consumer) {
    return;
  }
  id<SystemIdentity> identity =
      _authenticationService->GetPrimaryIdentity(signin::ConsentLevel::kSignin);
  if (!identity) {
    // This can happen if identity is removed from the device while the history
    // sync screen is open. There is no point in updating the UI since the
    // dialog will be automatically closed.
    return;
  }
  [self updateAvatarImageWithIdentity:identity];
}

#pragma mark - IdentityManagerObserverBridgeDelegate

- (void)onPrimaryAccountChanged:
    (const signin::PrimaryAccountChangeEvent&)event {
  if (event.GetEventTypeFor(signin::ConsentLevel::kSignin) ==
      signin::PrimaryAccountChangeEvent::Type::kCleared) {
    [self.delegate historySyncMediatorPrimaryAccountCleared:self];
  }
}

#pragma mark - Private

// Updates the avatar image for the consumer from `identity`.
- (void)updateAvatarImageWithIdentity:(id<SystemIdentity>)identity {
  UIImage* image = _accountManagerService->GetIdentityAvatarWithIdentity(
      identity, IdentityAvatarSize::Large);
  [self.consumer setPrimaryIdentityAvatarImage:image];

  NSString* accessibilityLabel = nil;
  if (identity.userFullName.length == 0) {
    accessibilityLabel = identity.userEmail;
  } else {
    accessibilityLabel = [NSString
        stringWithFormat:@"%@ %@", identity.userFullName, identity.userEmail];
  }
  [self.consumer setPrimaryIdentityAvatarAccessibilityLabel:accessibilityLabel];
}

@end

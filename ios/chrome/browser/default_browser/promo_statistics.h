// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_DEFAULT_BROWSER_PROMO_STATISTICS_H_
#define IOS_CHROME_BROWSER_DEFAULT_BROWSER_PROMO_STATISTICS_H_

#import <UIKit/UIKit.h>

// Represents a `PromoStatistics`.
@interface PromoStatistics : NSObject

// Number of promos user has already seen.
@property(nonatomic, assign) int promoDisplayCount;
// Number of days since user last interacted with a promo.
@property(nonatomic, assign) int numDaysSinceLastPromo;
// Number of cold starts in the last `kTriggerCriteriaExperimentStatExpiration`
// days.
@property(nonatomic, assign) int chromeColdStartCount;
// Number of warm starts in the last `kTriggerCriteriaExperimentStatExpiration`
// days.
@property(nonatomic, assign) int chromeWarmStartCount;
// Number of indirect starts in the last
// `kTriggerCriteriaExperimentStatExpiration` days.
@property(nonatomic, assign) int chromeIndirectStartCount;
// Number of active days in the last `kTriggerCriteriaExperimentStatExpiration`
// days.
@property(nonatomic, assign) int activeDayCount;
// Number of times password manager features where used in the last
// `kTriggerCriteriaExperimentStatExpiration` days.
@property(nonatomic, assign) int passwordManagerUseCount;
// Number of times user copy-pasted in omnibox in the last
// `kTriggerCriteriaExperimentStatExpiration` days.
@property(nonatomic, assign) int omniboxClipboardUseCount;

// TODO(crbug.com/1456438): Implement rest of the metrics:
// OmniboxClipboardUseCount
// BookmarkUseCount
// AutofillUseCount
// SpecialTabsUseCount

@end

#endif  // IOS_CHROME_BROWSER_DEFAULT_BROWSER_PROMO_STATISTICS_H_

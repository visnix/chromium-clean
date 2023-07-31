// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_SEARCH_ENGINE_CHOICE_SEARCH_ENGINE_CHOICE_SERVICE_H_
#define CHROME_BROWSER_SEARCH_ENGINE_CHOICE_SEARCH_ENGINE_CHOICE_SERVICE_H_

#include "base/containers/flat_set.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_observation.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_list_observer.h"
#include "components/keyed_service/core/keyed_service.h"

class Browser;
class BrowserListObserver;

// Service handling the Search Engine Choice dialog.
class SearchEngineChoiceService : public KeyedService {
 public:
  SearchEngineChoiceService();
  ~SearchEngineChoiceService() override;

  // Informs the service that a Search Engine Choice dialog has been opened
  // for `browser`.
  // Virtual to be able to mock in tests.
  virtual void NotifyDialogOpened(Browser* browser);

  // Informs the service that a Search Engine Choice dialog has been closed for
  // `browser`.
  void NotifyDialogClosed(Browser* browser);

  // Returns whether a Search Engine Choice dialog is currently open or not for
  // `browser`.
  bool IsShowingDialog(Browser* browser);

  // Returns whether the Search Engine Choice dialog should be displayed or not.
  static bool ShouldDisplayDialog(Browser& browser);

 private:
  // Observes the BrowserList to make sure that closed browsers are correctly
  // removed from our set of browser pointers. This ensures that we don't get
  // dangling pointers.
  class BrowserObserver : public BrowserListObserver {
   public:
    explicit BrowserObserver(SearchEngineChoiceService& service);
    ~BrowserObserver() override;

    void OnBrowserRemoved(Browser* browser) override;

   private:
    base::raw_ref<SearchEngineChoiceService> search_engine_choice_service_;
    base::ScopedObservation<BrowserList, BrowserListObserver> observation_{
        this};
  };
  friend class SearchEngineChoiceServiceFactory;

  // The set of Browser windows which have an open Search Engine Choice dialog.
  base::flat_set<Browser*> browsers_with_open_dialogs_;

  // Observes the browser list for closed browsers.
  BrowserObserver browser_observer_{*this};
  base::WeakPtrFactory<SearchEngineChoiceService> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_SEARCH_ENGINE_CHOICE_SEARCH_ENGINE_CHOICE_SERVICE_H

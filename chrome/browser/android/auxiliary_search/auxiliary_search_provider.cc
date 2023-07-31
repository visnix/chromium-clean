// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/android/auxiliary_search/auxiliary_search_provider.h"

#include "base/android/jni_android.h"
#include "base/android/jni_array.h"
#include "base/memory/singleton.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/android/chrome_jni_headers/AuxiliarySearchBridge_jni.h"
#include "chrome/browser/android/auxiliary_search/proto/auxiliary_search_group.pb.h"
#include "chrome/browser/bookmarks/bookmark_model_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_android.h"
#include "chrome/browser/profiles/profile_keyed_service_factory.h"
#include "components/bookmarks/browser/bookmark_model.h"
#include "components/bookmarks/browser/bookmark_node.h"
#include "components/bookmarks/browser/bookmark_utils.h"

using base::android::ToJavaByteArray;
using bookmarks::BookmarkModel;
using bookmarks::BookmarkNode;

namespace {

const size_t kMaxBookmarksCount = 100u;

class AuxiliarySearchProviderFactory : public ProfileKeyedServiceFactory {
 public:
  static AuxiliarySearchProvider* GetForProfile(Profile* profile) {
    return static_cast<AuxiliarySearchProvider*>(
        GetInstance()->GetServiceForBrowserContext(profile, true));
  }

  static AuxiliarySearchProviderFactory* GetInstance() {
    return base::Singleton<AuxiliarySearchProviderFactory>::get();
  }

  AuxiliarySearchProviderFactory()
      : ProfileKeyedServiceFactory(
            "AuxiliarySearchProvider",
            ProfileSelections::Builder()
                .WithRegular(ProfileSelection::kRedirectedToOriginal)
                .WithGuest(ProfileSelection::kNone)
                .Build()) {}

 private:
  // ProfileKeyedServiceFactory overrides
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override {
    Profile* profile = Profile::FromBrowserContext(context);
    DCHECK(!profile->IsOffTheRecord());

    return new AuxiliarySearchProvider(profile);
  }
};

}  // namespace

AuxiliarySearchProvider::AuxiliarySearchProvider(Profile* profile)
    : profile_(profile) {}

AuxiliarySearchProvider::~AuxiliarySearchProvider() = default;

base::android::ScopedJavaLocalRef<jbyteArray>
AuxiliarySearchProvider::GetBookmarksSearchableData(JNIEnv* env) const {
  auxiliary_search::AuxiliarySearchBookmarkGroup group =
      GetBookmarks(BookmarkModelFactory::GetForBrowserContext(profile_.get()));

  std::string serialized_group;
  if (!group.SerializeToString(&serialized_group)) {
    serialized_group.clear();
  }

  return ToJavaByteArray(env, serialized_group);
}

base::android::ScopedJavaLocalRef<jbyteArray>
AuxiliarySearchProvider::GetTabsSearchableData(JNIEnv* env) const {
  auxiliary_search::AuxiliarySearchTabGroup group = GetTabs();

  std::string serialized_group;
  if (!group.SerializeToString(&serialized_group)) {
    serialized_group.clear();
  }

  return ToJavaByteArray(env, serialized_group);
}

auxiliary_search::AuxiliarySearchBookmarkGroup
AuxiliarySearchProvider::GetBookmarks(bookmarks::BookmarkModel* model) const {
  auxiliary_search::AuxiliarySearchBookmarkGroup group;
  std::vector<const BookmarkNode*> nodes;
  bookmarks::GetMostRecentlyUsedEntries(model, kMaxBookmarksCount, &nodes);
  for (const BookmarkNode* node : nodes) {
    auxiliary_search::AuxiliarySearchEntry* bookmark = group.add_bookmark();
    bookmark->set_title(base::UTF16ToUTF8(node->GetTitle()));
    bookmark->set_url(node->url().spec());
    if (!node->date_added().is_null()) {
      bookmark->set_creation_timestamp(node->date_added().ToJavaTime());
    }
    if (!node->date_last_used().is_null()) {
      bookmark->set_last_access_timestamp(node->date_last_used().ToJavaTime());
    }
  }

  return group;
}

auxiliary_search::AuxiliarySearchTabGroup AuxiliarySearchProvider::GetTabs()
    const {
  auxiliary_search::AuxiliarySearchTabGroup group;
  // TODO(crbug.com/1462378): Add implementation for reading tabs info from
  // PersistedTabData.

  return group;
}

// static
jlong JNI_AuxiliarySearchBridge_GetForProfile(
    JNIEnv* env,
    const base::android::JavaParamRef<jobject>& j_profile) {
  Profile* profile = ProfileAndroid::FromProfileAndroid(j_profile);
  DCHECK(profile);

  return reinterpret_cast<intptr_t>(
      AuxiliarySearchProviderFactory::GetForProfile(profile));
}

// static
void AuxiliarySearchProvider::EnsureFactoryBuilt() {
  AuxiliarySearchProviderFactory::GetInstance();
}

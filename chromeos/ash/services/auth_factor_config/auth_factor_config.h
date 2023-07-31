// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_ASH_SERVICES_AUTH_FACTOR_CONFIG_AUTH_FACTOR_CONFIG_H_
#define CHROMEOS_ASH_SERVICES_AUTH_FACTOR_CONFIG_AUTH_FACTOR_CONFIG_H_

#include "base/containers/enum_set.h"
#include "chromeos/ash/components/login/auth/auth_factor_editor.h"
#include "chromeos/ash/components/login/auth/public/authentication_error.h"
#include "chromeos/ash/services/auth_factor_config/chrome_browser_delegates.h"
#include "chromeos/ash/services/auth_factor_config/public/mojom/auth_factor_config.mojom.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"

#include "components/prefs/pref_registry_simple.h"
#include "components/user_manager/user.h"

namespace ash::auth {

// The implementation of the AuthFactorConfig service.
class AuthFactorConfig : public mojom::AuthFactorConfig {
 public:
  using AuthFactorSet = base::EnumSet<mojom::AuthFactor,
                                      mojom::AuthFactor::kMinValue,
                                      mojom::AuthFactor::kMaxValue>;

  explicit AuthFactorConfig(QuickUnlockStorageDelegate*);
  ~AuthFactorConfig() override;

  AuthFactorConfig(const AuthFactorConfig&) = delete;
  AuthFactorConfig& operator=(const AuthFactorConfig&) = delete;

  static void RegisterPrefs(PrefRegistrySimple* registry);

  void BindReceiver(mojo::PendingReceiver<mojom::AuthFactorConfig> receiver);

  void ObserveFactorChanges(
      mojo::PendingRemote<mojom::FactorObserver>) override;
  void IsSupported(const std::string& auth_token,
                   mojom::AuthFactor factor,
                   base::OnceCallback<void(bool)>) override;
  void IsConfigured(const std::string& auth_token,
                    mojom::AuthFactor factor,
                    base::OnceCallback<void(bool)>) override;
  void GetManagementType(
      const std::string& auth_token,
      mojom::AuthFactor factor,
      base::OnceCallback<void(mojom::ManagementType)>) override;
  void IsEditable(const std::string& auth_token,
                  mojom::AuthFactor factor,
                  base::OnceCallback<void(bool)>) override;

  // Reload auth factor data from cryptohome and notify factor change observers
  // of the change. This method must be called after successful mutating
  // UserDataAuth calls so that the list of auth factors remains up to date.
  // `context` should be a copy of the user context stored in quick unlock
  // storage. In particular, `context` should contain an authenticated auth
  // session
  void NotifyFactorObserversAfterSuccess(
      AuthFactorSet changed_factor,
      std::unique_ptr<UserContext> context,
      base::OnceCallback<void(mojom::ConfigureResult)> callback);

  // Like NotifyFactorObserversAfterSuccess, but supposed to be called before
  // we return a `kFatalError` result because of a failed mutating UserDataAuth
  // call. This method will reload auth factors and send a change notification
  // to observers for all auth factors.
  // This is useful because a likely source of errors is outdated information
  // about the status of configured auth factors, resulting in an invalid
  // UserDataAuth call. For example, we might think that an auth factor is
  // configured and try to update it. If some other system has removed this
  // auth factor without our knowledge, the update call will fail. By
  // refreshing our information on what auth factors are configured, we can
  // recover so that the user can try again.
  void NotifyFactorObserversAfterFailure(std::unique_ptr<UserContext> context,
                                         base::OnceCallback<void()> callback);

 private:
  void OnGetAuthFactorsConfiguration(
      AuthFactorSet changed_factors,
      base::OnceCallback<void(mojom::ConfigureResult)> callback,
      std::unique_ptr<UserContext> context,
      absl::optional<AuthenticationError> error);

  raw_ptr<QuickUnlockStorageDelegate> quick_unlock_storage_;
  mojo::ReceiverSet<mojom::AuthFactorConfig> receivers_;
  mojo::RemoteSet<mojom::FactorObserver> observers_;
  AuthFactorEditor auth_factor_editor_;
  base::WeakPtrFactory<AuthFactorConfig> weak_factory_{this};
};

}  // namespace ash::auth

#endif  // CHROMEOS_ASH_SERVICES_AUTH_FACTOR_CONFIG_AUTH_FACTOR_CONFIG_H_

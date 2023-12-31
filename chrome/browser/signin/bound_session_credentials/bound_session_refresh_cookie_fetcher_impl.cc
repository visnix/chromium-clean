// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/signin/bound_session_credentials/bound_session_refresh_cookie_fetcher_impl.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/location.h"
#include "base/time/time.h"
#include "components/signin/public/base/wait_for_network_callback_helper.h"
#include "google_apis/gaia/gaia_urls.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "net/cookies/canonical_cookie.h"
#include "net/http/http_status_code.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/cookie_access_observer.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace {
constexpr char kRotationChallengeResponseHeader[] =
    "Sec-Session-Google-Response";

bool IsExpectedCookie(
    const GURL& url,
    const std::string& cookie_name,
    const network::mojom::CookieOrLineWithAccessResultPtr& cookie_ptr) {
  if (cookie_ptr->access_result.status.IsInclude()) {
    CHECK(cookie_ptr->cookie_or_line->is_cookie());
    const net::CanonicalCookie& cookie =
        cookie_ptr->cookie_or_line->get_cookie();
    return (cookie.Name() == cookie_name) && cookie.IsDomainMatch(url.host());
  }
  return false;
}
}  // namespace

BoundSessionRefreshCookieFetcherImpl::BoundSessionRefreshCookieFetcherImpl(
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
    WaitForNetworkCallbackHelper& wait_for_network_callback_helper,
    const GURL& cookie_url,
    base::flat_set<std::string> cookie_names)
    : url_loader_factory_(std::move(url_loader_factory)),
      wait_for_network_callback_helper_(wait_for_network_callback_helper),
      expected_cookie_domain_(cookie_url),
      expected_cookie_names_(std::move(cookie_names)) {}

BoundSessionRefreshCookieFetcherImpl::~BoundSessionRefreshCookieFetcherImpl() =
    default;

void BoundSessionRefreshCookieFetcherImpl::Start(
    RefreshCookieCompleteCallback callback) {
  CHECK(!callback_);
  CHECK(callback);
  callback_ = std::move(callback);
  wait_for_network_callback_helper_->DelayNetworkCall(
      base::BindOnce(&BoundSessionRefreshCookieFetcherImpl::StartRefreshRequest,
                     weak_ptr_factory_.GetWeakPtr()));
}

void BoundSessionRefreshCookieFetcherImpl::StartRefreshRequest() {
  // TODO(b/273920907): Update the `traffic_annotation` setting once a mechanism
  // allowing the user to disable the feature is implemented.
  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("gaia_auth_rotate_bound_cookies",
                                          R"(
        semantics {
          sender: "Chrome - Google authentication API"
          description:
            "This request is used to rotate bound Google authentication "
            "cookies."
          trigger:
            "This request is triggered in a bound session when the bound Google"
            " authentication cookies are soon to expire."
          user_data {
            type: ACCESS_TOKEN
          }
          data: "Request includes cookies and a signed token proving that a"
                " request comes from the same device as was registered before."
          destination: GOOGLE_OWNED_SERVICE
          internal {
            contacts {
                email: "chrome-signin-team@google.com"
            }
          }
          last_reviewed: "2023-05-09"
        }
        policy {
          cookies_allowed: YES
          cookies_store: "user"
          setting:
            "This is a new feature being developed behind a flag that is"
            " disabled by default (kEnableBoundSessionCredentials). This"
            " request will only be sent if the feature is enabled and once"
            " a server requests it with a special header."

          policy_exception_justification:
            "Not implemented. "
            "If the feature is on, this request must be made to ensure the user"
            " maintains their signed in status on the web for Google owned"
            " domains."
        })");

  auto request = std::make_unique<network::ResourceRequest>();
  request->url = GaiaUrls::GetInstance()->rotate_bound_cookies_url();
  request->method = "GET";

  // TODO(b/284956553): Properly sign the challenge and attach it to the
  // request. This temporary to unblock local testing as the rotation endpoint
  // requires the presence of this header to issue required cookies.
  request->headers.SetHeader(kRotationChallengeResponseHeader,
                             "fakeChallengeResponse");

  url::Origin origin = GaiaUrls::GetInstance()->gaia_origin();
  request->site_for_cookies = net::SiteForCookies::FromOrigin(origin);
  request->trusted_params = network::ResourceRequest::TrustedParams();
  request->trusted_params->isolation_info =
      net::IsolationInfo::CreateForInternalRequest(origin);

  mojo::PendingRemote<network::mojom::CookieAccessObserver> remote;
  cookie_observers_.Add(this, remote.InitWithNewPipeAndPassReceiver());
  request->trusted_params->cookie_observer = std::move(remote);

  // TODO(b/273920907): Figure out how to handle redirects. Currently
  // `network::SimpleURLLoader::SetOnRedirectCallback()` doesn't support
  // modifying the headers nor asynchronously resuming the reguest.
  url_loader_ =
      network::SimpleURLLoader::Create(std::move(request), traffic_annotation);
  url_loader_->SetRetryOptions(
      3, network::SimpleURLLoader::RETRY_ON_NETWORK_CHANGE);
  // TODO(b/273920907): Download the response body to support in refresh DBSC
  // instructions update.
  // `base::Unretained(this)` is safe as `this` owns `url_loader_`.
  url_loader_->DownloadHeadersOnly(
      url_loader_factory_.get(),
      base::BindOnce(&BoundSessionRefreshCookieFetcherImpl::OnURLLoaderComplete,
                     base::Unretained(this)));
}

void BoundSessionRefreshCookieFetcherImpl::OnURLLoaderComplete(
    scoped_refptr<net::HttpResponseHeaders> headers) {
  url_loader_completed_ = true;
  net::Error net_error = static_cast<net::Error>(url_loader_->NetError());

  result_ = GetResultFromNetErrorAndHttpStatusCode(
      net_error,
      headers ? absl::optional<int>(headers->response_code()) : absl::nullopt);

  if (result_ == Result::kSuccess && !reported_cookies_notified_) {
    // Normally, a cookie update notification should be sent before the request
    // is complete. Add some leeway in the case mojo messages are delivered out
    // of order.
    const base::TimeDelta kResponseCookiesNotifiedMaxDelay =
        base::Milliseconds(100);
    // `base::Unretained` is safe as `this` owns
    // `reported_cookies_notified_timer_`.
    reported_cookies_notified_timer_.Start(
        FROM_HERE, kResponseCookiesNotifiedMaxDelay,
        base::BindOnce(
            &BoundSessionRefreshCookieFetcherImpl::ReportRefreshResult,
            base::Unretained(this)));
  } else {
    ReportRefreshResult();
  }
}

BoundSessionRefreshCookieFetcher::Result
BoundSessionRefreshCookieFetcherImpl::GetResultFromNetErrorAndHttpStatusCode(
    net::Error net_error,
    absl::optional<int> response_code) {
  if ((net_error != net::OK &&
       net_error != net::ERR_HTTP_RESPONSE_CODE_FAILURE) ||
      !response_code) {
    return Result::kConnectionError;
  }

  if (response_code == net::HTTP_OK) {
    return Result::kSuccess;
  }

  if (response_code >= net::HTTP_INTERNAL_SERVER_ERROR) {
    // Server error 5xx.
    return Result::kServerTransientError;
  }

  if (response_code >= net::HTTP_BAD_REQUEST) {
    // Server error 4xx.
    return Result::kServerPersistentError;
  }

  // Unexpected response code.
  return Result::kServerPersistentError;
}

void BoundSessionRefreshCookieFetcherImpl::ReportRefreshResult() {
  reported_cookies_notified_timer_.Stop();
  CHECK(url_loader_completed_);
  if (result_ == Result::kSuccess && !expected_cookies_set_) {
    result_ = Result::kServerUnexepectedResponse;
  }

  std::move(callback_).Run(result_);
}

void BoundSessionRefreshCookieFetcherImpl::OnCookiesAccessed(
    std::vector<network::mojom::CookieAccessDetailsPtr> details_vector) {
  for (const auto& cookie_details : details_vector) {
    if (cookie_details->type !=
        network::mojom::CookieAccessDetails::Type::kChange) {
      continue;
    }

    reported_cookies_notified_ = true;
    bool all_cookies_set = true;
    for (const std::string& expected_cookie_name : expected_cookie_names_) {
      auto it = base::ranges::find_if(
          cookie_details->cookie_list,
          [this, &expected_cookie_name](
              const network::mojom::CookieOrLineWithAccessResultPtr& cookie) {
            return IsExpectedCookie(expected_cookie_domain_,
                                    expected_cookie_name, cookie);
          });
      if (it == cookie_details->cookie_list.end()) {
        all_cookies_set = false;
        break;
      }
    }
    expected_cookies_set_ = expected_cookies_set_ || all_cookies_set;
  }

  if (url_loader_completed_ && reported_cookies_notified_) {
    ReportRefreshResult();
  }
}

void BoundSessionRefreshCookieFetcherImpl::Clone(
    mojo::PendingReceiver<network::mojom::CookieAccessObserver> observer) {
  cookie_observers_.Add(this, std::move(observer));
}

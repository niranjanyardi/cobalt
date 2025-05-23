// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_KCER_NSS_KCER_TOKEN_IMPL_NSS_H_
#define CHROME_BROWSER_CHROMEOS_KCER_NSS_KCER_TOKEN_IMPL_NSS_H_

#include <stdint.h>

#include <queue>
#include <string>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "chrome/browser/chromeos/kcer_nss/cert_cache_nss.h"
#include "chromeos/components/kcer/kcer_token.h"
#include "crypto/scoped_nss_types.h"
#include "net/cert/cert_database.h"
#include "net/cert/scoped_nss_types.h"
#include "net/cert/x509_certificate.h"

namespace kcer::internal {

// Implementation of KcerToken that uses NSS as a permanent storage.
class KcerTokenImplNss : public KcerToken, public net::CertDatabase::Observer {
 public:
  enum class State {
    // Cache must be updated before it can be used.
    kCacheOutdated,
    // Cache is currently being updated.
    kCacheUpdating,
    // Cache is up-to-date and can be used.
    kCacheUpToDate,
    // Terminal state, initialization failed.
    kInitializationFailed,
  };

  explicit KcerTokenImplNss(Token token);
  ~KcerTokenImplNss() override;

  KcerTokenImplNss(const KcerTokenImplNss&) = delete;
  KcerTokenImplNss& operator=(const KcerTokenImplNss&) = delete;
  KcerTokenImplNss(KcerTokenImplNss&&) = delete;
  KcerTokenImplNss& operator=(KcerTokenImplNss&&) = delete;

  // Returns a weak pointer for the token. The pointer can be used to post tasks
  // for the token.
  base::WeakPtr<KcerTokenImplNss> GetWeakPtr();

  // Initializes the token with the provided NSS slot. If `nss_slot` is nullptr,
  // the initialization is considered failed and the token will return an error
  // for all queued and future requests.
  void Initialize(crypto::ScopedPK11Slot nss_slot);

  // Implements net::CertDatabase::Observer.
  void OnCertDBChanged() override;

  // Implements KcerToken.
  void GenerateRsaKey(uint32_t modulus_length_bits,
                      bool hardware_backed,
                      Kcer::GenerateKeyCallback callback) override;
  void GenerateEcKey(EllipticCurve curve,
                     bool hardware_backed,
                     Kcer::GenerateKeyCallback callback) override;
  void ImportKey(Pkcs8PrivateKeyInfoDer pkcs8_private_key_info_der,
                 Kcer::ImportKeyCallback callback) override;
  void ImportCertFromBytes(CertDer cert_der,
                           Kcer::StatusCallback callback) override;
  void ImportPkcs12Cert(Pkcs12Blob pkcs12_blob,
                        std::string password,
                        bool hardware_backed,
                        Kcer::StatusCallback callback) override;
  void ExportPkcs12Cert(scoped_refptr<const Cert> cert,
                        Kcer::ExportPkcs12Callback callback) override;
  void RemoveKeyAndCerts(PrivateKeyHandle key,
                         Kcer::StatusCallback callback) override;
  void RemoveCert(scoped_refptr<const Cert> cert,
                  Kcer::StatusCallback callback) override;
  void ListKeys(TokenListKeysCallback callback) override;
  void ListCerts(TokenListCertsCallback callback) override;
  void DoesPrivateKeyExist(PrivateKeyHandle key,
                           Kcer::DoesKeyExistCallback callback) override;
  void Sign(PrivateKeyHandle key,
            SigningScheme signing_scheme,
            DataToSign data,
            Kcer::SignCallback callback) override;
  void SignRsaPkcs1Raw(PrivateKeyHandle key,
                       SigningScheme signing_scheme,
                       DigestWithPrefix digest_with_prefix,
                       Kcer::SignCallback callback) override;
  void GetTokenInfo(Kcer::GetTokenInfoCallback callback) override;
  void GetKeyInfo(PrivateKeyHandle key,
                  Kcer::GetKeyInfoCallback callback) override;
  void SetKeyNickname(PrivateKeyHandle key,
                      std::string nickname,
                      Kcer::StatusCallback callback) override;
  void SetKeyPermissions(PrivateKeyHandle key,
                         chaps::KeyPermissions key_permissions,
                         Kcer::StatusCallback callback) override;
  void SetCertProvisioningProfileId(PrivateKeyHandle key,
                                    std::string profile_id,
                                    Kcer::StatusCallback callback) override;

 private:
  // Immediately blocks the queue and returns a closure that unblocks it when
  // called or destroyed.
  base::OnceClosure BlockQueueGetUnblocker();
  // Immediately unblocks the queue and attempts to perform the next task.
  void UnblockQueueProcessNextTask();

  // Updates the cached certificates to match the ones in NSS.
  void UpdateCache();
  void UpdateCacheWithCerts(net::ScopedCERTCertificateList certs);

  // Convenience method for calling the callback with the
  // kTokenInitializationFailed error and scheduling the next task.
  template <typename T>
  void HandleInitializationFailed(
      base::OnceCallback<void(base::expected<T, Error>)> callback);

  // Sends a notification about changed certs (if needed) and forwards the
  // result.
  void OnCertsModified(Kcer::StatusCallback callback,
                       base::expected<void, Error> result);

  // Indicates whether the task queue is blocked. Task queue should be blocked
  // until NSS is initialized, during the processing of most requests and
  // during updating the cache.
  bool is_blocked_ = true;
  State state_ = State::kCacheOutdated;
  // Token type of this KcerToken.
  const Token token_;
  // The underlying storage for KcerTokenNss. In this context the words "token"
  // and "slot" are synonyms.
  crypto::ScopedPK11Slot slot_;
  // Queue for the tasks that were received while the tast queue was blocked.
  std::queue<base::OnceClosure> task_queue_;
  // Cache for certificates.
  CertCacheNss cert_cache_;

  base::WeakPtrFactory<KcerTokenImplNss> weak_factory_{this};
};

}  // namespace kcer::internal

#endif  // CHROME_BROWSER_CHROMEOS_KCER_NSS_KCER_TOKEN_IMPL_NSS_H_

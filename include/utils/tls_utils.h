#ifndef LOAD_BALANCER_TLS_UTILS_H
#define LOAD_BALANCER_TLS_UTILS_H

#include <openssl/ssl.h>
#include <string>

namespace load_balancer {
namespace utils {

// Provides utility functions for OpenSSL TLS operations.
// This class encapsulates common tasks like initializing OpenSSL, creating SSL
// contexts, and configuring them with certificates and private keys.
class TlsUtils {
 public:
  // Initializes the OpenSSL library.
  // This should be called once at the start of the application to prepare
  // OpenSSL for use.
  static void Initialize();

  // Creates a new SSL context (SSL_CTX).
  static SSL_CTX* CreateContext(bool is_server);

  // Configures an SSL context with a certificate and private key.
  static void ConfigureContext(SSL_CTX* ctx, const std::string& cert_file,
                               const std::string& key_file);
};

}  // namespace utils
}  // namespace load_balancer

#endif  // LOAD_BALANCER_TLS_UTILS_H

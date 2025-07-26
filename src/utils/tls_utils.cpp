#include "utils/tls_utils.h"

#include <openssl/err.h>
#include <spdlog/spdlog.h>

namespace load_balancer {
namespace utils {

void TlsUtils::Initialize() {
  // Load human-readable error messages.
  SSL_load_error_strings();
  // Register all available SSL/TLS algorithms.
  OpenSSL_add_ssl_algorithms();
}

SSL_CTX *TlsUtils::CreateContext(bool is_server) {
  // Select the appropriate SSL/TLS method based on whether it's a server or
  // client.
  const SSL_METHOD* method = is_server ? TLS_server_method()
                                       : TLS_client_method();
  SSL_CTX* ctx = SSL_CTX_new(method);
  if (!ctx) {
    spdlog::error("Unable to create SSL context");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
  return ctx;
}

void TlsUtils::ConfigureContext(SSL_CTX* ctx, const std::string& cert_file,
                                const std::string& key_file) {
  // Load the certificate file into the context.
  if (SSL_CTX_use_certificate_file(ctx, cert_file.c_str(), SSL_FILETYPE_PEM) <=
      0 ||
      // Load the private key file into the context.
      SSL_CTX_use_PrivateKey_file(ctx, key_file.c_str(), SSL_FILETYPE_PEM) <=
      0) {
    spdlog::error("Failed to configure SSL context");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
}

}  // namespace utils
}  // namespace load_balancer

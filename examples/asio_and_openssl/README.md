### SSL/TLS Configuration Guide

To enable secure communication between the client and the server, you must generate and configure SSL certificates.

#### 1. How to generate a self-signed certificate

Run the following command in your terminal (PowerShell) to generate a 2048-bit RSA key and a self-signed certificate
valid for 365 days:

```textmate
$env:OPENSSL_CONF = "$PWD/external_install/Debug/openssl/ssl/openssl.cnf"
& "external_install/Debug/openssl/x64/bin/openssl.exe" req -x509 -newkey rsa:2048 -keyout server.key -out server.crt -days 365 -nodes -subj "/CN=localhost"
```

**NOTE:** To generate sertificates secured by password, remove the `-nodes` option and use "ssl_context_
.set_password_callback" to set the password from the C++ code.

```textmate
$env:OPENSSL_CONF = "$PWD/external_install/Debug/openssl/ssl/openssl.cnf"
& "external_install/Debug/openssl/x64/bin/openssl.exe" req -x509 -newkey rsa:2048 -keyout server.key -out server.crt -days 365 -subj "/CN=localhost"
```

#### 2. Server Setup

Place the following generated files into the server's executable output directory (e.g., `cmake-build-debug/`):

- `server.crt` (Certificate chain file)
- `server.key` (Private key file)

The server uses these files to identify itself and establish an encrypted channel.

```cpp
// TODO For testing purposes only:
ssl_context_.use_certificate_chain_file("server.crt");
ssl_context_.use_private_key_file("server.key", asio::ssl::context::pem);
```

#### 3. Client Setup

Copy the `server.crt` file into the client's executable output directory.

To ensure the client verifies the server's identity, configure the `ssl_context` as follows:

```c++
// Enable peer verification
ssl_context.set_verify_mode(asio::ssl::verify_peer);

// Load the certificate to trust the specific server
ssl_context.load_verify_file("server.crt");
```

**Note:** When `verify_peer` is enabled, the client will drop the connection if the server's certificate does not match
the local `server.crt` file or is not signed by a trusted Authority.
Error "SSL Handshake failed: invalid certificate (X509 V3 routines)" will be thrown.
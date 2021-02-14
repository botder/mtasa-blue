/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server authentication middleware
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPServerVerification.h"
#include "SharedUtil.Crypto.h"
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>

namespace mtasa
{
    bool HTTPServerVerification::PreProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response)
    {
        HTTPHeader challenge;

        if (request.uri == "/get_verification_key_code" && request.TryGetHeader("crypto_challenge", challenge) && !challenge.value.empty())
        {
            SString path = g_pServerInterface->GetModManager()->GetAbsolutePath("verify.key");
            SString encodedPublicKey;
            SharedUtil::FileLoad(path, encodedPublicKey, 392);

            using namespace CryptoPP;

            try
            {
                // Load public RSA key from disk
                RSA::PublicKey publicKey;
                std::string    base64Data = SharedUtil::Base64decode(encodedPublicKey);
                StringSource   stringSource(base64Data, true);
                publicKey.Load(stringSource);

                // Launch encryptor and encrypt
                RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
                SecByteBlock             cipherText(encryptor.CiphertextLength(challenge.value.size()));
                AutoSeededRandomPool     rng;
                encryptor.Encrypt(rng, (const CryptoPP::byte*)challenge.value.data(), challenge.value.size(), cipherText.begin());

                if (!cipherText.empty())
                {
                    response.body = std::string(cipherText.begin(), cipherText.end());
                    response.statusCode = 200;
                    return false;
                }
                else
                    CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Empty crypto challenge was passed during verification\n");
            }
            catch (const std::exception&)
            {
                CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Invalid verify.key keyfile\n");
            }

            response.statusCode = 401;
            return false;
        }

        return true;
    }
}            // namespace mtasa

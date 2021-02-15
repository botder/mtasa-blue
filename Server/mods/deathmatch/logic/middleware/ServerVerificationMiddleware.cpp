/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for MTA server verification
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ServerVerificationMiddleware.h"
#include "web/Request.h"
#include "web/Response.h"
#include "SharedUtil.Crypto.h"
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>

namespace mtasa
{
    using namespace mtasa::web;
    using namespace std::string_view_literals;

    bool ServerVerificationMiddleware::PreProcessRequest(const Request& request, Response& response, AuxiliaryMiddlewarePayload& payload)
    {
        if (const URI* uri = request.GetURI(); uri != nullptr)
        {
            Header challenge;

            if (uri->path == "/get_verification_key_code"sv && request.TryGetHeader("Crypto-Challenge"sv, challenge) && !challenge.value.empty())
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
                        response.SetStatusCode(200);
                        response.SetBody(std::string(cipherText.begin(), cipherText.end()));
                        return false;
                    }
                    else
                        CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Empty crypto challenge was passed during verification\n");
                }
                catch (const std::exception&)
                {
                    CLogger::LogPrintf(LOGLEVEL_MEDIUM, "ERROR: Invalid verify.key keyfile\n");
                }

                response.SetStatusCode(401);
                return false;
            }
        }

        return true;
    }
}

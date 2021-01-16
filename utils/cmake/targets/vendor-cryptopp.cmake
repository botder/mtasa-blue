#
# CryptoPP library target
#
set(VENDOR_CRYPTOPP_DIR "${MTASA_VENDOR_DIR}/cryptopp")

file(GLOB_RECURSE CRYPTOPP_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_CRYPTOPP_DIR}/*.h"
)

add_library(vendor-cryptopp STATIC
    ${CRYPTOPP_SOURCES}
    "${VENDOR_CRYPTOPP_DIR}/cryptlib.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cpu.cpp"
    "${VENDOR_CRYPTOPP_DIR}/integer.cpp"
    "${VENDOR_CRYPTOPP_DIR}/3way.cpp"
    "${VENDOR_CRYPTOPP_DIR}/adler32.cpp"
    "${VENDOR_CRYPTOPP_DIR}/algebra.cpp"
    "${VENDOR_CRYPTOPP_DIR}/algparam.cpp"
    "${VENDOR_CRYPTOPP_DIR}/arc4.cpp"
    "${VENDOR_CRYPTOPP_DIR}/aria.cpp"
    "${VENDOR_CRYPTOPP_DIR}/aria_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ariatab.cpp"
    "${VENDOR_CRYPTOPP_DIR}/asn.cpp"
    "${VENDOR_CRYPTOPP_DIR}/authenc.cpp"
    "${VENDOR_CRYPTOPP_DIR}/base32.cpp"
    "${VENDOR_CRYPTOPP_DIR}/base64.cpp"
    "${VENDOR_CRYPTOPP_DIR}/basecode.cpp"
    "${VENDOR_CRYPTOPP_DIR}/bfinit.cpp"
    "${VENDOR_CRYPTOPP_DIR}/blake2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/blake2s_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/blake2b_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/blowfish.cpp"
    "${VENDOR_CRYPTOPP_DIR}/blumshub.cpp"
    "${VENDOR_CRYPTOPP_DIR}/camellia.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cast.cpp"
    "${VENDOR_CRYPTOPP_DIR}/casts.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cbcmac.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ccm.cpp"
    "${VENDOR_CRYPTOPP_DIR}/chacha.cpp"
    "${VENDOR_CRYPTOPP_DIR}/chacha_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/chacha_avx.cpp"
    "${VENDOR_CRYPTOPP_DIR}/chachapoly.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cham.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cham_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/channels.cpp"
    "${VENDOR_CRYPTOPP_DIR}/cmac.cpp"
    "${VENDOR_CRYPTOPP_DIR}/crc.cpp"
    "${VENDOR_CRYPTOPP_DIR}/crc_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/darn.cpp"
    "${VENDOR_CRYPTOPP_DIR}/default.cpp"
    "${VENDOR_CRYPTOPP_DIR}/des.cpp"
    "${VENDOR_CRYPTOPP_DIR}/dessp.cpp"
    "${VENDOR_CRYPTOPP_DIR}/dh.cpp"
    "${VENDOR_CRYPTOPP_DIR}/dh2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/dll.cpp"
    "${VENDOR_CRYPTOPP_DIR}/donna_32.cpp"
    "${VENDOR_CRYPTOPP_DIR}/donna_64.cpp"
    "${VENDOR_CRYPTOPP_DIR}/donna_sse.cpp"
    "${VENDOR_CRYPTOPP_DIR}/dsa.cpp"
    "${VENDOR_CRYPTOPP_DIR}/eax.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ec2n.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ecp.cpp"
    "${VENDOR_CRYPTOPP_DIR}/elgamal.cpp"
    "${VENDOR_CRYPTOPP_DIR}/emsa2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/esign.cpp"
    "${VENDOR_CRYPTOPP_DIR}/files.cpp"
    "${VENDOR_CRYPTOPP_DIR}/filters.cpp"
    "${VENDOR_CRYPTOPP_DIR}/fips140.cpp"
    "${VENDOR_CRYPTOPP_DIR}/fipstest.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gcm.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gcm_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gf256.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gf2_32.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gf2n.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gf2n_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gfpcrypt.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gost.cpp"
    "${VENDOR_CRYPTOPP_DIR}/gzip.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hc128.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hc256.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hex.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hight.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hmac.cpp"
    "${VENDOR_CRYPTOPP_DIR}/hrtimer.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ida.cpp"
    "${VENDOR_CRYPTOPP_DIR}/idea.cpp"
    "${VENDOR_CRYPTOPP_DIR}/iterhash.cpp"
    "${VENDOR_CRYPTOPP_DIR}/kalyna.cpp"
    "${VENDOR_CRYPTOPP_DIR}/kalynatab.cpp"
    "${VENDOR_CRYPTOPP_DIR}/keccak.cpp"
    "${VENDOR_CRYPTOPP_DIR}/keccak_core.cpp"
    "${VENDOR_CRYPTOPP_DIR}/keccak_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/lea.cpp"
    "${VENDOR_CRYPTOPP_DIR}/lea_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/luc.cpp"
    "${VENDOR_CRYPTOPP_DIR}/mars.cpp"
    "${VENDOR_CRYPTOPP_DIR}/marss.cpp"
    "${VENDOR_CRYPTOPP_DIR}/md2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/md4.cpp"
    "${VENDOR_CRYPTOPP_DIR}/md5.cpp"
    "${VENDOR_CRYPTOPP_DIR}/misc.cpp"
    "${VENDOR_CRYPTOPP_DIR}/modes.cpp"
    "${VENDOR_CRYPTOPP_DIR}/mqueue.cpp"
    "${VENDOR_CRYPTOPP_DIR}/mqv.cpp"
    "${VENDOR_CRYPTOPP_DIR}/nbtheory.cpp"
    "${VENDOR_CRYPTOPP_DIR}/oaep.cpp"
    "${VENDOR_CRYPTOPP_DIR}/osrng.cpp"
    "${VENDOR_CRYPTOPP_DIR}/padlkrng.cpp"
    "${VENDOR_CRYPTOPP_DIR}/panama.cpp"
    "${VENDOR_CRYPTOPP_DIR}/pkcspad.cpp"
    "${VENDOR_CRYPTOPP_DIR}/poly1305.cpp"
    "${VENDOR_CRYPTOPP_DIR}/polynomi.cpp"
    "${VENDOR_CRYPTOPP_DIR}/pssr.cpp"
    "${VENDOR_CRYPTOPP_DIR}/pubkey.cpp"
    "${VENDOR_CRYPTOPP_DIR}/queue.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rabin.cpp"
    "${VENDOR_CRYPTOPP_DIR}/randpool.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rabbit.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rc2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rc5.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rc6.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rdrand.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rdtables.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rijndael.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rijndael_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ripemd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rng.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rsa.cpp"
    "${VENDOR_CRYPTOPP_DIR}/rw.cpp"
    "${VENDOR_CRYPTOPP_DIR}/safer.cpp"
    "${VENDOR_CRYPTOPP_DIR}/salsa.cpp"
    "${VENDOR_CRYPTOPP_DIR}/scrypt.cpp"
    "${VENDOR_CRYPTOPP_DIR}/seal.cpp"
    "${VENDOR_CRYPTOPP_DIR}/seed.cpp"
    "${VENDOR_CRYPTOPP_DIR}/serpent.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sha.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sha_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sha3.cpp"
    "${VENDOR_CRYPTOPP_DIR}/shacal2.cpp"
    "${VENDOR_CRYPTOPP_DIR}/shacal2_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/shake.cpp"
    "${VENDOR_CRYPTOPP_DIR}/shark.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sharkbox.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simeck.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simeck_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simon.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simon64_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simon128_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/simple.cpp"
    "${VENDOR_CRYPTOPP_DIR}/skipjack.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sm3.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sm4.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sm4_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sosemanuk.cpp"
    "${VENDOR_CRYPTOPP_DIR}/speck.cpp"
    "${VENDOR_CRYPTOPP_DIR}/speck64_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/speck128_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/square.cpp"
    "${VENDOR_CRYPTOPP_DIR}/squaretb.cpp"
    "${VENDOR_CRYPTOPP_DIR}/sse_simd.cpp"
    "${VENDOR_CRYPTOPP_DIR}/strciphr.cpp"
    "${VENDOR_CRYPTOPP_DIR}/tea.cpp"
    "${VENDOR_CRYPTOPP_DIR}/tftables.cpp"
    "${VENDOR_CRYPTOPP_DIR}/threefish.cpp"
    "${VENDOR_CRYPTOPP_DIR}/tiger.cpp"
    "${VENDOR_CRYPTOPP_DIR}/tigertab.cpp"
    "${VENDOR_CRYPTOPP_DIR}/ttmac.cpp"
    "${VENDOR_CRYPTOPP_DIR}/tweetnacl.cpp"
    "${VENDOR_CRYPTOPP_DIR}/twofish.cpp"
    "${VENDOR_CRYPTOPP_DIR}/vmac.cpp"
    "${VENDOR_CRYPTOPP_DIR}/wake.cpp"
    "${VENDOR_CRYPTOPP_DIR}/whrlpool.cpp"
    "${VENDOR_CRYPTOPP_DIR}/xed25519.cpp"
    "${VENDOR_CRYPTOPP_DIR}/xtr.cpp"
    "${VENDOR_CRYPTOPP_DIR}/xtrcrypt.cpp"
    "${VENDOR_CRYPTOPP_DIR}/zdeflate.cpp"
    "${VENDOR_CRYPTOPP_DIR}/zinflate.cpp"
    "${VENDOR_CRYPTOPP_DIR}/zlib.cpp"
)

if (MTASA_OS_WINDOWS)
    # Workaround for Visual Studio generator bug (it gets confused by rdrand.cpp)
    configure_file("${VENDOR_CRYPTOPP_DIR}/rdrand.asm" "${CMAKE_CURRENT_BINARY_DIR}/cryptopp-rdrand.asm" COPYONLY)

    set(CRYPTOPP_ASM_FILES "${CMAKE_CURRENT_BINARY_DIR}/cryptopp-rdrand.asm")

    if (MTASA_X64)
        list(APPEND CRYPTOPP_ASM_FILES "${VENDOR_CRYPTOPP_DIR}/x64dll.asm")
        list(APPEND CRYPTOPP_ASM_FILES "${VENDOR_CRYPTOPP_DIR}/x64masm.asm")
    endif()

    set_source_files_properties(
        ${CRYPTOPP_ASM_FILES}
        PROPERTIES COMPILE_DEFINITIONS "$<IF:$<BOOL:${MTASA_X64}>,_M_X64,_M_X86>"
    )

    enable_language(ASM_MASM)

    set_source_files_properties(
        ${CRYPTOPP_ASM_FILES}
        PROPERTIES LANGUAGE ASM_MASM
    )

    target_sources(vendor-cryptopp PRIVATE ${CRYPTOPP_ASM_FILES})
endif()

target_compile_definitions(vendor-cryptopp
    PRIVATE
        USE_PRECOMPILED_HEADERS

    PUBLIC
        CRYPTOPP_DISABLE_CLMUL
        CRYPTOPP_DISABLE_AESNI
        CRYPTOPP_DISABLE_SSE4
        CRYPTOPP_DISABLE_SSSE3

        $<$<BOOL:${MTASA_OS_APPLE}>:
            CRYPTOPP_DISABLE_ASM
        >
)

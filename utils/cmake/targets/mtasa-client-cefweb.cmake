#
# Client CEFweb library target
#
set(CLIENT_CEFWEB_DIR "${MTASA_CLIENT_DIR}/cefweb")

add_library(mtasa-client-cefweb SHARED
    "${CLIENT_CEFWEB_DIR}/CAjaxResourceHandler.cpp"
    "${CLIENT_CEFWEB_DIR}/CAjaxResourceHandler.h"
    "${CLIENT_CEFWEB_DIR}/CefWeb.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebApp.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebApp.h"
    "${CLIENT_CEFWEB_DIR}/CWebCore.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebCore.h"
    "${CLIENT_CEFWEB_DIR}/CWebDevTools.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebDevTools.h"
    "${CLIENT_CEFWEB_DIR}/CWebsiteRequests.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebsiteRequests.h"
    "${CLIENT_CEFWEB_DIR}/CWebView.cpp"
    "${CLIENT_CEFWEB_DIR}/CWebView.h"
    "${CLIENT_CEFWEB_DIR}/StdInc.h"
    "${CLIENT_CEFWEB_DIR}/WebBrowserHelpers.h"
)

target_precompile_headers(mtasa-client-cefweb PRIVATE "${CLIENT_CEFWEB_DIR}/StdInc.h")

set_target_properties(mtasa-client-cefweb PROPERTIES OUTPUT_NAME "cefweb")

mtasa_set_target_outputdir(mtasa-client-cefweb "mta")

target_link_libraries(mtasa-client-cefweb PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    vendor-sparsehash
    vendor-cef
)

target_link_options(mtasa-client-cefweb PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /SAFESEH:NO
    >
)

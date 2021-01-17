#
# Client xml library target
#
set(CLIENT_XML_DIR "${MTASA_SHARED_DIR}/XML")

add_library(mtasa-client-xml SHARED
    "${CLIENT_XML_DIR}/CXMLArray.cpp"
    "${CLIENT_XML_DIR}/CXMLArray.h"
    "${CLIENT_XML_DIR}/CXMLAttributeImpl.cpp"
    "${CLIENT_XML_DIR}/CXMLAttributeImpl.h"
    "${CLIENT_XML_DIR}/CXMLAttributesImpl.cpp"
    "${CLIENT_XML_DIR}/CXMLAttributesImpl.h"
    "${CLIENT_XML_DIR}/CXMLFileImpl.cpp"
    "${CLIENT_XML_DIR}/CXMLFileImpl.h"
    "${CLIENT_XML_DIR}/CXMLImpl.cpp"
    "${CLIENT_XML_DIR}/CXMLImpl.h"
    "${CLIENT_XML_DIR}/CXMLNodeImpl.cpp"
    "${CLIENT_XML_DIR}/CXMLNodeImpl.h"
    "${CLIENT_XML_DIR}/XML.cpp"
)

target_link_libraries(mtasa-client-xml PRIVATE
    mtasa-shared-sdk
    vendor-tinyxml
)

set_target_properties(mtasa-client-xml PROPERTIES
    OUTPUT_NAME "xmll"
    DEBUG_POSTFIX ""
)

mtasa_set_target_outputdir(mtasa-client-xml "mta")

mtasa_create_debug_postfix_copy(mtasa-client-xml)

target_precompile_headers(mtasa-client-xml PRIVATE "${CLIENT_XML_DIR}/StdInc.h")

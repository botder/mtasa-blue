#
# Server xml library target
#
set(SERVER_XML_DIR "${MTASA_SHARED_DIR}/XML")

add_library(mtasa-server-xml SHARED
    "${SERVER_XML_DIR}/CXMLArray.cpp"
    "${SERVER_XML_DIR}/CXMLArray.h"
    "${SERVER_XML_DIR}/CXMLAttributeImpl.cpp"
    "${SERVER_XML_DIR}/CXMLAttributeImpl.h"
    "${SERVER_XML_DIR}/CXMLAttributesImpl.cpp"
    "${SERVER_XML_DIR}/CXMLAttributesImpl.h"
    "${SERVER_XML_DIR}/CXMLFileImpl.cpp"
    "${SERVER_XML_DIR}/CXMLFileImpl.h"
    "${SERVER_XML_DIR}/CXMLImpl.cpp"
    "${SERVER_XML_DIR}/CXMLImpl.h"
    "${SERVER_XML_DIR}/CXMLNodeImpl.cpp"
    "${SERVER_XML_DIR}/CXMLNodeImpl.h"
    "${SERVER_XML_DIR}/XML.cpp"
)

target_link_libraries(mtasa-server-xml PRIVATE
    mtasa-shared-sdk
    vendor-tinyxml
)

set_target_properties(mtasa-server-xml PROPERTIES
    OUTPUT_NAME "xmll"
    DEBUG_POSTFIX ""
)

mtasa_set_target_outputdir(mtasa-server-xml "${MTASA_SERVER_OUTPUT_DIR}")

mtasa_create_debug_postfix_copy(mtasa-server-xml)

target_precompile_headers(mtasa-server-xml PRIVATE "${SERVER_XML_DIR}/StdInc.h")

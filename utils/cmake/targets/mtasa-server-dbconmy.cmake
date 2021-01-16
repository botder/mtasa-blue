#
# Server MySQL database connection library target
#
set(SERVER_DBCONMY_DIR "${MTASA_SERVER_DIR}/dbconmy")

add_library(mtasa-server-dbconmy SHARED
    "${SERVER_DBCONMY_DIR}/CDatabaseConnectionMySql.cpp"
    "${SERVER_DBCONMY_DIR}/StdInc.cpp" # for SharedUtil.hpp
)

target_precompile_headers(mtasa-server-dbconmy PRIVATE "${SERVER_DBCONMY_DIR}/StdInc.h")

set_target_properties(mtasa-server-dbconmy PROPERTIES OUTPUT_NAME "dbconmy")

mtasa_set_target_outputdir(mtasa-server-dbconmy "${MTASA_SERVER_OUTPUT_DIR}")

target_link_libraries(mtasa-server-dbconmy PRIVATE
    mtasa-server-sdk
    mtasa-shared-sdk
    vendor-sparsehash
    vendor-mysql
)

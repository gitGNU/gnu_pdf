C_SOURCES = $(shell find src -name '*.[chly]')
local-checks-to-skip = sc_prohibit_strcmp \
                       sc_file_system \
                       sc_trailing_blank \
                       sc_immutable_NEWS \
                       sc_cross_check_PATH_usage_in_tests

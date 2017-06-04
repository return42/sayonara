function(NEW_TEST ...)

        STRING(REGEX MATCH "^(.*/)*(.*)\\.[^.]*$" dummy ${ARGV0})
	SET(TEST_NAME ${CMAKE_MATCH_2})

	ADD_EXECUTABLE(${TEST_NAME} ${ARGV})
	TARGET_LINK_LIBRARIES(${TEST_NAME}
		sayonara_test
		Qt5::Test
	)

	ADD_TEST(NAME ${TEST_NAME} COMMAND ${TEST_NAME} -o "${TEST_NAME}.out")
	MESSAGE("Add test ${TEST_NAME}")

endfunction()


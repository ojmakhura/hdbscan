project(collections_tests)

add_executable(collections_prime_tests primetests.c)
target_link_libraries(collections_prime_tests ${COLLECTIONS_LIBRARY}_static cunit)

add_executable(collections_arraylist_tests listtests.c)
target_link_libraries(collections_arraylist_tests ${COLLECTIONS_LIBRARY}_static cunit)

add_executable(collections_linkedlist_tests linkedlisttests.c)
target_link_libraries(collections_linkedlist_tests ${COLLECTIONS_LIBRARY}_static cunit)

add_executable(collections_hashtable_tests hashtabletests.c)
target_link_libraries(collections_hashtable_tests ${COLLECTIONS_LIBRARY}_static cunit)

add_executable(collections_set_tests settests.c)
target_link_libraries(collections_set_tests ${COLLECTIONS_LIBRARY}_static cunit)

include_directories(${GNULIB_INCLUDE_DIR} ${COLLECTIONS_INCLUDE_DIR})
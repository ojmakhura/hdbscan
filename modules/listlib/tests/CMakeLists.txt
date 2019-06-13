project(listlib_tests)

add_executable(listlib_prime_tests primetests.c)
target_link_libraries(listlib_prime_tests ${LISTLIB_LIBRARY}_static cunit)

add_executable(listlib_arraylist_tests listtests.c)
target_link_libraries(listlib_arraylist_tests ${LISTLIB_LIBRARY}_static cunit)

add_executable(listlib_linkedlist_tests linkedlisttests.c)
target_link_libraries(listlib_linkedlist_tests ${LISTLIB_LIBRARY}_static cunit)

add_executable(listlib_hashtable_tests hashtabletests.c)
target_link_libraries(listlib_hashtable_tests ${LISTLIB_LIBRARY}_static cunit)

include_directories(${GNULIB_INCLUDE_DIR} ${LISTLIB_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS})
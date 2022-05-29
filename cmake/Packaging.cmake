# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}
    CACHE STRING "HDBSCAN"
)
# which is useful in case of packing only selected components instead of the whole thing
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HDBSCAN C implementation"
    CACHE STRING "HDBSCAN C implementation"
)
set(CPACK_PACKAGE_VENDOR "Rogue Systems")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

# https://unix.stackexchange.com/a/11552/254512
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})#/${CMAKE_PROJECT_VERSION}")

set(CPACK_PACKAGE_VERSION_MAJOR ${HDBSCAN_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${HDBSCAN_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${HDBSCAN_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT "ojmakhura@roguesystems.co.bw")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Onalenna J. Makhura")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# set(CPACK_DEBIAN_PACKAGE_DEPENDS "m")

# package name for deb
# if set, then instead of some-application-0.9.2-Linux.deb
# you'll get some-application_0.9.2_amd64.deb (note the underscores too)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

# if you want every group to have its own package,
# although the same happens if this is not sent (so it defaults to ONE_PER_GROUP)
# and CPACK_DEB_COMPONENT_INSTALL is set to YES
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)#ONE_PER_GROUP)
# without this you won't be able to pack only specified component
set(CPACK_DEB_COMPONENT_INSTALL YES)

include(CPack)
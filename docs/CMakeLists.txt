# Copyright 2021 Arnaud Becheler
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)

if( NOT DOXYGEN_FOUND)

  message([WARNING] " Doxygen need to be installed to generate the doxygen documentation")

else ()


  # doxygen settings can be set here, prefixed with "DOXYGEN_"
  set(DOXYGEN_PROJECT_NAME "Quetzal-CoaTL")
  set(DOXYGEN_PROJECT_BRIEF "The Coalescence Template Library")
  set(DOXYGEN_PROJECT_LOGO "quetzal-coaltl-fa.png")
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "mainpage.md")
  set(DOXYGEN_EXCLUDE_PATTERNS "README.md" "doxygen-awesome-css" "*env*" "conanfile.*")
  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")
  set(DOXYGEN_HTML_EXTRA_STYLESHEET "doxygen-awesome-css/doxygen-awesome.css"
                                    "doxygen-awesome-css/doxygen-awesome-sidebar-only.css"
                                    "doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js"
                                    "doxygen-awesome-css/doxygen-awesome-sidebar-only-darkmode-toggle.css"
                                    "doxygen-awesome-css/doxygen-awesome-fragment-copy-button.js")
  # Documentation for tutorials requires ctest call to populate build folder
  set(DOXYGEN_EXAMPLE_PATH "../example" "../build/example/output")

  # Required by doxygen awesome
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_HTML_HEADER "header.html")
  set(DOXYGEN_COLLABORATION_GRAPH NO)
  set(DOXYGEN_EXTRACT_PRIVATE NO)
  set(DOXYGEN_EXTRACT_ALL NO)
  set(DOXYGEN_INHERIT_DOC YES)
  set(DOXYGEN_INLINE_INHERITED_MEMB YES)
  set(DOXYGEN_USE_MATHJAX YES)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
  set(DOXYGEN_HTML_COLORSTYLE LIGHT)

  # run "make docs" to create the doxygen documentation
  doxygen_add_docs(docs ${PROJECT_SOURCE_DIR} COMMENT "Generate API-documents.")

endif ()


# look for Doxygen package
# Require dot, treat the other components as optional
find_package(Doxygen
             #REQUIRED dot
           OPTIONAL_COMPONENTS dot mscgen dia)

if(DOXYGEN_FOUND)

  # doxygen settings can be set here, prefixed with "DOXYGEN_"
  set(DOXYGEN_PROJECT_NAME "Quetzal-CoalTL")
  set(DOXYGEN_PROJECT_BRIEF "The Coalescence Template Library")
  set(DOXYGEN_PROJECT_LOGO "quetzal-coaltl-fa.png")
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "mainpage.md")
  set(DOXYGEN_EXCLUDE_PATTERNS "README.md" "doxygen-awesome-css" "*env*")
 
  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")

  set(DOXYGEN_HTML_EXTRA_STYLESHEET "doxygen-awesome-css/doxygen-awesome.css"
                                    "doxygen-awesome-css/doxygen-awesome-sidebar-only.css"
                                    "doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js"
                                    "doxygen-awesome-css/doxygen-awesome-sidebar-only-darkmode-toggle.css"
                                    "doxygen-awesome-css/doxygen-awesome-fragment-copy-button.js")
  # Required by doxygen awesome
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_HTML_HEADER "header.html")

  set(DOXYGEN_EXTRACT_PRIVATE NO)
  set(DOXYGEN_INLINE_INHERITED_MEMB YES)
  set(DOXYGEN_EXAMPLE_PATH "examples")
  set(DOXYGEN_USE_MATHJAX YES)
  # this target will only be built if specifically asked to.
  # run "make docs" to create the doxygen documentation
  doxygen_add_docs(
    docs
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate API-documents."
  )
else (DOXYGEN_FOUND)
  message([WARNING] " Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

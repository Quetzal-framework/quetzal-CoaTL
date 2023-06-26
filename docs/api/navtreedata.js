/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Quetzal-CoaTL", "index.html", [
    [ "The Quetzal Coalescence Library", "index.html", null ],
    [ "Introduction", "md_1_introduction.html", [
      [ "Why Quetzal?", "md_1_introduction.html#autotoc_md1", null ],
      [ "Why -CoaTL ?", "md_1_introduction.html#autotoc_md3", null ],
      [ "Why C++?", "md_1_introduction.html#autotoc_md4", null ],
      [ "Why templates?", "md_1_introduction.html#autotoc_md6", null ],
      [ "Why decoupling algorithms from data structures?", "md_1_introduction.html#autotoc_md8", null ]
    ] ],
    [ "Abstract", "md_2_abstract.html", [
      [ "Input/Output", "md_2_abstract.html#autotoc_md11", null ],
      [ "Algorithms", "md_2_abstract.html#autotoc_md12", null ],
      [ "Graphs", "md_2_abstract.html#autotoc_md13", null ],
      [ "Compile-time units system", "md_2_abstract.html#autotoc_md14", null ],
      [ "Data Structures", "md_2_abstract.html#autotoc_md15", null ]
    ] ],
    [ "Installation and Usage", "md_3_installation.html", [
      [ "Structure and Dependencies", "md_3_installation.html#autotoc_md17", null ],
      [ "Installation and Reuse", "md_3_installation.html#autotoc_md19", [
        [ "Copy", "md_3_installation.html#autotoc_md21", null ],
        [ "Copy + CMake", "md_3_installation.html#autotoc_md23", null ],
        [ "Conan + CMake", "md_3_installation.html#autotoc_md25", null ],
        [ "Docker", "md_3_installation.html#autotoc_md26", null ],
        [ "Starter template + VSC", "md_3_installation.html#autotoc_md27", null ]
      ] ],
      [ "Tooling Overview", "md_3_installation.html#autotoc_md29", [
        [ "CMake", "md_3_installation.html#autotoc_md31", null ],
        [ "Conan", "md_3_installation.html#autotoc_md33", null ],
        [ "Docker", "md_3_installation.html#autotoc_md35", null ]
      ] ]
    ] ],
    [ "Include", "md_4_include.html", null ],
    [ "Tutorials", "tutorials.html", "tutorials" ],
    [ "Developer's notes", "md_99_developers_notes.html", [
      [ "Conventions", "md_99_developers_notes.html#autotoc_md98", null ],
      [ "Build process", "md_99_developers_notes.html#autotoc_md99", null ],
      [ "Packaging", "md_99_developers_notes.html#autotoc_md100", null ],
      [ "Documentation", "md_99_developers_notes.html#autotoc_md101", null ]
    ] ],
    [ "Boost Graph Library Notes", "md_tbd_98_BGL.html", [
      [ "Property Maps", "md_tbd_98_BGL.html#autotoc_md104", null ],
      [ "Implementation perspective", "md_tbd_98_BGL.html#autotoc_md105", null ],
      [ "Conceptual perspective", "md_tbd_98_BGL.html#autotoc_md106", null ],
      [ "How to initialize a property", "md_tbd_98_BGL.html#autotoc_md107", null ],
      [ "How to modify a property map", "md_tbd_98_BGL.html#autotoc_md108", null ],
      [ "Auxillary Property Maps for Algortihms", "md_tbd_98_BGL.html#autotoc_md109", [
        [ "Predecessor Maps", "md_tbd_98_BGL.html#autotoc_md110", null ],
        [ "Color Maps", "md_tbd_98_BGL.html#autotoc_md111", [
          [ "Objectives", "newick_generator.html#autotoc_md58", null ],
          [ "From a Quetzal binary tree", "newick_generator.html#autotoc_md60", [
            [ "No property", "newick_generator.html#autotoc_md62", null ],
            [ "Custom properties", "newick_generator.html#autotoc_md64", null ]
          ] ],
          [ "From a Quetzal k-ary tree", "newick_generator.html#autotoc_md66", [
            [ "No property", "newick_generator.html#autotoc_md68", null ],
            [ "Custom properties", "newick_generator.html#autotoc_md70", null ]
          ] ],
          [ "Interfacing legacy code", "newick_generator.html#autotoc_md72", [
            [ "No property", "newick_generator.html#autotoc_md74", null ],
            [ "Custom properties", "newick_generator.html#autotoc_md76", null ]
          ] ],
          [ "Reading a single variable", "geotiff_parser.html#autotoc_md86", null ],
          [ "Reading multiple variables", "geotiff_parser.html#autotoc_md88", null ],
          [ "Next steps", "geotiff_parser.html#autotoc_md90", [
            [ "Demes on a regular spatial grid", "geotiff_parser.html#autotoc_md92", null ],
            [ "Demes on a variable spatial graph", "geotiff_parser.html#autotoc_md94", null ],
            [ "Inform local processes", "geotiff_parser.html#autotoc_md96", null ]
          ] ],
          [ "Invariants Guarantee", "graphs_in_quetzal.html#autotoc_md77", null ],
          [ "Edge and Vertices descriptors", "graphs_in_quetzal.html#autotoc_md78", null ],
          [ "Edge and Vertices Information (Property Classes)", "graphs_in_quetzal.html#autotoc_md79", null ],
          [ "Depth First Search (DFS)  Algorithms", "graphs_in_quetzal.html#autotoc_md80", null ],
          [ "DFS on trees", "graphs_in_quetzal.html#autotoc_md81", null ],
          [ "Create a color map", "md_tbd_98_BGL.html#autotoc_md112", null ],
          [ "Call a BFS", "md_tbd_98_BGL.html#autotoc_md113", null ],
          [ "Mapping colors to numbers", "md_tbd_98_BGL.html#autotoc_md114", null ]
        ] ]
      ] ],
      [ "Notes", "md_tbd_98_BGL.html#autotoc_md115", null ]
    ] ],
    [ "Polymorphism statistics", "md_tbd_accumulators.html", [
      [ "Hello world!", "md_tbd_accumulators.html#autotoc_md117", null ],
      [ "Motivations", "md_tbd_accumulators.html#autotoc_md118", null ],
      [ "Implementation", "md_tbd_accumulators.html#autotoc_md119", null ],
      [ " Terminology", "md_tbd_accumulators.html#autotoc_md120", null ],
      [ "Overview", "md_tbd_accumulators.html#autotoc_md121", null ]
    ] ],
    [ "Frameworks basics", "md_tbd_framework_basics.html", [
      [ "Basic Concepts", "md_tbd_framework_basics.html#autotoc_md123", [
        [ "Genetics", "md_tbd_framework_basics.html#autotoc_md124", [
          [ "Sequences Concepts", "md_tbd_framework_basics.html#autotoc_md125", null ]
        ] ]
      ] ]
    ] ],
    [ "Defining and using systems of units in Quetzal", "md_tbd_system_of_units.html", null ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"",
"classquetzal_1_1coalescence_1_1container_1_1Forest.html#a95803e45bef856e06cea28d83d00d2fd",
"classquetzal_1_1demography_1_1HistoryBase.html#a41d0bfe5fc89709eef30a4300b0dae1a",
"classquetzal_1_1geography_1_1gdalcpp_1_1detail_1_1Driver.html#a9006b5ea4117ad9f09212513739aeb3b",
"classquetzal_1_1utils_1_1random_1_1TransitionKernel_3_01Distribution_01_4.html#ad9d2f423bb919ab5a3fc8fd02d6b7f74",
"namespacequetzal_1_1coalescence.html#a313b46624ad48cebcb9671f9a20f01ac",
"structquetzal_1_1coalescence_1_1detail_1_1EdgeManager.html#a73827ec54c199de81f337afd70d36be3",
"structquetzal_1_1geography_1_1lonlat.html#ac89ed034286671da8d7a1e9e6fdaa335"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';
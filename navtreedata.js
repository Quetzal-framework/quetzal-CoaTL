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
    [ "Introduction", "md_1-introduction.html", [
      [ "Why Quetzal?", "md_1-introduction.html#autotoc_md1", null ],
      [ "Why -CoaTL ?", "md_1-introduction.html#autotoc_md3", null ],
      [ "Why C++?", "md_1-introduction.html#autotoc_md5", null ],
      [ "Why templates?", "md_1-introduction.html#autotoc_md7", null ],
      [ "Why decoupling algorithms from data structures?", "md_1-introduction.html#autotoc_md9", null ]
    ] ],
    [ "Installation and Usage", "md_2-installation.html", [
      [ "Structure and Dependencies", "md_2-installation.html#autotoc_md12", null ],
      [ "Installation and Reuse", "md_2-installation.html#autotoc_md14", [
        [ "Easiest: DevContainer on Visual Studio Code", "md_2-installation.html#autotoc_md16", null ],
        [ "Copy", "md_2-installation.html#autotoc_md18", null ],
        [ "Copy + CMake", "md_2-installation.html#autotoc_md20", null ],
        [ "Conan + CMake", "md_2-installation.html#autotoc_md22", null ],
        [ "Docker", "md_2-installation.html#autotoc_md23", null ],
        [ "Starter template + VSC", "md_2-installation.html#autotoc_md24", null ]
      ] ],
      [ "Tooling Overview", "md_2-installation.html#autotoc_md26", [
        [ "CMake", "md_2-installation.html#autotoc_md28", null ],
        [ "Conan", "md_2-installation.html#autotoc_md30", null ],
        [ "Docker", "md_2-installation.html#autotoc_md32", null ]
      ] ]
    ] ],
    [ "Include", "md_3-include.html", null ],
    [ "Tutorials", "tutorials.html", [
      [ "Parsing a Newick string", "newick_parser.html", [
        [ "Input/Output", "tutorials.html#autotoc_md34", null ],
        [ "Demographic Histories", "tutorials.html#autotoc_md35", null ],
        [ "Graphs", "tutorials.html#autotoc_md36", [
          [ "Introduction", "newick_parser.html#autotoc_md37", [
            [ "Background", "newick_parser.html#autotoc_md39", null ],
            [ "Grammar", "newick_parser.html#autotoc_md41", null ],
            [ "Objectives", "newick_parser.html#autotoc_md43", null ]
          ] ],
          [ "To a Quetzal k-ary tree", "newick_parser.html#autotoc_md45", [
            [ "Default properties", "newick_parser.html#autotoc_md47", null ],
            [ "Custom properties", "newick_parser.html#autotoc_md49", null ]
          ] ],
          [ "Interfacing legacy code", "newick_parser.html#autotoc_md51", null ]
        ] ]
      ] ],
      [ "Generating a Newick string", "newick_generator.html", null ],
      [ "Parsing an Extended Newick string", "extended_newick_parser.html", null ],
      [ "Generating an Extended Newick string", "extended_newick_generator.html", null ],
      [ "Reading spatial variables from rasters", "geotiff_parser.html", null ],
      [ "Writing spatial variables to rasters", "geotiff_generator.html", null ],
      [ "Writing spatial samples to shapefile", "shapefile_generator.html", null ],
      [ "Demographic Histories in Quetzal", "demographic_histories_in_quetzal.html", null ],
      [ "Spatial Graphs", "spatial_graphs.html", null ],
      [ "Construction", "spatial_graph_construction.html", null ],
      [ "Embedding vertex/edge information", "spatial_graph_information.html", null ],
      [ "Local Parameters", "spatial_graph_local_parameters.html", [
        [ "Environmental Niche Models", "spatial_graph_local_parameters.html#autotoc_md113", [
          [ "Niche models as mathematical expressions", "spatial_graph_local_parameters.html#autotoc_md114", null ],
          [ "Mathematical composition in C++", "spatial_graph_local_parameters.html#autotoc_md115", null ],
          [ "Mathematical composition with quetzal::expressive", "spatial_graph_local_parameters.html#autotoc_md116", null ]
        ] ],
        [ "Preparing a suitability landscape for the simulation", "spatial_graph_local_parameters.html#autotoc_md118", null ],
        [ "Adjusting a suitability map using a Digital Elevation Model", "spatial_graph_local_parameters.html#autotoc_md120", [
          [ "Why separating Elevation from Suitability ?", "spatial_graph_local_parameters.html#autotoc_md121", null ]
        ] ],
        [ "Defining a friction model for trans-oceanic dispersal events", "spatial_graph_local_parameters.html#autotoc_md123", null ]
      ] ],
      [ "Autoregressive processes and time series", "spatial_graph_local_process.html", null ],
      [ "Dispersal on a spatial graph", "spatial_graph_dispersal_kernels.html", null ],
      [ "Graphs in Quetzal", "graphs_in_quetzal.html", null ],
      [ "Binary Trees", "coalescence_binary_tree.html", [
        [ "Introduction", "coalescence_binary_tree.html#autotoc_md76", null ],
        [ "Implementations", "coalescence_binary_tree.html#autotoc_md77", null ]
      ] ],
      [ "K-ary Trees", "coalescence_k_ary_tree.html", [
        [ "Introduction", "coalescence_k_ary_tree.html#autotoc_md78", null ],
        [ "Implementations", "coalescence_k_ary_tree.html#autotoc_md79", null ]
      ] ]
    ] ],
    [ "Developer's notes", "md_99-developers-notes.html", [
      [ "Conventions", "md_99-developers-notes.html#autotoc_md145", null ],
      [ "Build process", "md_99-developers-notes.html#autotoc_md146", null ],
      [ "Packaging", "md_99-developers-notes.html#autotoc_md147", null ],
      [ "Documentation", "md_99-developers-notes.html#autotoc_md148", null ]
    ] ],
    [ "Abstract", "md_drafts_22-abstract.html", [
      [ "Input/Output", "md_drafts_22-abstract.html#autotoc_md150", null ],
      [ "Algorithms", "md_drafts_22-abstract.html#autotoc_md151", null ],
      [ "Graphs", "md_drafts_22-abstract.html#autotoc_md152", null ],
      [ "Compile-time units system", "md_drafts_22-abstract.html#autotoc_md153", null ],
      [ "Data Structures", "md_drafts_22-abstract.html#autotoc_md154", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Concepts", "concepts.html", "concepts" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"Allele_8hpp_source.html",
"classquetzal_1_1coalescence_1_1binary__tree_3_01no__property_00_01EdgeProperty_01_4.html#a57f5dd629d965b601174769b61887b87",
"classquetzal_1_1coalescence_1_1detail_1_1binary__tree__common.html#aa74af67f0fd0f12addd4371057a99893",
"classquetzal_1_1coalescence_1_1k__ary__tree_3_01VertexProperty_00_01no__property_01_4.html#aa453eedcea59667362fefdb5c98812f5",
"classquetzal_1_1coalescence_1_1network_3_01VertexProperty_00_01no__property_01_4.html#a385fba6e22b1aca6cd46b02df84ee37f",
"classquetzal_1_1coalescence_1_1newick__with__distance__to__parent__and__leaf__name_1_1cell__type.html",
"classquetzal_1_1format_1_1newick_1_1Formatter.html#aa08c44858d09d59d7f3585cb41e6f638",
"classquetzal_1_1geography_1_1graph_3_01VertexProperty_00_01no__property_00_01Representation_00_01Directed_01_4.html#ab0b5f1fd7dd9d80ee86124cfc758ba78",
"classquetzal_1_1geography_1_1raster.html#a5713da017b8f6fdac1325314fac2d1cc",
"conceptquetzal_1_1geography_1_1Coordinate.html",
"newick_generator.html",
"structquetzal_1_1format_1_1newick_1_1TreeAlign.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';
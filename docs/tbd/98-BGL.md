# Boost Graph Library Notes

When manipulating graphs, there are 3 things we need to keep in mind:
1. How to store the Graph structure
2. How to associate properties to vertices or edges (distances, weights etc)
3. How get those properties in and out of algorithms (like DFS, BFS etc)


## Property Maps

A Property Map maps vertices/edges to additional data associated with the particular vertex/edge.
E.g. the weightmap in the example associates a weight (travelling cost) with each edge.

## Implementation perspective

The vertex and edge properties are not stored individually, but *within* the vertices
and the edges of the graph. Consequently
- There is no use of std::map or some other associative container.
- Whatever was provided to the `adjacency_list` as the `VertexProperties` and `EdgeProperties`
template parameter will be stored in the vertices and edges.
- It is the same as when one uses `std::list<T>` where `T` is stored in the nodes of the linked-list (along with the necessary next-prev pointers).
- In other words, `adjacency_list` will store vertices that contain an object of type `VertexProperties`, plus whatever edge-lists (in and out) that is needed.
- Looking up the vertex property from the vertex-descriptor is a constant-time operation (e.g., dereference a pointer, lookup by index, etc.).
- Fetching (for read or write) a specific property for a specific vertex is a constant-time operation.

## Conceptual perspective

When we use `property_map` (via `get`/`put` functions), it is only doing a bit
of template meta-programming magic to create a thin wrapper that will just
read/write the correct individual property for a vertex or edge.

Conceptually
```cpp
NodeInfo info = boost::get (NodeInfoPropertyTag(), G, v);
```
is equivalent to:
```cpp
NodeInfo info = G[v].NodeInfoProperty;
```
This is all the property-map really does, it looks up the vertex property
(by the vertex-descriptor in the given graph object), and it fetches the data
member (or sub-object) of the vertex property that corresponds to the given tag type.

Figuring out how to obtain the correct data member (or sub-object) for the correct
property tag is a piece of template meta-programming magic that figures it out at compile-time
(no run-time overhead).


## How to initialize a property

Here, `color_map` is essentially an unitialized reference:

```cpp
using color_map_t = typename boost::property_map<Graph, boost::vertex_color_t>::type;
color_map_t color_map;
```
@note
An error like `cannot form a reference to 'void'` usually indicates an associated
property map could not be found. This is what happens if you fail to supply
the `vertex_color` internal property. In this case the remedy is simply to use the default color map
using named parameters:

```cpp
random_spanning_tree(
    g, rng,
    boost::root_vertex(random_vertex(g, rng))
        .predecessor_map(boost::make_assoc_property_map(predecessors))
        .weight_map(boost::static_property_map(1.0)) // unweighted
);
```

Assuming that a `vertex_color_t` property map has been associated with the graph
by traits, that is the property is an iternal property of the graph:

```cpp
color_map_t color_map = get(boost::vertex_color, graph);
```
@note
Internal properties are often used by default.

To create a "virtual" property map (without a backing data structure) that
returns the construction parameter on every key (logically, the return value is constant):

```cpp
auto predecessor_map = boost::make_static_property_map<vertex_t, vertex_t>(vertex_t());
```
@note such a constant can not be modified

To create a modifiable (LValuePropertyMap), you can do:
```cpp
std::map<vertex_t, vertex_t> predecessors;
auto predecessor_map =boost::make_assoc_property_map(predecessors);
```

To use a vertex index to (optionally) translate descriptors into vector indices.

@note
The second is fixed-size, so initialize it after creating all vertices.

```cpp
auto vindex          = get(boost::vertex_index, g);
auto predecessors    = std::vector<vertex_t>(num_vertices(g));
auto predecessor_map = boost::make_safe_iterator_property_map(
    predecessors.begin(), predecessors.size(), vindex);
```

To create a constant weight map
```cpp
auto weight_map = boost::static_property_map(1.0);
```

## How to modify a property map

## Auxillary Property Maps for Algortihms

For performance reasons, most algorithm will require some type of property map: colors, weights, predecessors...

@note
Note most algorithms default the auxilliary property maps when none is provided

### Predecessor Maps

The predecessor_map is used to record the paths for all vertices
(for every vertex the predecessor on the path from the root is recorded).

@note
It is often user provided because that information is something one often hopes
to get out of the algorithm for performance reasons.

### Color Maps

The Type of the color map used in algorithms like BFS.

- If you don't care about efficiency they can be defaulted either
    - relying on the algorithm to default them
    - using tagged properties using `property<vertex_color_t, default_color_type>`
    - using trait specializations to associate your external maps with your graph type
      specializing `graph_property<>`
- To enable enormous efficiency gains, color maps can be supplied, so the coloring/storage can be reused across algorithm invocations.

#### Create a color map

This maps `vertex_descriptor` to (in this case) `default_color_type`.

```cpp
using color_map_t boost::property_map<GraphType, boost::vertex_color_t>::type;
color_map_t colorMap;
```

#### Call a BFS

```cpp
boost::breadth_first_visit(g, *boost::vertices(g).first, boost::color_map(colorMap));
```

#### Mapping colors to numbers

> Why would one want to do that?

Let's maps colors to integers with:

```cpp
const char* color_names[] = {"white", "gray", "green", "red", "black"};
```

one could iterate iterate over all vertices in the graph
and pass the current `vertex_descriptor` to the `[]-operator` in the color map:

```cpp
GraphType::vertex_iterator it, itEnd;
for (boost::tie(it, itEnd) = boost::vertices(g); it != itEnd; it++)
{
  std::cout << "Color of node " << *it << " is " << color_names[colorMap[*it]] << std::endl;
}
```

## Notes 

Edge descriptors are internal descriptors, sort of like stable iterators. E.g.
```cpp
edge_descriptor e = *edges(g).begin();
```

Edge properties are entirely separate notions. You get the edge properties from an edge descriptor like so:
```cpp
Bond* e_props = g[e]; // equivalent to:
e_props = get(boost::edge_bundle, g, e);
```
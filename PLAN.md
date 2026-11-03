# Weeks 1-3 plan

## Week 1: foundation (this starter)
1. Read the code end to end. Make sure you can explain CSR and why Dijkstra uses a priority queue.
2. Add `--lambda` sweep to the CLI to print a risk/time trade-off table.
3. Add a `bench/` target that times Dijkstra on a random 100k-node graph.
4. Push to GitHub, confirm CI is green, tag v0.1.0.

## Week 2: OSM ingestion
1. Choose your city, download its extract (Geofabrik or BBBike), keep it out of git.
2. Use libosmium (or a small Python pre-step to CSV if the build fights you) to extract
   highway ways, node coordinates, tags (highway, lit, maxspeed, oneway, surface).
3. Build the graph: split ways at intersections, compute lengths (haversine), speeds from
   defaults by road class, handle oneway. Keep the largest strongly connected component.
4. Write a validation test: known landmark-to-landmark route vs a map service, within tolerance.

## Week 3: edge safety features
1. Snap POIs (shops, bus stops, police, hospitals) to edges with a spatial index.
2. Compute per-edge features: lit flag, road class, open-POI density, transit proximity, isolation.
3. Produce an interpretable risk in [0,1] with tunable weights; export a per-edge CSV for inspection.
4. Map a sample of edges and sanity-check them against your own knowledge of the area.

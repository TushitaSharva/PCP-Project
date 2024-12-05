import networkx as nx
import sys

# Function to read edges from the input file
def read_edges_from_file(file_name):
    edges = []
    with open(file_name, 'r') as f:
        # Read the number of nodes (V) and edges (E)
        V, E = map(int, f.readline().split())
        # Read each edge from the file
        for line in f:
            u, v = map(int, line.split())
            edges.append((u, v))
    return edges, V, E

# Check if the filename is passed as an argument
if len(sys.argv) < 2:
    print("Usage: python3 python_Code.py <input_file>")
    sys.exit(1)

# Get the filename from command-line arguments
input_file = sys.argv[1]

# Read edges from the file
edges, V, E = read_edges_from_file(input_file)

# Create a directed graph
G = nx.DiGraph()

# Add edges to the graph
G.add_edges_from(edges)

# Compute betweenness centrality
betweenness_centrality = nx.betweenness_centrality(G, normalized=True, endpoints=False)

# print(list(G.edges(data=True)))
print(" ".join(f"{centrality}" for centrality in betweenness_centrality.values()))
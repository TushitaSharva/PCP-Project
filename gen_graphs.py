import random

def generate_random_graph(num_nodes, num_edges, output_file):
    edges = set()
    while len(edges) < num_edges:
        u = random.randint(0, num_nodes - 1)
        v = random.randint(0, num_nodes - 1)
        if u != v:  # Avoid self-loops
            edges.add((u, v))
    
    with open(output_file, "w") as f:
        f.write(f"{num_nodes} {num_edges}\n")
        for edge in edges:
            f.write(f"{edge[0]} {edge[1]}\n")

# Parameters
num_nodes = 100
num_edges = 500  # Adjust this number for desired edge density
output_file = "random_graph.txt"

# Generate random graph
generate_random_graph(num_nodes, num_edges, output_file)
print(f"Random graph with {num_nodes} nodes and {num_edges} edges written to {output_file}")

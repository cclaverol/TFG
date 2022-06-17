# Draws a specified graph g in a specified file.
# Input parameters: the name of the csv file where g is stored, and the name of the file where we want to draw g.

from graph_tool.all import *
import csv
import sys

g = Graph() # Creates an empty directed graph.
vprop_color = g.new_vertex_property("string")

with open(sys.argv[1]) as csv_file:
	csv_reader = csv.reader(csv_file)
	first_row = True

	for row in csv_reader:
		if first_row:
			n = int(row[0]) # In the first row we have the number of nodes.
			g.add_vertex(n) # Adds n nodes to the graph g.
			first_row = False

		else:
			v = int(row[0]) # The node v.
			if int(row[1]): # The immunization status.
				vprop_color[v] = "blue" # Blue for immunized nodes.
			else:
				vprop_color[v] = "red" # Red for vulnerable nodes.

			i = 2
			while int(row[i]) != -1: # The node u.
				e = g.add_edge(v, int(row[i])) # Adds the edge (v, u) to the graph.
				i += 1

graph_draw(g, output=sys.argv[2], vertex_fill_color=vprop_color)
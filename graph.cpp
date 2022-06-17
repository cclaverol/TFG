#include "graph.h"


Graph::Graph(int n) {
	edges = AdjacencyList(n);
}

bool Graph::existsEdge(int i, int j) {
	list<int> e = edges[j];
	list<int>::iterator it;
	for (it = e.begin(); it != e.end(); ++it) {
		if (i == *it)
			return true;
	}
	return false;
}

void Graph::addEdge(int i, int j) {
	if (not existsEdge(i, j)) {
		edges[i].push_back(j);
		edges[j].push_back(i);
	}
}

void Graph::dropEdge(int i, int j) {
	edges[i].remove(j);
	edges[j].remove(i);
}

list<int> Graph::getEdges(int i) {
	return edges[i];
}

bool Graph::isDeleted(int i) {
	list<int> e = edges[i];
	return (e.begin() != e.end() and *(e.begin()) == -1);
}

void Graph::deleteNode(int i) {
	edges[i].clear();
	edges[i].push_back(-1);

	for (int j = 0; j < edges.size(); j++)
		edges[j].remove(i);
}

void Graph::deleteNodes(list<int> l) {
	list<int>::iterator it;
	for (it = l.begin(); it != l.end(); ++it)
		deleteNode(*it);
}
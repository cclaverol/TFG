/**
	Represents an undirected graph.
*/

#include <list>
#include <vector>
using namespace std;

typedef vector<list <int> > AdjacencyList;


class Graph {

	private:
		///For each node i, contains a list of the nodes j such that exists (i,j), or a -1 if i has been deleted.
		AdjacencyList edges;

	public:
		/**
			Creates an empty graph with n nodes.

			@param n The number of nodes.
		*/
		Graph(int n);

		/**
			Says whether there exists an edge (i,j).

			@param i, j The nodes of the edge.
			@returns True if there exists an edge (i,j).
		*/
		bool existsEdge(int i, int j);

		/**
			Adds the edge (i,j) if it does not exist.

			@param i, j The nodes of the edge.
		*/
		void addEdge(int i, int j);

		/**
			Erases the edge (i,j) if it exists.

			@param i, j The nodes of the edge.
		*/
		void dropEdge(int i, int j);

		/**
			Returns the list of nodes connected to i.

			@param i A node.
			@returns The list of nodes j such that exists (i,j).
		*/
		list<int> getEdges(int i);

		/**
			Says whether the node i has been deleted from the graph.

			@param i A node.
			@returns True if the node i has been deleted from the graph.
		*/
		bool isDeleted(int i);

		/**
			Deletes the node i from the graph.

			@param i A node.
		*/
		void deleteNode(int i);

		/**
			Deletes a list of nodes from the graph.
			
			@param i A list of nodes.
		*/
		void deleteNodes(list<int> l);
};
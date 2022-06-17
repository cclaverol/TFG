/**
	Represents the Network Formation with attacks and immunization model.
*/

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <utility>
#include "graph.h"
using namespace std;

typedef list<int> VulnerableRegion;

struct strategy {
	list<int> bought;
	bool immunization;
};


class Model {

	private:
		///The current strategy profile:
		vector<strategy> s;

		///The corresponding graph to s:
		Graph graph;

		///Edge cost and immunization cost:
		double ce, ci;

		///The adversary (true for two attacks):
		bool adv2attacks;



		/**
			Immunizes with probability p the nodes of the current strategy profile s (deimmunizes with probability 1-p).

			@param p The probability that a node immunizes.
		*/
		void initImmunizations (double p);


		/**
			Adds m random edges into the current strategy profile s.

			@param m The number of edges to add.
		*/
		void initEdges(int m);


		/**
			Returns a swapstable best response s'_i for the player i to s_{-i}, as well as the corresponding graph
			to the strategy profile (s_{-i}, s'_i). If the current strategy of s is already a swapstable best
			response, returns the current strategy.

			@param i A player.
			@returns A swapstable best response s'_i and the corresponding graph to (s_{-i}, s'_i).
		*/
		pair <strategy, Graph> swapstableBR(int i); 


		/**
			Tries all the deviations from s consisting of i dropping an edge, both with and without changing
			i's immunization status. If a strategy s'_i such that i has a better utility in (s_{-i}, s'_i) than bu
			is found, updates the best strategy bs, the corresponding utility bu and the graph corresponding to
			(s_{-i}, bs).

			@param[in] i A player.
			@param[out] bs The strategy s'_i such that i in (s_{-i}, s'_i) has the best utility found,
			               if this utility is better than bu.
			@param[out] bg The graph corresponding to (s_{-i}, bs), if a strategy s'_i such that i has a better
						   utility in (s_{-i}, s'_i) than bu is found.
			@param[in, out] bu In: the utility we compare to the utilities found.
			                   Out: the utility of i in the strategy profile (s_{-i}, bs), if a strategy s'_i such
			                        that i has a better utility in (s_{-i}, s'_i) than this one is found.
		*/
		void doDropEdgeDeviations(int i, strategy &bs, Graph &bg, double &bu);


		/**
			Tries all the deviations from s consisting of i purchasing an edge, both with and without changing i's
			immunization status. If a strategy s'_i such that i has a better utility in (s_{-i}, s'_i) than bu is
			found, updates the best strategy bs, the corresponding utility bu and the graph corresponding to
			(s_{-i}, bs).

			@param[in] i A player.
			@param[out] bs The strategy s'_i such that i in  (s_{-i}, s'_i) has the best utility found,
						   if this utility is better than bu.
			@param[out] bg The graph corresponding to (s_{-i}, bs), if a strategy s'_i such that i has a better
						   utility in (s_{-i}, s'_i) than bu is found.
			@param[in, out] bu In: the utility we compare to the utilities found.
							   Out: the utility of i in the strategy profile (s_{-i}, bs), if a strategy s'_i such
							        that i has a better utility in (s_{-i}, s'_i) than this one is found.
		*/
		void doBuyEdgeDeviations(int i, strategy &bs, Graph &bg, double &bu);


		/**
			Tries all the deviations from s consisting of i swapping two edges, both with and without changing i's
			immunization status. If a strategy s'_i such that i has a better utility in (s_{-i}, s'_i) than bu is
			found, updates the best strategy bs, the corresponding utility bu and the graph corresponding to
			(s_{-i}, bs).

			@param[in] i A player.
			@param[out] bs The strategy s'_i such that i in (s_{-i}, s'_i) has the best utility found,
						   if this utility is better than bu.
			@param[out] bg The graph corresponding to (s_{-i}, bs), if a strategy s'_i such that i has a better
						   utility in (s_{-i}, s'_i) than bu is found.
			@param[in, out] bu In: the utility we compare to the utilities found.
							   Out: the utility of i in the strategy profile (s_{-i}, bs), if a strategy s'_i such
							        that i has a better utility in (s_{-i}, s'_i) than this one is found.
		*/
		void doSwapEdgesDeviations(int i, strategy &bs, Graph &bg, double &bu);


		/**
			Tries the deviation from (s_{-i}, cs) consisting of i changing her immunization status. If i has a
			better utility in the new strategy profile (s_{-i}, s'_i) than bu, updates the best strategy bs, the
			corresponding utility bu and the graph corresponding to (s_{-i}, bs).

			@param[in] i A player.
			@param[in] cs A strategy of i.
			@param[out] bs The strategy found after i changes her immunization status, s'_i, if i has a better
						   utility in (s_{-i}, s'_i) than bu.
			@param[in] cg The graph corresponding to (s_{-i}, cs).
			@param[out] bg The graph corresponding to (s_{-i}, bs), if i has a better utility than bu in the
						   strategy profile after she changes her immunization status.
			@param[in, out] bu In: the utility we compare to the utility found after i changes her immunization
								   status.
							   Out: the utility of i in the strategy profile (s_{-i}, bs), if i has a better
							        utility than bu in the strategy profile after she changes her immunization
							        status.
		*/
		void changeImmunizationDeviation(int i, strategy cs, strategy &bs, Graph cg, Graph &bg, double &bu);


		/**
			In strategy si, i buys the edge (i, j) if it does not exist.

			@param[in, out] si In: A strategy of i.
							   Out: The same strategy, where i has bought the edge (i, j).
			@param[in, out] g The graph corresponding to (s_{-i}, si).
			@param[in] i The node that buys the edge.
			@param[in] j The node i buys an edge to.
		*/
		void buyEdge(strategy &si, Graph &g, int i, int j); 


		/**
			In strategy si, i drops the edge (i, j) if it exists.

			@param[in, out] si In: A strategy of i.
							   Out: The same strategy, where i has dropped the edge (i, j).
			@param[in, out] g The graph corresponding to (s_{-i}, si).
			@param[in] i The node that drops the edge.
			@param[in] j The node i drops the edge from.
		*/
		void dropEdge(strategy &si, Graph &g, int i, int j); 


		/**
			In strategy si, i swaps the edge (i, j) to the edge (i,k) if (i,j) exists and (i,k) does not.

			@param[in, out] si In :A strategy of i.
							   Out: The same strategy, where i has swapped the edge (i, j) for (i, k).
			@param[in, out] g The graph corresponding to (s_{-i}, si).
			@param[in] i The node that swaps the edge.
			@param[in] j The node i drops the edge from.
			@param[in] k The node i buys an edge to.
		*/
		void swapEdges(strategy &si, Graph &g, int i, int j, int k);


		/**
			If the utility of i in the strategy profile (s_{-i}, cs) is better than bu, updates the best strategy
			bs, the corresponding utility bu and the graph corresponding to (s_{-i}, bs).

			@param[in] i A player.
			@param[in] cs A strategy of i.
			@param[out] bs The strategy cs, if i has a better utility in the strategy profile (s_{-i}, cs) than bu.
			@param[in] cg The graph corresponding to (s_{-i}, cs).
			@param[out] bg The graph cg, if i has a better utility in the strategy profile (s_{-i}, cs) than bu.
			@param[in, out] bu In: the utility we compare to the utility of cs.
							   Out: bs's utility, if i has a better utility in the strategy profile (s_{-i}, cs)
							        than bu.
		*/
		void updateBestStrategy(int i, strategy cs, strategy &bs, Graph cg, Graph &bg, double &bu);


		/**
			Returns the utility of i in the strategy profile (s_{-i}, si).

			@param i A player.
			@param si A strategy of i.
			@param g The graph corresponding to the strategy profile (s_{-i}, si).
			@returns The utility of i in the strategy profile (s_{-i}, si).
		*/
		double calculateUtility(int i, strategy si, Graph g);


		/**
			Returns the expected size of the connected component of i in the graph g, with the list of targeted
			regions tr, after the adversary makes a single attack.
			
			@param i A player.
			@param g A graph.
			@param tr A list of the targeted regions of the strategy profile to which corresponds the graph g,
					  of at least size 1.
			@returns The expected size of i's connected component in the graph g after the adversary makes
					 the attack.
		*/
		double calculateExpectedSzCC1attack(int i, Graph g, list<VulnerableRegion> tr);
		

		/**
			Returns the expected size of the connected component of i in the graph g, corresponding to the
			strategy profile (s_{-i}, si), after the adversary makes two attacks.
			
			@param i A player.
			@param si A strategy of player i.
			@param g The graph corresponding to the strategy profile (s_{-i}, si).
			@param vr A list of the vulnerable regions of  maximum size of the strategy profile (s_{-i}, si),
					  of at least size 1.
			@returns The expected size of i's connected component on the graph g after the adversary makes
					 the attacks.
		*/
		double calculateExpectedSzCC2attacks(int i, strategy si, Graph g, list<VulnerableRegion> vr);


		/**
			Returns the expected size of the connected component of i in the graph g, corresponding to the
			strategy profile (s_{-i}, si), which only has a vulnerable region of maximum size, after the
			adversary makes two attacks.
			
			@param i A player.
			@param si A strategy of player i.
			@param g The graph corresponding to the strategy profile (s_{-i}, si).
			@param vr A list which only has an element, the vulnerable region of maximum size of the strategy
					  profile (s_{-i}, si).
			@returns The expected size of i's connected component on the graph g after the adversary makes
					 the attacks.
		*/
		double calculateExpectedSzCC1VRmaxSz(int i, strategy si, Graph g, list<VulnerableRegion> vr);


		/**
			Returns the expected size of the connected component of i in the graph g, which has more than one
			vulnerable region of maximum size, after the adversary makes two attacks.
			
			@param i A player.
			@param g The graph.
			@param tr A list of the targeted regions of the strategy profile to which corresponds the graph g,
					  of at least size 2.
			@returns The expected size of i's connected component on the graph g after the adversary makes
					 the attacks.
		*/
		double calculateExpectedSzCCmoreVRmaxSz(int i, Graph g, list<VulnerableRegion> tr);


		/**
			Returns the list of vulnerable regions of maximum size of the strategy profile (s_{-i}, si).
			
			@param i A player.
			@param si A strategy of player i.
			@param g The graph corresponding to (s_{-i}, si).
			@returns The list of vulnerable regions of maximum size.
		*/
		list<VulnerableRegion> getVulnerableRegionsMaxSize(int i, strategy si, Graph g);


		/**
			Returns the list of vulnerable regions of the strategy profile (s_{-i}, si).
			
			@param i A player.
			@param si A strategy of player i.
			@param g The graph corresponding to (s_{-i}, si).
			@returns The list of vulnerable regions.
		*/
		list<VulnerableRegion> getVulnerableRegions(int i, strategy si, Graph g);


		/**
			Deletes from the graph corresponding to the strategy profile (s_{-i}, si) the immunized nodes of
			such strategy profile.
			
			@param[in] i A player.
			@param[in] si A strategy of player i.
			@param[in, out] g In: the graph corresponding to (s_{-i}, si).
							  Out: the same graph without the immunized nodes of such strategy profile.
		*/
		void deleteImmunizedNodes(int i, strategy si, Graph &g);


		/**
			Returns the size of i's connected component in the graph g.
			
			@param i The node of the graph.
			@param g The graph.
			@returns The size of i's connected component in g.
		*/
		int getConnectedComponentSize(int i, Graph g);


		/**
			Calculates i's connected component in graph g.
			
			@param[in] i The node of the graph.
			@param[in, out] visited In: the already visited nodes of the connected component are true.
									Out: all the nodes of the connected component are true.
			@param[in, out] CC In: the connected component of i that has already been calculated.
							   Out: the whole connected component of i.
			@param[in] g The graph.
		*/
		void getConnectedComponentUtil(int i, vector<bool> &visited, list<int> &CC, Graph g);



	public:

		/**
			Creates a random strategy profile and its corresponding graph.

			@param n The number of players.
			@param m The number of edges.
			@param p The probability that a player immunizes.
		*/
		Model(int n, int m, double p);


		/**
			Exports the graph corresponding to the current strategy profile s as a csv file.
			First row is the number of nodes. Then, for each row, first column is the node i, second column i's immunization
			status, and the rest of columns the nodes j belonging to x_i (the nodes i has bought an edge to). The row ends with a -1.

			@param nameFile The name of the file.
		*/
		void exportGraph(string nameFile);


		/**
			Runs a swapstable best response dynamics, starting from the current strategy profile s.
			
			@param ce The cost of the edges.
			@param ci The immunization cost.
			@param adv2attacks The adversary (true for the one that makes 2 attacks)
		*/
		void dynamics(double ce, double ci, bool adv2attacks);
};
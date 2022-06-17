#include <iostream>
#include <sstream>
#include "model.h"
#include "assert.h"
using namespace std;

int main() {
	int n, m;
	cout << "Enter number of nodes of the graph:" << endl;
	cin >> n;
	assert(n > 0);
	cout << "Enter number of edges of the inital graph, bigger than 0:" << endl;
	cin >> m;
	assert(m > 0 and m < ((n * (n-1)) / 2));

	double p;
	cout << "Enter the probability that a node is immunized in the initial graph (between 0 and 1):" << endl;
	cin >> p;
	assert(p >= 0 and p <= 1);

	Model model(n, m, p);

	stringstream nameFileInitial;
	nameFileInitial << "initial_graph_n" << n << "_m" << m << "_p" << p << ".csv";
	model.exportGraph(nameFileInitial.str());

	Model auxModel = model;

	int adversary;
	cout << "Choose the adversary. Enter 1 for the adversary that attacks a single player, and 2 for the adversary which attacks two players:" << endl;
	cin >> adversary;
	assert(adversary == 1 or adversary == 2);
	bool adv2attacks = (adversary == 2);

	double ce, ci;
	cout << "Enter non-negative Ce and Ci:" << endl;
	while (cin >> ce >> ci) {
		assert(ce >= 0 and ci >= 0);

		model = auxModel;

		model.dynamics(ce, ci, adv2attacks);

		stringstream nameFileFinal;
		nameFileFinal << "final_graph_n" << n << "_m" << m << "_p" << p << "_ce" << ce << "_ci" << ci << "_" << adversary << "attacks.csv";
		model.exportGraph(nameFileFinal.str());

		cout << "Enter non-negative Ce and Ci:" << endl;
	}
}
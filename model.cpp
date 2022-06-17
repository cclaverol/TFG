#include "model.h"

Model::Model(int n, int m, double p) : graph(n)
{
	s = vector<strategy>(n);

	initImmunizations(p);
	initEdges(m);
}

void Model::exportGraph(string nameFile) {
	ofstream myfile;
	myfile.open(nameFile);

	myfile << s.size() << endl;
	for (int i = 0; i < s.size(); ++i) {
		myfile << i << "," ;

		myfile << s[i].immunization;

		list<int> bought = s[i].bought;
		list<int>::iterator it;
		for (it = bought.begin(); it != bought.end(); ++it) {
			myfile << "," << *it;
		}

		myfile << ",-1" << endl;
	}

	myfile.close();
}

void Model::dynamics(double ce, double ci, bool adv2attacks) {
	this->ce = ce;
	this->ci = ci;
	this->adv2attacks = adv2attacks;

	bool equilibrium;
	do {
		equilibrium = true;

		for (int i = 0; i < s.size(); ++i) {
			strategy si = s[i];
			pair<strategy, Graph> swBR = swapstableBR(i);
			strategy sbr = swBR.first;

			bool sameStrategy =  (si.bought == sbr.bought and si.immunization == sbr.immunization);

			if (not sameStrategy) {
				equilibrium = false; //The strategy profile s is not a swapstable equilibrium
				s[i] = sbr;
				graph = swBR.second;
			}
		}
	}
	while (not equilibrium);
}


void Model::initImmunizations(double p) {
	srand(time(NULL));

	for (int i = 0; i < s.size(); ++i) {
		if (rand() % 100 < p*100) //True with probability p
			s[i].immunization = true;
		else
			s[i].immunization = false;
	}
}

void Model::initEdges(int m) {
	srand(time(NULL));

	int n = s.size();

	for(int i = 0; i < m; ++i) {
		bool sameNodes;
		bool existsEdge;

		do {
			sameNodes = false;
			existsEdge = false;

			int x = rand() % n;
			int y = rand() % n;

			if (x == y)
				sameNodes = true;

			else if (graph.existsEdge(x, y))
				existsEdge = true;

			else {
				if ((rand() % 2) == 0)
					buyEdge(s[x], graph, x, y); //x buys the edge
				else
					buyEdge(s[y], graph, y, x); //y buys the edge
			}
		}
		while(sameNodes or existsEdge);
	}
}

pair <strategy, Graph> Model::swapstableBR(int i) {
	strategy cs = s[i]; //Current strategy of i, s_i
	strategy bs = cs; //Best strategy of i found. Initialized to s_i

	Graph cg = graph; //The graph corresponding to s
	Graph bg = cg; //The graph corresponding to (s_{-i}, bs). Initialized to the graph corresponding to s

	double bu = calculateUtility(i, cs, cg); //The utility of i in (s_{-i}, bs). Initialized to i's utility in s

	changeImmunizationDeviation(i, cs, bs, cg, bg, bu);

	doDropEdgeDeviations(i, bs, bg, bu);

	doBuyEdgeDeviations(i, bs, bg, bu);

	doSwapEdgesDeviations(i, bs, bg, bu);

	return make_pair(bs, bg);
}

void Model::doDropEdgeDeviations(int i, strategy &bs, Graph &bg, double &bu) {
	list<int> bought = s[i].bought;
	list<int>::iterator it;
	for (it = bought.begin(); it != bought.end(); ++it) {
		//For each edge i has bought

		strategy cs = s[i]; //Current strategy of i, s_i
		Graph cg = graph; //The graph corresponding to s

		dropEdge(cs, cg, i, *it);

		updateBestStrategy(i, cs, bs, cg, bg, bu);

		changeImmunizationDeviation(i, cs, bs, cg, bg, bu);
	}
}

void Model::doBuyEdgeDeviations(int i, strategy &bs, Graph &bg, double &bu) {
	for (int j = 0; j < s.size(); j++) {
		if (not graph.existsEdge(i, j)) {
			//For each edge i has not bought

			strategy cs = s[i]; //Current strategy of i, s_i
			Graph cg = graph; //The graph corresponding to s

			buyEdge(cs, cg, i, j);

			updateBestStrategy(i, cs, bs, cg, bg, bu);

			changeImmunizationDeviation(i, cs, bs, cg, bg, bu);
		}
	}
}

void Model::doSwapEdgesDeviations(int i, strategy &bs, Graph &bg, double &bu) {
	list<int> bought = s[i].bought;
	list<int>::iterator it;
	for (it = bought.begin(); it != bought.end(); ++it) {
		//For each edge i has bought

		for (int j = 0; j < s.size(); ++j) {
			if (not graph.existsEdge(i, j)) {
				//For each edge i has not bought

				strategy cs = s[i]; //Current strategy of i, s_i
				Graph cg = graph; //The graph corresponding to s

				swapEdges(cs, cg, i, *it, j);

				updateBestStrategy(i, cs, bs, cg, bg, bu);

				changeImmunizationDeviation(i, cs, bs, cg, bg, bu);
			}
		}
	}
}

void Model::changeImmunizationDeviation(int i, strategy cs, strategy &bs, Graph cg, Graph &bg, double &bu)  {
	cs.immunization = not cs.immunization;
	updateBestStrategy(i, cs, bs, cg, bg, bu);
}

void Model::buyEdge(strategy &si, Graph &g, int i, int j) {
	if (not g.existsEdge(i,j)) {
		si.bought.push_back(j);
		g.addEdge(i, j);
	}
}

void Model::dropEdge(strategy &si, Graph &g, int i, int j) {
	si.bought.remove(j);
	g.dropEdge(i, j);
}

void Model::swapEdges(strategy &si, Graph &g, int i, int j, int k) {
	if (g.existsEdge(i,j) and not g.existsEdge(i,k)) {
		dropEdge(si, g, i, j);
		buyEdge(si, g, i, k);
	}
}

void Model::updateBestStrategy(int i, strategy cs, strategy &bs, Graph cg, Graph &bg, double &bu) {
	double cu = calculateUtility(i, cs, cg);

	if (cu > bu) {
		bs = cs;
		bg = cg;
		bu = cu;
	}
}

double Model::calculateUtility(int i, strategy si, Graph g) {
	double expsz; //The expected size of i's connected component after the attack.
	list<VulnerableRegion> vr = getVulnerableRegionsMaxSize(i, si, g);

	if (vr.size() == 0) //No vulnerable regions, so the adversary makes no attack
		expsz = getConnectedComponentSize(i, g);

	else if (not adv2attacks) //The adversary is the one that makes a single attack
		expsz = calculateExpectedSzCC1attack(i, g, vr);

	else //The adversary is the one that makes two attacks
		expsz = calculateExpectedSzCC2attacks(i, si, g, vr);

	list<int> xi = si.bought;
	bool yi = si.immunization;
	return expsz - (xi.size() * ce + yi * ci);
}

double Model::calculateExpectedSzCC1attack(int i, Graph g, list<VulnerableRegion> tr) {
	double expSz = 0; //The expected size of i's connected component after the attack
	double probT = 1.0/tr.size(); //The probability of attack to a targeted region

	list<VulnerableRegion>::iterator it;
	for (it = tr.begin(); it != tr.end(); ++it) {
		//For each targeted region

		VulnerableRegion t = *it;

		Graph aux = g;
		aux.deleteNodes(t); //Delete targeted region t

		expSz += probT * getConnectedComponentSize(i, aux); //The size of i's connected component post-attack to t is the size of
															//i's connected component in the graph where we have deleted t
	}
	return expSz;
}

double Model::calculateExpectedSzCC2attacks(int i, strategy si, Graph g, list<VulnerableRegion> vr) {
	if (vr.size() == 1) { //If there is only one vulnerable region of maximum size
		return calculateExpectedSzCC1VRmaxSz(i, si, g, vr);
	}

	else { //If there are more than one vulnerable regions of maximum size
		return calculateExpectedSzCCmoreVRmaxSz(i, g, vr);
	}
}

double Model::calculateExpectedSzCC1VRmaxSz(int i, strategy si, Graph g, list<VulnerableRegion> vr) {
	VulnerableRegion t = *(vr.begin()); //The vulnerable region of maximum size t

	g.deleteNodes(t); //Delete the vulnerable region t

	vr = getVulnerableRegionsMaxSize(i, si, g); //The vulnerable regions of the next maximum size

	if (vr.size() == 0) //The adversary only attacks a vulnerable region, t
		return getConnectedComponentSize(i, g); //The size of i's connected component post-attack to t is the size of i's
												//connected component in the graph where we have deleted t

	else //The adversary attacks the vulnerable region t and one vulnerable region of vr
		return calculateExpectedSzCC1attack(i, g, vr); //The expected size of i's connected component post-attack to t and a
													   //vulnerable region of vr is the expected size of i's connected component
													   //in the graph where we have deleted t, with targeted regions vr
}

double Model::calculateExpectedSzCCmoreVRmaxSz(int i, Graph g, list<VulnerableRegion> tr) {
	double expSz = 0; //The expected size of i's connected component after the attack
	double probT = 2.0/(tr.size()*(tr.size()-1)); //The probability of attack to two targeted regions

	list<VulnerableRegion>::iterator it;
	for (it = tr.begin(); it != tr.end(); ++it) {
		//For each targeted region

		VulnerableRegion t1 = *it;

		Graph aux = g;
		aux.deleteNodes(t1); //Delete targeted region t1

		list<VulnerableRegion>::iterator it2 = it;
		++it2;
		for (; it2 != tr.end(); ++it2) {
			//For each targeted region after t1

			VulnerableRegion t2 = *it2;

			Graph aux2 = aux;
			aux2.deleteNodes(t2); //Delete targeted region t2

			int ccsz = getConnectedComponentSize(i, aux2); //The size of i's connected component post-attack to t is the size of
														   //i's connected component in the graph where we have deleted t1 and t2
			expSz += probT * ccsz;
		}
	}
	return expSz;
}


list<VulnerableRegion> Model::getVulnerableRegionsMaxSize(int i, strategy si, Graph g) {
	list<VulnerableRegion> vr = getVulnerableRegions(i, si, g);
	list<VulnerableRegion> tr;
	int max = 0;

	list<VulnerableRegion>::iterator it;
	for (it = vr.begin(); it != vr.end(); ++it) {
		int size = (*it).size();
		if (size > max) {
			max = size;
			tr.clear();
			tr.push_back(*it);
		}
		else if (size == max)
			tr.push_back(*it);
	}
	return tr;
}

list<VulnerableRegion> Model::getVulnerableRegions(int i, strategy si, Graph g) {
	deleteImmunizedNodes(i, si, g);

	list<VulnerableRegion> vr;
	vector<bool> visited(s.size(), false);

	//Puts in vr all the connected components of the graph g, where we have deleted the immunized nodes
	for (int j = 0; j < s.size(); ++j) {
		if (not visited[j] and not g.isDeleted(j)) {
			VulnerableRegion v;
			getConnectedComponentUtil(j, visited, v, g);
			vr.push_back(v);
		}
	}
	return vr;
}

void Model::deleteImmunizedNodes(int i, strategy si, Graph &g) {
	for (int j = 0; j < s.size(); ++j) {
		bool imm; //i is immunized in (s_{-i}, si)
		if (j == i)
			imm = si.immunization;
		else
			imm = s[j].immunization;

		if (imm)
			g.deleteNode(j);
	}
}

int Model::getConnectedComponentSize(int i, Graph g) {
	vector<bool> visited(s.size(), false);
	list<int> CC;

	if (not g.isDeleted(i)) //If i has been deleted, her connected component size is 0
		getConnectedComponentUtil(i, visited, CC, g);

	return CC.size();
}

void Model::getConnectedComponentUtil(int i, vector<bool> &visited, list<int> &CC, Graph g) {
	visited[i] = true;
	CC.push_back(i);

	list<int>::iterator it;
	list<int> edgesi = g.getEdges(i);
	for(it = edgesi.begin(); it != edgesi.end(); ++it) {
		if (not visited[*it])
			getConnectedComponentUtil(*it, visited, CC, g);
	}
}
#include "boost/shared_array.hpp"
#include "boost/format.hpp"
#include <fstream>
#include <algorithm>
#include <queue>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;
using namespace boost;

/* input file format:

   VertexNum
   EdgeNum	
   point point weight
   ....
   (repeat EdgeNum)
*/

class Edge {
public:
	Edge(int tv, int tw, double tweight):
		v{tv}, w{tw}, weight{tweight} {}
	double getWeight()const {return weight;}
	int either()const {return v;}
	int other(int vertex)const {return vertex == v? w:v;}
	const string toString()const { 
		return string{str(format("%d - %d  %.2f") % v % w % weight)}; }
private:
	int v, w;
	double weight;
};

class EdgeWeightedGraph {
public:
	EdgeWeightedGraph(int V)
		:vertex{V}, edge{0}, adj{new vector<Edge>[V]} {}
	int getVertex()const {return vertex;}
	int getEdege()const {return edge; }
	EdgeWeightedGraph(istream &in) {
		int v; in >> v;
		new(this) EdgeWeightedGraph{v};
		int e; in >> e;
		while (e--) {
			int v, w;
			double weight;
			in >> v >> w >> weight;
			addEdge(Edge{v, w, weight});
		}
	}
	void addEdge(Edge e) {
		int v = e.either();
		int w = e.other(v);
		adj[v].push_back(e);
		adj[w].push_back(e);
		edge++;
	}
	const vector<Edge>& adjEdge(int v)const {return adj[v];}
private:
	int vertex, edge;
	shared_array<vector<Edge>> adj;
};

class LazyPrimMST {
public:
	LazyPrimMST(const EdgeWeightedGraph &G)
		:mask{new bool[G.getVertex()]}, pq { 
			[](const Edge&e1, const Edge&e2) {
				return e1.getWeight() > e2.getWeight();}}{ 
		for (int i = 0; i < G.getVertex(); i++) mask[i] = false;
		visit(G, 0);
		while (!pq.empty()) {
			int v = pq.top().either();
			int w = pq.top().other(v);
			if (mask[v] && mask[w]) {
				pq.pop(); continue;
			} else {
				mst.push_back(pq.top()); pq.pop();
			}
			if (!mask[v]) visit(G, v);
			if (!mask[w]) visit(G, w);
		}
	}
	void visit(const EdgeWeightedGraph &e, int v) {
		mask[v] = true;
		for (const auto& edge: e.adjEdge(v))
			if (!mask[edge.other(v)])
				pq.push(edge);
	}
	const vector<Edge>& getMst()const {return mst;}
private:
	priority_queue<Edge, vector<Edge>,
		function<bool(const Edge&, const Edge&)>> pq;
	shared_array<bool> mask;
	vector<Edge> mst;
};

int main() {
	ifstream fin{"tinyEWG.txt"};
	EdgeWeightedGraph Graph{fin};
	vector<Edge> mst{LazyPrimMST{Graph}.getMst()};
	for (const auto &e: mst)
		cout << e.toString() << endl;
	fin.close();
	return 0;
}

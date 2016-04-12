#include "boost/shared_array.hpp"
#include "boost/format.hpp"
#include <fstream>
#include <algorithm>
#include <queue>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

namespace Graph {
	using namespace std;
	using namespace boost;
	class WeightedQuckUnionUF {
	public:
		WeightedQuckUnionUF(int N)
			:count{N}, id{new int[N]} {
				for (int i = 0; i < N; i++) id[i] = -1;
			}
		bool connected(int p, int q) {return find(p) == find(q);}
		int find(int p) {
			while (id[p] >= 0) p = id[p];
			return p;
		}
		void Union(int p, int q) {
			int i = find(p), j = find(q);
			if (i == j) return;
			if (id[i] < id[j]) {id[i] += id[j]; id[j] = i;}
			else {id[j] += id[i]; id[i] = j;}
		}
	private:
		int count;
		shared_array<int> id;
	};

	class Edge {
	public:
		Edge(int tv, int tw, double tweight):
			v{tv}, w{tw}, weight{tweight} {}
		double getWeight()const {return weight;}
		int either()const {return v;}
		int other(int vertex)const {return vertex == v? w:v;}
		const string toString()const { 
			return string{str(format("%d - %d  %.2f") % v % w % weight)};
		}
	private:
		int v, w;
		double weight;
	};

	/* input file format:

	   VertexNum
	   EdgeNum	
	   point point weight
	   ....
	   (repeat EdgeNum)
	*/
	class EdgeWeightedGraph {
	public:
		EdgeWeightedGraph(int V)
			:vertex{V}, adj{new vector<Edge>[V]} {}
		int getVertex()const {return vertex;}
		int getEdege()const {return edge.size();}
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
			edge.push_back(e);
			int v = e.either();
			int w = e.other(v);
			adj[v].push_back(e);
			adj[w].push_back(e);
		}
		const vector<Edge>& adjEdge(int v)const {return adj[v];}
		typedef typename vector<Edge>::const_iterator iterator;
		iterator begin() const {return edge.begin();}
		iterator end() const {return edge.end();}
	private:
		int vertex;
		shared_array<vector<Edge>> adj;
		vector<Edge> edge;
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
		const vector<Edge>& getMst()const {return mst;}
	private:
		void visit(const EdgeWeightedGraph &e, int v) {
			mask[v] = true;
			for (const auto& edge: e.adjEdge(v))
				if (!mask[edge.other(v)])
					pq.push(edge);
		}
		priority_queue<Edge, vector<Edge>,
			function<bool(const Edge&, const Edge&)>> pq;
		shared_array<bool> mask;
		vector<Edge> mst;
	};

	class KruskalMST {
	public:
		KruskalMST(const EdgeWeightedGraph &G)
			:uf{G.getVertex()}, pq{[](const Edge&e1, const Edge&e2) {
				return e1.getWeight() > e2.getWeight();}}{
			for (const auto &e: G) pq.push(e);
			while (!pq.empty()) {
				Edge edge{pq.top()}; pq.pop();
				int v = edge.either();
				int w = edge.other(v);
				if (uf.connected(v, w)) continue;
				uf.Union(v, w);
				mst.push_back(edge);
			}
		}
		const vector<Edge>& getMst()const {return mst;}
	private:
		vector<Edge> mst;
		WeightedQuckUnionUF uf;
		priority_queue<Edge, vector<Edge>,
			function<bool(const Edge&, const Edge&)>> pq;
	};
}

int main(int argc, char *argv[], char *env[]) {
	using namespace Graph;
	using namespace std;
	using namespace boost;
	ifstream fin{"tinyEWG.txt"};
	EdgeWeightedGraph Graph{fin};
	/*
	vector<Edge> mst{LazyPrimMST{Graph}.getMst()};
	for (const auto &e: mst)
		cout << e.toString() << endl;
	*/
	vector<Edge> mst{KruskalMST{Graph}.getMst()};
	for (const auto &e: mst)
		cout << e.toString() << endl;
	fin.close();
	return 0;
}

/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include <queue>
#include <map>

class Backtrack {
private:
    int total_embedding;
    std::map<int, std::vector<std::pair<Vertex, Vertex>>> hashmap;

public:
    Backtrack();

    ~Backtrack();


    void PrintAllMatches(const Graph& data, const Graph& query,
        const CandidateSet& cs);

    bool find_Embedding(const Vertex data_v, const Vertex query_v, std::vector<std::pair<Vertex, Vertex>> embedding,
        std::vector<std::vector<Vertex>> dag, std::vector<std::vector<Vertex>> dag_invert,
        int can_visit[],
        const Graph& data, const Graph& query, const CandidateSet& cs);

    Vertex SelectRoot(const Graph& query, const CandidateSet& cs);

    void BuildDAG(Vertex root, const Graph& query, std::vector<std::vector<Vertex>>& DAG,
        std::vector<std::vector<Vertex>>& DAG_invert);

    void InsertionSort(std::pair<Vertex, int> query[], int p, int r);

    void Swap(std::pair<Vertex, int> query[], int p, int r);

    int Median(std::pair<Vertex, int> query[], int p, int q, int r);

    int Partition(std::pair<Vertex, int> query[], int p, int r);

    void QuickSort(std::pair<Vertex, int> query[], int p, int r);

    void Set_total_embedding(int num) {
        total_embedding = num;
    };

    int Get_total_embedding() {
        return total_embedding;
    };





};


#endif  // BACKTRACK_H_
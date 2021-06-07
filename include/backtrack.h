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

public:
    Backtrack();

    ~Backtrack();


    void PrintAllMatches(const Graph& data, const Graph& query,
        const CandidateSet& cs);

    bool find_Embedding(Vertex data_v, Vertex query_v, Vertex embedding[],
        std::vector<std::vector<Vertex>> dag, std::vector<std::vector<Vertex>> dag_invert,
        int can_visit[],
        const Graph& data, const Graph& query, const CandidateSet& cs, int embedding_size);

    Vertex SelectRoot(const Graph& query, const CandidateSet& cs);

    void BuildDAG(Vertex root, const Graph& query, std::vector<std::vector<Vertex>>& DAG,
        std::vector<std::vector<Vertex>>& DAG_invert);

    void Set_total_embedding(int num) {
        total_embedding = num;
    };

    int Get_total_embedding() const {
        return total_embedding;
    };
};


#endif  // BACKTRACK_H_
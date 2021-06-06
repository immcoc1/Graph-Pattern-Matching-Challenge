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


    int Get_HashVal(std::vector<std::pair<Vertex, Vertex>> emb) {
        int h_val = 0;
        for (int i = 0; i < (int)emb.size(); i++) {
            h_val += (emb[i].first + 1) * emb[i].second;
        }
        return h_val % 200000;
    }

    bool Insert_Hash(int h_val, std::vector<std::pair<Vertex, Vertex>> emb) {
        while (true) {
            if (hashmap.find(h_val) == hashmap.end()) {
                // hash.insert(hash_map<int, std::vector<std::pair<Vertex, Vertex>>>::value_type(h_val, emb));
                hashmap.insert(std::pair<int, std::vector<std::pair<Vertex, Vertex>>>(h_val, emb));
                return true;
            }
            else {

                if (Compare_Embedding(hashmap.find(h_val)->second, emb)) {
                    return false;
                }
                else {
                    h_val = (h_val + 7) % 200000;
                }
            }
        }
    }

    bool Compare_Embedding(std::vector<std::pair<Vertex, Vertex>> emb1, std::vector<std::pair<Vertex, Vertex>> emb2) {
        for (int i = 0; i < (int)emb1.size(); i++) {
            if (emb1[i].second != emb2[i].second) {
                return false;
            }
        }
        return true;
    }



};


#endif  // BACKTRACK_H_
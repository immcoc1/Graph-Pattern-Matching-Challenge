/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"


Backtrack::Backtrack() {}

Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph& data, const Graph& query,
    const CandidateSet& cs) {
    // implement your code here.

    // 1. Build DAG
    Set_total_embedding(0);

    // 1-1) initial Candidate Set에서의 canididate set의 크기 / query에서의 degree가 가장 작은 vertex를 root로 설정
    size_t numQueryVertice = query.GetNumVertices();

    Vertex root = SelectRoot(query, cs);

    std::vector<std::vector<Vertex>> DAG; // DAG
    std::vector<std::vector<Vertex>> DAG_invert; // DAG Invert

    DAG.resize(numQueryVertice);
    DAG_invert.resize(numQueryVertice);

    // 1-2) DAG와 DAG_invert 생성
    BuildDAG(root, query, DAG, DAG_invert);


    // 2. Embedding 찾기
    std::cout << "t " << numQueryVertice << "\n";

    for (size_t i = 0; i < cs.GetCandidateSize(root); i++) {
        Vertex first_candidate = cs.GetCandidate(root, i);

        int* can_visit;
        can_visit = (int*)calloc(numQueryVertice, sizeof(int));
        std::vector<std::pair<Vertex, Vertex>> embedding;

        if (find_Embedding(first_candidate, root, embedding, DAG, DAG_invert, can_visit, data, query, cs)) // true return은 10만개를 다 찾았음을 의미
            return;

        free(can_visit);
    }

}


bool Backtrack::find_Embedding(const Vertex data_v, const Vertex query_v, std::vector<std::pair<Vertex, Vertex>> embedding,
    std::vector<std::vector<Vertex>> dag, std::vector<std::vector<Vertex>> dag_invert,
    int can_visit[],
    const Graph& data, const Graph& query, const CandidateSet& cs) {
    /* ---------------------------------------------------------------------------------------------
        1. DAG_invert의 query vertex에 대해 parent이면서 data vertex가 이웃한 쌍이 embedding에 없으면 종료
       ---------------------------------------------------------------------------------------------*/

       //    for (size_t i = 0; i < dag_invert[query_v].size(); i++) {
       //        bool isFound = false; // true면 embedding에 query의 parent가 존재함을 의미
       //        Vertex curQuery = dag_invert[query_v][i];
       //
       //        for (Vertex j = 0; j < (int) embedding.size(); j++) {
       //            std::pair<Vertex, Vertex> curPair = embedding[j];
       //
       //            if (curPair.first == curQuery && data.IsNeighbor(curPair.second, data_v)) {
       //                isFound = true;
       //                break;
       //            }
       //        }
       //
       //        if (!isFound) {
       //            return false;
       //        }
       //    }

           /* ---------------------------------------------------------------------------------------------
               2. embedding에 query vertex, data vertex pair 넣기
              ---------------------------------------------------------------------------------------------*/
    embedding.emplace_back(query_v, data_v);

    int* can_visit_copy;
    can_visit_copy = (int*)calloc(query.GetNumVertices(), sizeof(int));
    std::copy(can_visit, can_visit + query.GetNumVertices(), can_visit_copy);

    can_visit_copy[query_v] = 1;

    /* ---------------------------------------------------------------------------------------------
        3. embedding size = query_size면 출력 후 종료
       ---------------------------------------------------------------------------------------------*/
    if (embedding.size() == query.GetNumVertices()) {
        sort(embedding.begin(), embedding.end());
        int hash_val = Get_HashVal(embedding);
        if (Insert_Hash(hash_val, embedding)) {
            std::cout << "a ";
            for (Vertex i = 0; i < (int)embedding.size(); i++) {
                std::cout << embedding[i].second << " ";
            }
            std::cout << std::endl;

            free(can_visit_copy);

            Set_total_embedding(Get_total_embedding() + 1);
            //std::cout << Get_total_embedding() << std::endl;
            if (Get_total_embedding() == 100000)
                return true;
            else
                return false;
        }
        else {
            return false;
        }

    }

    /* ---------------------------------------------------------------------------------------------
        4. 다음 방문할 vertex 정하기
       ---------------------------------------------------------------------------------------------*/

    int next_visit_count = 0;

    // 4-1) 먼저 해당 query_vertex에 연결된 vertex들 can_visit에 추가
    for (size_t i = 0; i < dag[query_v].size(); i++) {
        if (can_visit_copy[dag[query_v][i]] == 0) {
            can_visit_copy[dag[query_v][i]] = 2; // 방문할 수 있음
            next_visit_count++;
        }
    }

    // 4-2) can_visit에서 가장 작은 값에 해당하는 query vertex 방문
    std::pair<Vertex, int>* next_visit;

    next_visit = (std::pair<Vertex, int> *) calloc(query.GetNumVertices(), sizeof(std::pair<Vertex, int>));

    int count = 0;

    for (Vertex i = 0; i < (int)query.GetNumVertices(); i++) {
        if (can_visit_copy[i] == 2) {
            next_visit[count++] = std::pair<Vertex, int>(i, cs.GetCandidateSize(i));
        }
    }

    next_visit = (std::pair<Vertex, int> *) realloc(next_visit, count * sizeof(std::pair<Vertex, int>)); // 실제 사이즈로 재할당

    // 가장 candidate set이 작은 순서대로 방문하기 위해 정렬
    QuickSort(next_visit, 0, count - 1);

    for (int l = 0; l < count; l++) {
        Vertex min_vertex = next_visit[l].first;
        bool can_not_visit = false;

        for (int i = 0; i < next_visit[l].second; i++) {
            Vertex first_candidate = cs.GetCandidate(min_vertex, i);

            for (size_t j = 0; j < dag_invert[min_vertex].size(); j++) {
                bool isFound = false; // true면 embedding에 query의 parent가 존재함을 의미
                Vertex curQuery = dag_invert[min_vertex][j];

                for (Vertex k = 0; k < (int)embedding.size(); k++) {
                    std::pair<Vertex, Vertex> curPair = embedding[k];

                    if (curPair.first == curQuery && data.IsNeighbor(curPair.second, first_candidate)) {
                        isFound = true;
                        break;
                    }
                }

                if (!isFound) {
                    can_not_visit = true;
                }
            }

            if (!can_not_visit) {
                // embedding 복사
                std::vector<std::pair<Vertex, Vertex>> embedding_copy;
                embedding_copy.resize(embedding.size());
                std::copy(embedding.begin(), embedding.end(), embedding_copy.begin());
                /*for (int k = 0; k < query.GetNumVertices(); k++) {
                    std::cout << can_visit_copy[k]<< " ";
                }*/
                //td::cout << std::endl;
                if (find_Embedding(first_candidate, min_vertex, embedding_copy, dag, dag_invert, can_visit_copy, data, query,
                    cs))
                    return true;
            }
        }

        if (!can_not_visit)
            break;
    }

    free(can_visit_copy);
    free(next_visit);
    return false; // 아직 10만개를 다 찾지 못했으므로 false
}

Vertex Backtrack::SelectRoot(const Graph& query, const CandidateSet& cs) {
    // root는 일단 Vertex 0으로 가정
    Vertex root = 0;
    size_t size = query.GetNumVertices();
    double root_data = (double)cs.GetCandidateSize(0) / (double)query.GetDegree(0);

    // query의 vertex를 한번씩 훑으면서 계산
    for (Vertex i = 1; i < (int)size; i++) {
        double v_data = (double)cs.GetCandidateSize(i) / (double)query.GetDegree(i);

        if (root_data > v_data) {
            root = i;
            root_data = v_data;
        }
    }

    return root;
}

void Backtrack::BuildDAG(Vertex root, const Graph& query, std::vector<std::vector<Vertex>>& DAG,
    std::vector<std::vector<Vertex>>& DAG_invert) {
    std::queue<Vertex> Q;
    bool* visited;
    size_t size = query.GetNumVertices();

    // 메모리 동적 할당
    visited = (bool*)calloc(size, sizeof(bool));

    Q.push(root);

    while (!Q.empty()) {
        Vertex u = Q.front();
        Q.pop();

        Vertex start = query.GetNeighborStartOffset(u);
        Vertex end = query.GetNeighborEndOffset(u);

        for (Vertex j = start; j < end; j++) {
            Vertex curNeigh = query.GetNeighbor(j);

            if (!visited[curNeigh]) {
                DAG[u].push_back(curNeigh);
                DAG_invert[curNeigh].push_back(u);
                Q.push(curNeigh);
            }
        }

        visited[u] = true;

    }

    // 메모리 동적 할당 해제
    free(visited);

}

void Backtrack::InsertionSort(std::pair<Vertex, int> query[], int p, int r) {
    for (int last = p + 1; last <= r; last++) {
        int index = last;
        int data = query[last].second;
        std::pair<Vertex, int> element = query[last];

        for (int i = last - 1; (i >= 0) && (query[i].second > data); i--) {
            query[i + 1] = query[i];
            index = i;
        }

        query[index] = element;
    }
}

void Backtrack::Swap(std::pair<Vertex, int> query[], int p, int r) {
    if (p == r)
        return;

    std::pair<Vertex, int> temp = query[p];
    query[p] = query[r];
    query[r] = temp;
}

int Backtrack::Median(std::pair<Vertex, int> query[], int p, int q, int r) {
    int m = (query[p].second + query[q].second + query[r].second) -
        std::max(query[p].second, std::max(query[q].second, query[r].second)) -
        std::min(query[p].second, std::min(query[q].second, query[r].second));

    return (m == query[p].second) ? p : (m == query[q].second) ? q : r;
}

int Backtrack::Partition(std::pair<Vertex, int> query[], int p, int r) {
    int q = p + (r - p) / 2;
    int pivot_index = Median(query, p, q, r);
    std::pair<Vertex, int> pivot = query[pivot_index];

    Swap(query, p, pivot_index);

    int index = p;

    for (int i = p + 1; i <= r; i++) {
        if (query[i].second < pivot.second) {
            Swap(query, i, ++index);
        }
    }

    Swap(query, p, index);

    return index;
}

void Backtrack::QuickSort(std::pair<Vertex, int> query[], int p, int r) {
    if (p < r) {
        if (r - p + 1 <= 10)
            InsertionSort(query, p, r);
        else {
            int q = Partition(query, p, r);
            QuickSort(query, p, q - 1);
            QuickSort(query, q + 1, r);
        }
    }
}

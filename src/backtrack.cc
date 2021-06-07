/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"


Backtrack::Backtrack() {}

Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
    // 1. Build DAG
    Set_total_embedding(0); // 10만개를 출력하면 종료할 수 있게 total_embedding 멤버변수 0으로 설정

    // 1-1) initial Candidate Set에서의 canididate set의 크기 / query에서의 degree가 가장 작은 vertex를 root로 설정
    size_t numQueryVertice = query.GetNumVertices();

    Vertex root = SelectRoot(query, cs);

    std::vector<std::vector<Vertex>> DAG; // DAG
    std::vector<std::vector<Vertex>> DAG_invert; // DAG Invert (DAG에서 반대로 edge를 설정)

    DAG.resize(numQueryVertice);
    DAG_invert.resize(numQueryVertice);

    // 1-2) DAG와 DAG_invert 생성
    BuildDAG(root, query, DAG, DAG_invert);


    // 2. Embedding 찾기
    std::cout << "t " << numQueryVertice << "\n";

    // embedding을 저장할 배열
    auto *embedding = new Vertex[numQueryVertice];
    std::fill_n(embedding, numQueryVertice, -1);

    // 다음에 방문 가능한지 여부를 저장할 배열
    int *can_visit;
    can_visit = (int *) calloc(numQueryVertice, sizeof(int));

    // rooted-DAG의 root에 대하여
    for (size_t i = 0; i < cs.GetCandidateSize(root); i++) {
        Vertex first_candidate = cs.GetCandidate(root, i); // root의 cs의 원소들을 하나씩 find_embedding 함수에 넣어줍니다

        // true를 return하면 embedding 10만개를 다 찾았음을 의미
        if (find_Embedding(first_candidate, root, embedding, DAG, DAG_invert, can_visit, data, query,
                           cs, 0)) {
            delete[] embedding;
            free(can_visit);
            return;
        }
    }

    // 10만개 이하의 embedding이 나오는 경우
    delete[] embedding;
    free(can_visit);

}


bool
Backtrack::find_Embedding(const Vertex data_v, const Vertex query_v, Vertex embedding[],
                          std::vector<std::vector<Vertex>> dag, std::vector<std::vector<Vertex>> dag_invert,
                          int can_visit[],
                          const Graph &data, const Graph &query, const CandidateSet &cs, int embedding_size) {
    /* ---------------------------------------------------------------------------------------------
        1. DAG_invert의 query vertex에 대해 parent이면서 data vertex가 이웃한 쌍이 embedding에 없으면 종료
       ---------------------------------------------------------------------------------------------*/

    bool is_neighbor = true;

    for (size_t i = 0; i < dag_invert[query_v].size(); i++) {
        Vertex curQuery = dag_invert[query_v][i];

        if (!data.IsNeighbor(embedding[curQuery], data_v)) {
            is_neighbor = false;
            break;
        }
    }

    if (!is_neighbor) // 해당 data_vertex의 query_vertex의 부모 query_vertex에 mapping된 data_vertex들과 연결되어 있지 않으므로 종료
        return false;

    /* ---------------------------------------------------------------------------------------------
        2. embedding에 query vertex, data vertex pair 넣기
       ---------------------------------------------------------------------------------------------*/

    Vertex *embedding_copy;
    embedding_copy = (Vertex *) calloc(query.GetNumVertices(), sizeof(Vertex));

    // embedding 복사 + injective 조건 확인
    for (Vertex i = 0; i < (int)query.GetNumVertices(); i++){
        if (embedding[i] == data_v){ // 이미 해당 data vertex가 embedding에 있으므로 injective 조건을 만족 x
            free(embedding_copy);
            return false;
        }

        embedding_copy[i] = embedding[i];
    }

    embedding_copy[query_v] = data_v; // 해당 data vertex embedding에 넣기


    int *can_visit_copy;
    can_visit_copy = (int *) calloc(query.GetNumVertices(), sizeof(int));
    std::copy(can_visit, can_visit + query.GetNumVertices(), can_visit_copy);

    can_visit_copy[query_v] = 1; // 해당 query vertex는 방문 했음을 뜻함

    /* ---------------------------------------------------------------------------------------------
        3. embedding size = query_size면 출력, return 하여 재귀 호출 이전으로 귀환
       ---------------------------------------------------------------------------------------------*/

    if (embedding_size + 1 == (int) query.GetNumVertices()) {

        std::cout << "a ";
        for (Vertex i = 0; i < (int) query.GetNumVertices(); i++) {
            std::cout << embedding_copy[i] << " ";
        }
        std::cout << std::endl;

        free(embedding_copy);
        free(can_visit_copy);

        Set_total_embedding(Get_total_embedding() + 1);

        if (Get_total_embedding() == 100000) // 10만개를 다 찾은 경우 true를 return
            return true;
        else
            return false;
    }

    /* ---------------------------------------------------------------------------------------------
        4. 다음 방문할 vertex 정하기
       ---------------------------------------------------------------------------------------------*/

    // 4-1) 먼저 해당 query_vertex에 연결된 vertex들이 다음에 방문 가능한지 check
    for (size_t i = 0; i < dag[query_v].size(); i++) {
        Vertex curQuery = dag[query_v][i]; // 해당 query_vertex에 연결된 vertex

        if (can_visit_copy[curQuery] == 0) { // 아직 방문 안된 경우
            bool next_visitable = true;

            // 그 vertex의 부모 vertex를 봅니다
            for (size_t j = 0; j < dag_invert[curQuery].size(); j++) {
                if (can_visit_copy[dag_invert[curQuery][j]] !=
                    1) { // 해당 vertex의 모든 부모들을 이미 방문한 경우에만 진짜로 다음에 방문 가능한 vertex가 됩니다
                    next_visitable = false;
                    break;
                }
            }

            if (next_visitable)
                can_visit_copy[curQuery] = 2; // 방문할 수 있는 query vertex임을 표시
        }
    }

    // 4-2) can_visit에서 cs값이 가장 작은 값에 해당하는 query vertex 방문

    Vertex next_vertex = -1;
    int next_vertex_cs_count = -1;

    for (Vertex i = 0; i < (int) query.GetNumVertices(); i++) {
        if (can_visit_copy[i] == 2) { // 다음 방문 가능한 query vertex에 대하여
            if (next_vertex == -1) { // 처음 for loop에 들어와서 값이 변경되는 경우
                next_vertex = i;
                next_vertex_cs_count = (int) cs.GetCandidateSize(i);
            } else {
                if (next_vertex_cs_count >
                    (int) cs.GetCandidateSize(i)) { // cs size가 가장 작은 vertex가 다음 방문할 query vertex가 됩니다
                    next_vertex = i;
                    next_vertex_cs_count = (int) cs.GetCandidateSize(i);
                }
            }
        }
    }

    if (next_vertex == -1) { // 다음 방문할 vertex가 없는 경우 종료
        free(embedding_copy);
        free(can_visit_copy);
        return false;
    }

    for (int i = 0; i < next_vertex_cs_count; i++) {
        Vertex v = cs.GetCandidate(next_vertex, i);

        // 다음 query vertex의 data vertex들 하나 하나 find_embedding 함수를 call 해줍니다.
        if (find_Embedding(v, next_vertex, embedding_copy, dag, dag_invert, can_visit_copy, data, query,
                           cs, embedding_size + 1)) { // 10만개를 다 찾았을 때 true 받아, 해당 함수를 벗어남
            free(embedding_copy);
            free(can_visit_copy);
            return true;
        }

    }

    free(embedding_copy);
    free(can_visit_copy);
    return false; // 아직 10만개를 다 찾지 못했으므로 false

}

Vertex Backtrack::SelectRoot(const Graph &query, const CandidateSet &cs) {
    // root는 일단 Vertex 0으로 가정
    Vertex root = 0;
    double root_data = (double) cs.GetCandidateSize(0) / (double) query.GetDegree(0);

    // query의 vertex를 한번씩 훑으면서 계산
    for (Vertex v = 1; v < (int) query.GetNumVertices(); v++) {
        double v_data = (double) cs.GetCandidateSize(v) / (double) query.GetDegree(v);

        if (root_data > v_data) { // (initial Candidate Set에서의 canididate set의 크기 / query에서의 degree)가 가장 작은 vertex를 root로 설정
            root = v;
            root_data = v_data;
        }
    }

    return root;

}

void Backtrack::BuildDAG(Vertex root, const Graph &query, std::vector<std::vector<Vertex>> &DAG,
                         std::vector<std::vector<Vertex>> &DAG_invert) {
    std::queue<Vertex> Q; // BFS를 위한 queue
    bool *visited;

    // 메모리 동적 할당
    visited = (bool *) calloc(query.GetNumVertices(), sizeof(bool));

    Q.push(root); // root를 가장 먼저 방문

    // BFS order로 vertex들을 훑으면서 DAG, DAG_invert에 vertex들을 넣습니다
    while (!Q.empty()) {
        Vertex u = Q.front();
        Q.pop();

        Vertex start = query.GetNeighborStartOffset(u);
        Vertex end = query.GetNeighborEndOffset(u);

        for (Vertex v = start; v < end; v++) {
            Vertex curNeigh = query.GetNeighbor(v);

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

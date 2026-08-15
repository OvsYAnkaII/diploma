#include "graph.h"

#include <charconv>
#include <fstream>
#include <vector>
#include <algorithm>
#include <optional>

using namespace std::literals;

Graph::Graph(const std::string_view str_file_path) {
    std::ifstream col_file(str_file_path);
    std::string info_line;

    auto [marker,c_lines] = RemoveFileHeader(col_file);
    std::tie(vertices_amount_, edges_amount_) = GetVerticesAndEdges(marker, col_file);

    adjacency_lists_.resize(vertices_amount_); // метод GetVerticesAndEdges возвращает истинное количество вершин реализации (т е +1)
    //list_of_edges_.reserve(edges_amount_);
    //degrees_.resize(vertices_amount_);

    //constexpr int p_lines = 1;
    /*FormDegrees(col_file, c_lines);
    for (int vertex = 1; vertex < vertices_amount_; ++vertex) {
        adjacency_lists_[vertex].reserve(degrees_[vertex]);
    }*/
    col_file.close();
    InitializeLists(str_file_path); // Adjacency and edges lists
    InitializeMatrix();
}

void Graph::Represent(std::ostream& output) const {
    output << "Vertices_amount: " << V() << std::endl;
    output << "Edges_amount: " << E() << std::endl;
    output << "Density: " << Density() << std::endl;
    output << "Max degree: " << MaxDegree() << std::endl;
    output << "Min degree: " << MinDegree() << std::endl;
    output << "Avg degree: " << AvgDegree() << std::endl << std::endl;

    output << "Sum of degrees: " << std::boolalpha << (SumDegree() == 2*E())
    << " " << SumDegree() << " " << AvgDegree()*V() << " vs " << 2*E() << std::endl;

    std::cout << "Check Degrees for each vertex..." << std::endl;
    for (int i = 1; i <= V(); i++) {
        if (Degree(i) <= 0) {
            std::cerr << i << "Vertex degree is <= 0" << std::endl;
            // assert(false && "Parsing Error");
            // return;
        }
        assert(Degree(i) == adjacency_lists_[i].size() && "invalid degree");
    }

    for (int i = 1; i <= V(); i++) {
        std::cout << i << ":";
        for (const int j: AdjacentVertices(i)) {
            std::cout << " " << j;
        }
        std::cout << std::endl;
    }

}


void Graph::CheckImplementationRestrictions(const int v, const int e) {
    if (v > MAX_VERTICES_ ) {
        throw std::logic_error(R"(too many vertices for this graph implementation (need <= 5000))");
    }

    if (e > MAX_EDGES_) {
        throw std::logic_error(R"(too many edges for this graph implementation (need <= 50kk))");
    }

    //if (1.0*e > 0.5*v*(v-1)) {
        //throw std::logic_error(R"(too many edges for set vertices, should be |E| <= 1/2 * |V||V-1|)");
    //}
    if (e > v*(v-1)) {
        throw std::logic_error(R"(too many edges for set vertices, should be |E| <= 1/2 * |V||V-1|)");
    }
}

std::tuple<char, int> Graph::RemoveFileHeader(std::ifstream& input_file) {
    char marker = GetMarker(input_file);
    std::string info_line;
    int c_lines = 0;
    while (marker == 'c') {
        getline(input_file, info_line);
        c_lines++;
        marker = GetMarker(input_file);
    }
    return {marker, c_lines};
}

std::tuple<int, int> Graph::GetVerticesAndEdges(const char marker, std::ifstream& input_file) {
    if (marker == 'p') {
        int v_amount = 0;
        int e_amount = 0;
        if (std::string info_line; !(input_file >> info_line >> v_amount >> e_amount)) {
            throw std::logic_error(R"(Wrong line with 'p' marker: amounts of vertices and edges are expected in this line)");
        }
        CheckImplementationRestrictions(v_amount, e_amount);
        return {v_amount+1, e_amount};
    }
    throw std::logic_error(R"(Wrong .col file format: 'p' marker is expected after 'c' markers)");
}

void Graph::InitializeLists(const std::string_view path_to_col_file) { // Adjacency and edges lists
    std::ifstream input_file(path_to_col_file);
    std::string info_line;
    RemoveFileHeader(input_file);
    getline(input_file, info_line);
    char marker = ' ';
    int v1 = 0;
    int v2 = 0;
    for (int edge_idx = 0; edge_idx < E(); ++edge_idx) {
        input_file >> std::ws >> marker >> std::ws >> v1 >> std::ws >> v2;
        if (std::max(v1, v2) > adjacency_lists_.capacity()) {
            //std::cout << "v1: " << v1 << " v2: " << v2 << " " << adjacency_lists_.size() << std::endl;
            throw std::logic_error("the indices of the vertices in the graph must not decrease monotonically and must not exceed the index of the maximum vertex == |V|");
        }

        if (v1 == v2) {
            throw std::logic_error("a vertex of a graph cannot be self connected");
        }
        adjacency_lists_[v1].push_back(v2);
        adjacency_lists_[v2].push_back(v1);
    }

    /*for (int vertex = 1; vertex <= V(); ++vertex) {
        std::sort(adjacency_lists_[vertex].begin(), adjacency_lists_[vertex].end(),
            [this](const int lhs, const int rhs) {
                return Degree(lhs) >= Degree(rhs);
            });
    }

    std::sort(adjacency_lists_.begin()+1, adjacency_lists_.end(),
        [](std::vector<int>& lhs, std::vector<int>& rhs) {
            return lhs.size() >= rhs.size();
        });*/
}

void Graph::InitializeMatrix() {
    // Используем std::vector<char>, так как vector<bool> не всегда эффективен по кэшу
    adjacency_matrix_.assign((V()+1) * (V()+1), 0);

    for (int u = 1; u <= V(); ++u) {
        for (const int v : adjacency_lists_[u]) {
            adjacency_matrix_[u * vertices_amount_ + v] = 1;
        }
    }
}


char Graph::GetMarker(std::ifstream& input_file) {
    int ch;
    do {
        ch = input_file.get();
        if (ch == EOF) throw std::logic_error("Unexpected EOF");
    } while (std::isspace(ch));
    const char marker = static_cast<char>(ch);
    if (marker != 'c' && marker != 'p' && marker != 'e') {
        throw std::logic_error("Wrong file format: Unexpected marker: "s + std::string{marker} + " but one of {c, p, e} was expected"s);
    }
    return marker;
}

// граф не считается валидным если в нем есть кратные ребра или если он не связный, программа не проверяет эти критерии из-за производительности
/*[[nodiscard]] bool Graph::IsConnected(const int root_vertex_index = 0) const{
    auto visited = std::vector<bool>(degrees_.size(), false);
    std::queue<int> to_visit{};
    to_visit.push(root_vertex_index);
    while (!to_visit.empty()) {
        int another_vertex_index = to_visit.front();
        to_visit.pop();
        visited[another_vertex_index] = true;
        for (auto adjacent_vertex_index : AdjacentVertices(another_vertex_index)) {
            if (!visited[adjacent_vertex_index]) {
                to_visit.push(adjacent_vertex_index);
            }
        }
    }

    for (const bool is_visited: visited) {
        if (!is_visited) {
            return false;
        }
    }
    return true;
}

void Graph::IsValidGraph() const {
    if (!IsConnected()) {
        throw std::logic_error("Wrong Graph implementation: graph is not connected");
    }

    for (int v = 1; v < adjacency_lists_.size(); v++) {
        bool has_duplicates = std::adjacent_find(adjacency_lists_[v].begin(), adjacency_lists_[v].end()) != adjacency_lists_[v].end();
        if (has_duplicates) {
            throw std::logic_error("Wrong Graph implementation: duplicated edges are detected");
        }
        bool is_sorted = std::is_sorted(adjacency_lists_[v].begin(), adjacency_lists_[v].end());
        if (!is_sorted) {
            throw std::logic_error("Inner problem: adjacency_lists_ are not sorted");
        }
    }
}*/
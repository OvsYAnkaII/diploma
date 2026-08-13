# pragma once
#include <charconv>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <optional>

using namespace std::literals;

class Graph {
public:
    explicit Graph(std::string_view str_file_path);
    [[nodiscard]] int V() const {
        return vertices_amount_-1;
    }

    [[nodiscard]] int E() const {
        return edges_amount_;
    }

    [[nodiscard]] const std::vector<int>& AdjacentVertices(const int vertex) const {
        return adjacency_lists_[vertex];
    }

    [[nodiscard]] bool HasEdge(const int vertex1, const int vertex2) const {
        assert(adjacency_matrix_[vertex1*(V()+1)+vertex2] == adjacency_matrix_[vertex2*(V()+1)+vertex1]);
        return  adjacency_matrix_[vertex1*(V()+1)+vertex2] == 1;
    }

    [[nodiscard]] double Density() const {
        const int max_edges = 0.5*V()*(V()-1);
        if (max_edges == 0) {
            return 0;
        }
        return static_cast<double>(E()) / max_edges;
    }

    [[nodiscard]] int Degree(const int v) const {
        return static_cast<int>(adjacency_lists_[v].size());
    }

    [[nodiscard]] int MaxDegree() const {
        int max_degree = Degree(1);
        for (int v_idx = 1; v_idx <= V(); v_idx++) {
            max_degree = std::max(max_degree, Degree(v_idx));
        }
        return max_degree;
    }

    [[nodiscard]] int MinDegree() const {
        int min_degree = Degree(1);
        for (int degree_idx = 1; degree_idx <= V(); ++degree_idx) {
            min_degree = std::min(min_degree, Degree(degree_idx));
        }
        return min_degree;
    }

    [[nodiscard]] int SumDegree() const {
        int sum_degree = 0;
        for (int vertex_idx = 1; vertex_idx <= V(); ++vertex_idx) {
            sum_degree += Degree(vertex_idx);
        }
        return sum_degree;
    }

    [[nodiscard]] double AvgDegree() const {
        return 2.0*E()/V();
    }

    void Represent(std::ostream& output = std::cout) const;

    size_t size() const {
        size_t total = 0;

        // 1. Память под сами объекты векторов (управляющие блоки)
        total += sizeof(adjacency_lists_);      // размер самого вектора (24 байта в 64-bit)
        total += sizeof(adjacency_matrix_);
        total += sizeof(is_vertex_active_);

        // 2. Память под данные векторов
        // Для списков смежности: сумма capacity() * sizeof(int) для каждого вектора
        for (const auto& list : adjacency_lists_) {
            total += list.capacity() * sizeof(int);
            // + размер самого vector-объекта (уже учтён в sizeof(adjacency_lists_))
        }

        // Для матрицы смежности
        total += adjacency_matrix_.capacity() * sizeof(char);

        // Для is_vertex_active_
        total += is_vertex_active_.capacity() * sizeof(char);

        // 3. Примитивные поля
        total += sizeof(vertices_amount_);
        total += sizeof(edges_amount_);

        return total;
    }

private:
    int vertices_amount_{};
    int edges_amount_{};
    std::vector<std::vector<int>> adjacency_lists_;
    std::vector<char> adjacency_matrix_;
    std::vector<char> is_vertex_active_;
    static constexpr int MAX_VERTICES_ = 5001;
    static constexpr int MAX_EDGES_ = 50'000'000;

    static void CheckImplementationRestrictions(int v, int e);

    static std::tuple<char, int> RemoveFileHeader(std::ifstream& input_file);

    static std::tuple<int, int> GetVerticesAndEdges(char marker, std::ifstream& input_file);

    /*void FormDegrees(std::ifstream& input_file, const int c_lines) {
        char marker = ' ';
        int v1 = 0;
        int v2 = 0;
        // const int p_lines = 1
        int current_line = c_lines+1;
        for (int edge_idx = 0; edge_idx < edges_amount_; ++edge_idx) {
            current_line++;
            if (!(input_file >> std::ws >> marker >> std::ws >> v1 >> std::ws >> v2)) {
                throw std::logic_error("Wrong .col file format: wrong format at the line: " + std::to_string(current_line));
            }
            if (v1 <= 0 || v2 <= 0) {
                throw std::logic_error("Wrong .col file format: vertices indexes should be positive (>0), problem at line: " + std::to_string(current_line));
            }
            degrees_[v1]++;
            degrees_[v2]++;
        }
    }*/

    void InitializeLists(std::string_view path_to_col_file); // Adjacency lists

    void InitializeMatrix();



    static char GetMarker(std::ifstream& input_file);

    // граф не считается валидным если в нем есть кратные ребра или если он не связный, программа не проверяет эти критерии из-за производительности
    // [[nodiscard]] bool IsConnected(const int root_vertex_index = 0) const;
    // void IsValidGraph() const;
};
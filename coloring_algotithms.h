#pragma once

#include "graph.h"
#include <queue>
#include <chrono>
#include <random>
#include <unordered_set>

struct Coloring {
    explicit Coloring(const int vertices):color(vertices, -1){}
    std::vector<int> color; // color[v] = номер цвета (1..k)
    int numColors = -1; // сколько цветов использовано
    int conflicts = -1;
    std::chrono::duration<double> time{}; // время раскраски
    void Represent(std::ostream& out = std::cout) const {
        out << "colors: " << numColors << std::endl;
        out << "time: " << time.count() << std::endl;
    }
};

int GetRandom(int from, int to);

// O(m)
int CountConflicts(const Graph& graph, const std::vector<int>& color);

// O(m)
inline bool HasConflicts(const Graph& graph, const std::vector<int>& color) {
    return CountConflicts(graph, color) != 0;
}

static bool Colored(const int v) {
    return v > -1;
}

// возвращает минимальный цвет для вершины получая цвета и раскраску ее соседей O(deg(v) + k)
int GetColour(const std::vector<int>& colours, const std::vector<int>& adjacent_vertices);

// O(kn+m)
Coloring ColorWithBFS(const Graph& graph, int root);

// O(k*n^2)
Coloring ColorStraightForward(const std::vector<int>& ordered_vertexes, const Graph& graph);

//
std::vector<int> GetMaxIndependentSetWithRandom(const Graph& graph, std::vector<int>& candidate_vertices);

//
std::vector<int> GetMaxIndependentSetRLF(const Graph& graph, std::vector<int>& candidate_vertices);

namespace algorithms {
    // O(k*n^3)
    Coloring GreedyBFS(const Graph& graph, int root = 1);

    // O(k*n^2)
    Coloring GreedyForward(const Graph& graph);

    // O(k*n^2)
    Coloring GreedySortedASC(const Graph& graph);

    // O(k*n^2)
    Coloring GreedySortedDESC(const Graph& graph); // Powell-Welsh

    namespace detail {
        struct VertexInfo {
            int vertex;
            std::unordered_set<int> color_neighbourhood; // cn(v)
            int degree; // d(v)
            VertexInfo(int v, int d): vertex(v), degree(0) {};
        };

        // Компаратор для set: сортировка по убыванию saturation, затем по возрастанию degree
        struct Compare {
            bool operator()(const VertexInfo& lhs, const VertexInfo& rhs) const {
                if (lhs.color_neighbourhood.size() != rhs.color_neighbourhood.size())
                    return lhs.color_neighbourhood.size() > rhs.color_neighbourhood.size(); // первичная сортировка по saturation
                if (lhs.degree != rhs.degree)
                    return lhs.degree < rhs.degree; // вторичная сортировка по degree
                return lhs.vertex < rhs.vertex;
            }
        };
    }
    //
    Coloring DSatur(const Graph& graph);

    //
    Coloring RandomIndependentSets(const Graph& graph);

    //
    Coloring RLF(const Graph& graph);

}

namespace chrom_num_bounds {
    inline int lower_bound_fraction(const Graph& graph) {
        const double n = graph.V();
        const double m = graph.E();
        return  std::floor(n*n/(n*n-2*m));
    }

    inline int upper_bound_sqrt(const Graph& graph) {
        const double m = graph.E();
        return  std::ceil(1.0/2 + std::sqrt(2*m+1.0/4));
    }

    inline int upper_bound_degree(const Graph& graph) {
        int max_degree = std::numeric_limits<int>::min();
        for (int v = 1; v <= graph.V(); v++) {
            max_degree = std::max(max_degree, graph.Degree(v));
        }
        return  max_degree+1;
    }

    inline int upper_bound_max_min(const Graph& graph) {
        std::vector<int> degrees;
        degrees.reserve(graph.V());
        for (int v = 1; v <= graph.V(); v++) {
            degrees.push_back(graph.Degree(v));
        }
        std::sort(degrees.begin(), degrees.end(), [](const int lhs, const int rhs){return lhs > rhs;});
        int iterable_value = std::numeric_limits<int>::max();
        int max_value = std::numeric_limits<int>::min();
        for (int v_idx = 1; v_idx <= degrees.size(); v_idx++) {
            const int vertex = v_idx-1;
            iterable_value = std::min(degrees[vertex]+1, v_idx);
            max_value = std::max(max_value, iterable_value);
        }
        return  max_value;
    }
}
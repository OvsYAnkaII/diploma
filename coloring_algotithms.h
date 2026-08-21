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
        // out << "time: " << time.count() << std::endl;
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
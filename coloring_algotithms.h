#pragma once

#include "graph.h"
#include <queue>
#include <chrono>

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

static bool Colored(const int v) {
    return v != -1;
}

// возвращает минимальный цвет для вершины получая цвета и раскраску ее соседей O(deg(v) + k)
int GetColour(const std::vector<int>& colours, const std::vector<int>& adjacent_vertices);

// O(m)
int CountConflicts(const Graph& graph, const std::vector<int>& color);

// O(m)
inline bool HasConflicts(const Graph& graph, const std::vector<int>& color) {
    return CountConflicts(graph, color) != 0;
}

// O(kn+m)
Coloring ColorWithBFS(const Graph& graph, int root);

// O(k*n^2)
Coloring ColorStraightForward(const std::vector<int>& ordered_vertexes, const Graph& graph);

namespace algorithms {
    // O(k*n^3)
    Coloring GreedyBFS(const Graph& graph, int root = 1);

    // O(k*n^2)
    Coloring GreedyForward(const Graph& graph);

    // O(k*n^2)
    Coloring GreedySortedDESC(const Graph& graph);

    // O(k*n^2)
    Coloring GreedySortedASC(const Graph& graph);
}
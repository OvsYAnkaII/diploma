#include "coloring_algotithms.h"

#include <unordered_set>

// инвариант: |adj_vertices| < |colours| == n
int GetColour(const std::vector<int>& colours, const std::vector<int>& adjacent_vertices) {
    // маска вектора цветов (1..k) (присутствует ли цвет в множестве смежных вершин)
    // имеет максимальный размер в n (для худшего случая)
    std::vector<char> used(colours.size(), false);
    for (const int v : adjacent_vertices) {
        if (colours[v] != -1) { // если текущая вершина окрашена (имеет цвет)
            used[colours[v]] = true;
        }
    }
    // ищем с 1 тк цвета 1..k
    for (int c = 1; c < static_cast<int>(colours.size()); ++c) {
        if (!used[c]) { // если цвет не присутствует в множестве смежных вершин - покрасим в этот цвет
            return c;
        }
    }
    return static_cast<int>(colours.size()); // если цвет не присутствует в множестве смежных вершин - покрасим в новый цвет
}

int CountConflicts(const Graph& graph, const std::vector<int>& color) {
    int conflicts = 0;
    for (int v1 = 1; v1 <= graph.V(); v1++) {
        for (const auto v2: graph.AdjacentVertices(v1)) {
            if (color[v1] == color[v2]) {
                conflicts++;
            }
        }
    }
    assert(color[0] == -1 && "покрашена фиктивная вершина");
    assert(std::count(color.begin(), color.end(), -1) == 1 && "не покрашена какая то вершина графа");
    return conflicts;
}

inline Coloring ColorWithBFS(const Graph& graph, const int root) {
    Coloring colors(graph.V()+1);
    std::vector<bool> visited(graph.V(), false);
    int total_colours = -1;
    std::queue<int> vertices_to_color;
    vertices_to_color.push(root);
    while (!vertices_to_color.empty()) {
        const int current_vertex = vertices_to_color.front();
        vertices_to_color.pop();
        visited[current_vertex] = true;
        if (!Colored(colors.color[current_vertex])) {
            colors.color[current_vertex] = GetColour(colors.color, graph.AdjacentVertices(current_vertex));
        }
        total_colours = std::max(total_colours, colors.color[current_vertex]);
        for (int adj_vertex: graph.AdjacentVertices(current_vertex)) {
            if (!visited[adj_vertex]) {
                vertices_to_color.push(adj_vertex);
            }
        }
    }
    colors.numColors = total_colours;

    return colors;
}

Coloring ColorStraightForward(const std::vector<int>& ordered_vertexes, const Graph& graph){
    Coloring colors(ordered_vertexes.size());
    int max_colours = -1;
    for (const auto vertex: ordered_vertexes) {
        if (vertex != 0) {
            auto c = GetColour(colors.color, graph.AdjacentVertices(vertex));
            max_colours = std::max(max_colours, c);
            colors.color[vertex] = c;
        }
    }
    colors.numColors = max_colours;
    return colors;
}

Coloring algorithms::GreedyBFS(const Graph& graph, const int root) {
    const auto start{std::chrono::steady_clock::now()};
    auto colors = ColorWithBFS(graph, root);
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}

Coloring algorithms::GreedyForward(const Graph& graph) {
    auto vertices = std::vector<int>();
    vertices.reserve(graph.V()+1);
    for (auto v = 0; v <= graph.V(); v++) {
        vertices.emplace_back(v);
    }
    const auto start{std::chrono::steady_clock::now()};
    auto colors = ColorStraightForward(vertices, graph);
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}

Coloring algorithms::GreedySortedDESC(const Graph& graph) {
    auto vertices = std::vector<int>();
    vertices.reserve(graph.V());
    for (auto v = 0; v <= graph.V(); v++) {
        vertices.emplace_back(v);
    }
    std::sort(vertices.begin(), vertices.end(),
        [&graph](const int lhs, const int rhs) {
        return graph.Degree(lhs) < graph.Degree(rhs);
    });
    const auto start{std::chrono::steady_clock::now()};
    auto colors = ColorStraightForward(vertices, graph);
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}

Coloring GreedySortedASC(const Graph& graph) {
    auto vertices = std::vector<int>();
    vertices.reserve(graph.V());
    for (auto v = 0; v <= graph.V(); v++) {
        vertices.emplace_back(v);
    }
    std::sort(vertices.begin(), vertices.end(),
        [&graph](const int lhs, const int rhs) {
        return graph.Degree(lhs) < graph.Degree(rhs);
    });
    const auto start{std::chrono::steady_clock::now()};
    auto colors = ColorStraightForward(vertices, graph);
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}


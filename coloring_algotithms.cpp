#include "coloring_algotithms.h"
#include <unordered_set>
#include <unordered_map>
#include <set>

int GetRandom(const int from, const int to) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(from, to);
    return dist(gen);
}

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
    int uncolored = 0; // реальные вершины должны быть покрашены, иначе - конфликт
    for (int v1 = 0; v1 <= graph.V(); v1++) {
        for (const auto v2: graph.AdjacentVertices(v1)) {
            if (color[v1] == color[v2]) {
                conflicts++;
            }
            if (!Colored(color[v1])) {
                uncolored++;
            }
            if (!Colored(color[v2])) {
                uncolored++;
            }
        }
    }
    assert(uncolored == 0 && "не покрашена какая то вершина графа");
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

std::vector<int> GetMaxIndependentSetWithRandom(const Graph& graph, std::vector<int>& candidate_vertices) {
    auto X = std::unordered_set(candidate_vertices.begin(), candidate_vertices.end());
    std::vector<int> Y;
    Y.reserve(candidate_vertices.size());
    std::vector<int> S;
    while (!X.empty()) {
        auto it = X.begin();
        std::advance(it, GetRandom(0, X.size()-1)); // O(1)
        const auto u = *it;
        S.push_back(*it);
        X.erase(it); // O(1)
        for (auto v = X.begin(); v != X.end();) { // O(|X|)
            if (graph.HasEdge(u, *v)) {
                Y.push_back(*v);
                v = X.erase(v);
            } else {
                ++v;
            }
        }
    }
    candidate_vertices = Y;
    return S;
}

template <class Container>
int Degree(int v, const Container& candidates, const Graph& graph) {
    int degree = 0;
    for (const auto other_vertex: candidates) {
        if (graph.HasEdge(v, other_vertex)) {
            ++degree;
        }
    }
    return degree;
}

struct VertexData {
    int degX;
    int degY;
};

std::vector<int> GetMaxIndependentSetRLF(const Graph& graph, std::vector<int>& candidate_vertices) {
    std::unordered_set X(candidate_vertices.begin(), candidate_vertices.end());
    std::unordered_set<int> Y;
    std::unordered_map<int, VertexData> cache;

    if (X.size() == graph.V()+1) {
        for (int v : candidate_vertices) {
            cache[v] = {graph.Degree(v), 0};
        }
    } else { // O(n^2)
        for (int v : candidate_vertices) {
            cache[v] = {Degree(v, X, graph), 0};
        }
    }


    std::vector<int> S;
    bool first_pick = true;

    while (!X.empty()) {
        std::unordered_set<int>::iterator it;
        if (first_pick) { // O(n)
            first_pick = false;
            // Выбираем вершину с максимальной степенью в X
            it = std::max_element(X.begin(), X.end(),
                [&cache](const int lhs, const int rhs) {
                    return cache[lhs].degX < cache[rhs].degX;
                });
        } else {
            // Выбираем вершину с максимальной степенью в Y, при равенстве — минимальную степень в X
            it = std::max_element(X.begin(), X.end(),
                [&cache](const int lhs, const int rhs) {
                    if (cache[lhs].degY != cache[rhs].degY)
                        return cache[lhs].degY < cache[rhs].degY;
                    return cache[lhs].degX > cache[rhs].degX;
                });
        }

        int u = *it;
        S.push_back(u);
        X.erase(it);

        for (int w : graph.AdjacentVertices(u)) {
            if (X.contains(w)) {
                X.erase(w);
                Y.insert(w);
                cache.erase(w);
            }
        }

        // Обновляем степени для оставшихся вершин в X
        for (int w : graph.AdjacentVertices(u)) {
            if (Y.contains(w)) {
                for (int z : graph.AdjacentVertices(w)) {
                    if (X.contains(z)) {
                        cache[z].degY++;
                    }
                }
            }
        }

        // Уменьшаем degX для всех вершин, которые были соседями удалённых
        for (int w : graph.AdjacentVertices(u)) {
            if (Y.contains(w)) {
                for (int z : graph.AdjacentVertices(w)) {
                    if (X.contains(z)) {
                        cache[z].degX--;
                    }
                }
            }
        }
    }

    candidate_vertices = std::vector<int>(Y.begin(), Y.end());
    return S;
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
    vertices.reserve(graph.V()+1);
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

Coloring algorithms::GreedySortedASC(const Graph& graph) {
    auto vertices = std::vector<int>();
    vertices.reserve(graph.V()+1);
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

Coloring algorithms::DSatur(const Graph& graph) {
    using namespace detail;
    Coloring colors(graph.V()+1);
    const auto start{std::chrono::steady_clock::now()};
    std::set<VertexInfo, detail::Compare> Q;
    std::vector<std::set<VertexInfo, Compare>::iterator> Q_iterators;
    Q_iterators.resize(graph.V()+1);
    for (int v = 0; v <= graph.V(); ++v) {
        auto [it, res] = Q.emplace(v, graph.Degree(v)); // color_neighbourhood = {}
        if (!res) {
            assert(false && "Element construction in Q failed");
        }
        Q_iterators[v] = it;
    }

    while (!Q.empty()) {
        VertexInfo v_info = Q.extract(Q.begin()).value();
        int u = v_info.vertex;
        int c = GetColour(colors.color, graph.AdjacentVertices(u)); // O(deg(v)+k)
        colors.color[u] = c;
        colors.numColors = std::max(colors.numColors, c);
        for (int adj_v: graph.AdjacentVertices(u)) { // O(deg(v))
            if (!Colored(adj_v)) {
                VertexInfo adj_v_info = Q.extract(Q_iterators[adj_v-1]).value(); // O(avg 1)
                adj_v_info.color_neighbourhood.insert(c);
                adj_v_info.degree -= 1;
                auto [it, res] = Q.insert(adj_v_info); // O(avg 1)
                if (!res) {
                    assert(false && "Element construction in Q failed");
                }
                Q_iterators[adj_v] = it;
            }
        }
    }
    const auto finish{std::chrono::steady_clock::now()};
    colors.conflicts = CountConflicts(graph, colors.color);
    colors.time = std::chrono::duration<double>(finish - start);
    return colors;
}

Coloring algorithms::RandomIndependentSets(const Graph& graph) {
    Coloring colors(graph.V()+1);
    auto vertices_to_color = std::vector<int>();
    vertices_to_color.reserve(graph.V());
    for (auto v = 0; v <= graph.V(); v++) {
        vertices_to_color.emplace_back(v);
    }

    int c = 0;
    const auto start{std::chrono::steady_clock::now()};
    while (!vertices_to_color.empty()) {
        c++;
        auto color_class = GetMaxIndependentSetWithRandom(graph, vertices_to_color);
        for (const auto v: color_class) {
            colors.color[v] = c;
        }
    }
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.numColors = c;
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}

Coloring algorithms::RLF(const Graph &graph) {
    Coloring colors(graph.V()+1);
    auto vertices_to_color = std::vector<int>();
    vertices_to_color.reserve(graph.V());
    for (auto v = 0; v <= graph.V(); v++) {
        vertices_to_color.emplace_back(v);
    }

    int c = 0;
    const auto start{std::chrono::steady_clock::now()};
    while (!vertices_to_color.empty()) {
        c++;
        auto color_class = GetMaxIndependentSetRLF(graph, vertices_to_color);
        for (const auto v: color_class) {
            colors.color[v] = c;
        }
    }
    const auto finish{std::chrono::steady_clock::now()};
    colors.time = std::chrono::duration<double>(finish - start);
    colors.numColors = c;
    colors.conflicts = CountConflicts(graph, colors.color);
    return colors;
}



#include <movable/Bot2.hpp>
#include <limits.h>

#define BOT 1
#define OP 2
#define EQUI 0

struct Node {
    Location l;
    int c;
    int dist;
};

class AugmentedHeap {
    private:
        std::vector<Node> heap;
        std::vector<std::vector<int>> index;
        
    public:
        AugmentedHeap(int w, int h) {
            index.resize(w, std::vector<int>(h));
            for (int i = 0; i < w; ++i) {
                for (int j = 0; j < h; ++j) {
                    add({{i, j}, EQUI, INT_MAX});
                }
            }
        }

        void swap(int i, int j) {
            Node tmp = heap[i];
            Location li = heap[i].l;
            Location lj = heap[j].l;
            heap[i] = heap[j];
            heap[j] = tmp;

            index[li.w][li.h] = j;
            index[lj.w][lj.h] = i;
        }

        void fixUp(int i) {
            while (i > 0 && heap[i].dist < heap[(i-1)/2].dist) {
                swap(i, (i - 1)/2);

                i = (i - 1) / 2;
            }
        }

        void fixDown(int i) {
            int size = heap.size();
            while (2 * i + 1 < size) {
                int j = 2 * i + 1;
                if (heap[j].dist > heap[j + 1].dist) ++j;
                if (heap[i].dist <= heap[j].dist) break;

                swap(i, j);
                i = j;
            }
        }

        void add(Node n) {
            heap.push_back(n);
            index[n.l.w][n.l.h] = heap.size() - 1;
            fixUp(heap.size() - 1);
        }

        void pop() {
            swap(0, heap.size() - 1);
            heap.pop_back();
            fixDown(0);
        }

        void update(Location l, int dist, int c) {
            int i = index[l.w][l.h];
            if (i < 0 || i > heap.size() - 1) {
                return;
            }
            heap[i].dist = dist;
            heap[i].c = c;
            fixUp(i);
            fixDown(i);
        }

        bool isEmpty() {
            return heap.size() == 0; 
        }

        Node top() {
            return heap[0];
        }

        Node getNode(Location l) {
            return heap[index[l.w][l.h]];
        }
};

std::vector<std::vector<char>> Bot2::VoronoiDiagram(GameState game) {
    int W = game.w;
    int H = game.h;

    std::vector<std::vector<char>> visited(W, std::vector<char>(H, -1));
    std::vector<std::vector<int>> dist(W, std::vector<int>(H, -1));
    AugmentedHeap set(W, H);

    Location bot = game.bot.location;
    Location op = game.oponent.location;

    visited[bot.w][bot.h] = BOT;
    visited[op.w][op.h] = OP;
    
    set.update(bot, 0, BOT);
    set.update(op, 0, OP);

    while (!set.isEmpty()) {
        Node minNode = set.top();
        set.pop();

        Location l = minNode.l;
        if (game.getCrashed(l)) {
            continue;
        }
        visited[l.w][l.h] = minNode.c;
        dist[l.w][l.h] = minNode.dist;
        Location move[] = {
            {l.w - 1, l.h},
            {l.w + 1, l.h},
            {l.w, l.h - 1},
            {l.w, l.h + 1},
        };

        for (Location n: move) {
            if (game.getCrashed(n) || visited[n.w][n.h] != -1 || n.equal(game.bot.location) || n.equal(game.oponent.location)) {
                continue;
            }
            if (minNode.dist + 1 < set.getNode(n).dist) {
                set.update(n, minNode.dist + 1, minNode.c);
            }
            if (minNode.dist + 1 == set.getNode(n).dist && minNode.c != set.getNode(n).c) {
                set.update(n, minNode.dist + 1, EQUI);
            }
        }
    }
    
    // for (int i = 0; i < W; ++i) {
    //     for (int j = 0; j < H; ++j) {
    //         std::cout << static_cast<int> (visited[i][j]);
    //     }
    //     std::cout << std::endl;
    // }
    visited[bot.w][bot.h] = -1;
    visited[op.w][op.h] = -1;
    return visited;
}

double Bot2::evaluate() {
    return 50.0;
};

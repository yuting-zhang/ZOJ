#include <cstdio>
#include <iostream>
#include <cstdio>
#include <memory>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cassert>
#include <algorithm>

#define MAXN 1000000050


using namespace std;

struct node_t{
    int elem;
    node_t *left, *right;
    int size;
    unsigned short cnt;
    unsigned short priority;
};

node_t pool[60000 * 16 + 1];
int pool_index = 0;

class treap {
public:
    treap(){
        srand(time(0));
        root = nullptr;
    }

    void insert(int elem){
        insert(root, elem);    
    }

    void remove(int elem){
        remove(root, elem);
    }

    int rank(int elem) {
        return rank(root, elem);
    }
    
    vector<int> to_vector() {
        vector<int> ret;
        ret.reserve(root ? root->size : 1);
        to_vector(root, ret);
        return ret;
    }

private:
    node_t *root;

    node_t* rotateLeft(node_t*& node){
        node_t *right = node->right, *rightLeft = right->left;
        right->left = node;
        node->right = rightLeft;

        node->size = get_size(node->left, node->right) + node->cnt;
        right->size = get_size(right->left, right->right) + right->cnt;
        return right;
    }

    node_t* rotateRight(node_t*& node){
        node_t *left = node->left, *leftRight = left->right;
        left->right = node;
        node->left = leftRight;

        node->size = get_size(node->left, node->right) + node->cnt;
        left->size = get_size(left->left, left->right) + left->cnt;
        return left;
    }

    void insert(node_t*& node, int elem){
        if (node == nullptr){
            node = &pool[pool_index++];
            assert(pool_index <= 60000 * 16);
            node->elem = elem;
            node->left = node->right = nullptr;
            node->priority = rand() % USHRT_MAX;
            node->size = 1;
            node->cnt = 1;
            return;
        }

        if (node->elem == elem) {
            node->cnt++;
            node->size = get_size(node->left, node->right) + node->cnt;
        }
        else if (node->elem > elem){
            insert(node->left, elem);
            node->size = get_size(node->left, node->right) + node->cnt;
            if (node->priority < node->left->priority)
                node = rotateRight(node);
        }else{
            insert(node->right, elem);
            node->size = get_size(node->left, node->right) + node->cnt;
            if (node->priority < node->right->priority)
                node = rotateLeft(node);
        }
    }

    void remove(node_t*& node, int elem){
        if (node == nullptr)
            return;
        
        if (node->elem == elem){
            if (node->cnt > 1) {
                node->cnt--;
                node->size = get_size(node->left, node->right) + node->cnt;
            }
            else if (!node->left && !node->right) {
                node = nullptr;
            }
            else if (!node->left || (node->left && node->right && 
                node->left->priority < node->right->priority)){
                node = rotateLeft(node);
                remove(node->left, elem);
                node->size = get_size(node->left, node->right) + node->cnt;
            }
            else{
                node = rotateRight(node);
                remove(node->right, elem);
                node->size = get_size(node->left, node->right) + node->cnt;
            }
        }
        else if (node->elem > elem) {
            remove(node->left, elem);
            node->size = get_size(node->left, node->right) + node->cnt;
        }
        else {
            remove(node->right, elem);
            node->size = get_size(node->left, node->right) + node->cnt;
        }
    }

    int get_size(node_t*& left, node_t*& right) {
        int size = 0;
        if (left)
            size += left->size;
        if (right)
            size += right->size;
        return size;
    }

    int rank(node_t*& node, int elem) {
        if (!node)
            return 0;

        if (node->elem > elem) 
            return rank(node->left, elem);
        else {
            int size = node->cnt;
            if (node->left)
                size += node->left->size;

            if (node->elem == elem)
                return size;

            size += rank(node->right, elem);
            return size;
        }
    }

    void to_vector(node_t*& node, vector<int> &ret) {
        if (node) {
            to_vector(node->left, ret);
            for (int i = 0; i < node->cnt; i++)
                ret.push_back(node->elem);
            to_vector(node->right, ret);
        }
    }
};


vector<int> A;

struct segment_tree {
    size_t n;
    vector<treap> st;

    int left(int p) {
        return p << 1;
    }

    int right(int p) {
        return (p << 1) + 1;
    }

    void build(int p, int L, int R) {
        if (L == R) 
            st[p].insert(A[L]);
        else {
            for (int i = L; i <= R; i++)
                st[p].insert(A[i]);
            int lchild = left(p), rchild = right(p);
            build(lchild, L, (L + R) / 2);
            build(rchild, (L + R) / 2 + 1, R);
        }
    }

    int query(int p, int L, int R, int i, int j, int val) {
        if (i > R || j < L)
            return 0;
        if (L >= i && R <= j) 
            return st[p].rank(val);

        int p1 = query(left(p), L, (L + R) / 2, i, j, val);
        int p2 = query(right(p), (L + R) / 2 + 1, R, i, j, val);
        return p1 + p2;
    }

    void update(int p, int L, int R, int pos, int old, int val) {
        st[p].remove(old);
        st[p].insert(val);

        if (L == R)
            return;

        if (pos <= (L + R) / 2)
            update(left(p), L, (L + R) / 2, pos, old, val);
        else
            update(right(p), (L + R) / 2 + 1, R, pos, old, val);

    }

    segment_tree() {
        n = A.size();
        st.assign(n * 4, treap());
        build(1, 0, n - 1);
    }

    int query(int i, int j, int val) {
        return query(1, 0, n - 1, i, j, val);
    }

    void update(int pos, int val) {
        update(1, 0, n - 1, pos, A[pos], val);
    }
};

int main() {
    int X;
    scanf("%d", &X);
    while (X--) {
        pool_index = 0;
        int N, M;
        scanf("%d %d", &N, &M);
        A.assign(N, 0);
        for (int i = 0; i < N; i++)
            scanf("%d", &A[i]);

        segment_tree st;

        while (M--) {
            char op;
            scanf(" %c ", &op);
            if (op == 'Q') {
                int i, j, k;
                scanf("%d %d %d", &i, &j, &k);
                i--, j--;
                int lower = 0, upper = MAXN, result = MAXN;
                while (lower <= upper) {
                    int mid = (lower + upper) >> 1;
                    int p = st.query(i, j, mid);

                    if (p >= k) {
                        result = mid;
                    }

                    if (p < k)
                        lower = mid + 1;
                    else
                        upper = mid - 1;
                }
                printf("%d\n", result);
            }
            else if (op == 'C') {
                int pos, val;
                scanf("%d %d", &pos, &val);
                st.update(pos - 1, val);
                A[pos - 1] = val;
            }
        }
    }
    return 0;
}

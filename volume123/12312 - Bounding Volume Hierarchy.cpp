#include <bits/stdc++.h>
using namespace std;
 
const double eps = 1e-12;
struct Pt3D {
	double x, y, z;
	int read() {
		return scanf("%lf%lf%lf", &x, &y, &z) == 3;
	}
	Pt3D(double x=0, double y=0, double z=0):
		x(x), y(y), z(z) {}
	double& operator[](int i) {
		if (i == 0)	return x;
		if (i == 1)	return y;
		if (i == 2)	return z;
		assert(false);
	}
	static Pt3D cross(Pt3D a, Pt3D b, Pt3D c) {
		b.x -= a.x, b.y -= a.y, b.z -= a.z;
		c.x -= a.x, c.y -= a.y, c.z -= a.z;
		return Pt3D(b.y*c.z-b.z*c.y, b.z*c.x-b.x*c.z, b.x*c.y-b.y*c.x);
	}
	static double dot(Pt3D a, Pt3D b) {
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}
	static double area(Pt3D a, Pt3D b, Pt3D c) {
		return cross(a, b, c).length()/2;
	}
	static double line2pt(Pt3D st, Pt3D ed, Pt3D p) {
		double a = area(st, ed, p);
		return a*2 / st.distance(ed);
	}
	void normal() {
		double v = sqrt(x*x+y*y+z*z);
		x /= v, y /= v, z /= v;
	}
	Pt3D operator+(const Pt3D &a) const {
        return Pt3D(x + a.x, y + a.y, z + a.z);
    }
    Pt3D operator-(const Pt3D &a) const {
        return Pt3D(x - a.x, y - a.y, z - a.z);
    }
    Pt3D operator/(const double val) const {
        return Pt3D(x / val, y / val, z / val);
    }
    Pt3D operator*(const double val) const {
        return Pt3D(x * val, y * val, z * val);
    }
	void print() {
		printf("%.6lf %.6lf %.6lf\n", (double) x, (double) y, (double) z);
	}
	double distance(Pt3D p) {
		return sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z));
	}
	double length() {
		return sqrt(x*x+y*y+z*z);
	}
};

int inTriangle(Pt3D a, Pt3D b, Pt3D c, Pt3D p) {
    Pt3D v0 = c - a;
    Pt3D v1 = b - a;
    Pt3D v2 = p - a;

    double dot00 = Pt3D::dot(v0, v0);
    double dot01 = Pt3D::dot(v0, v1);
    double dot02 = Pt3D::dot(v0, v2);
    double dot11 = Pt3D::dot(v1, v1);
    double dot12 = Pt3D::dot(v1, v2);

    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    if (u < 0 || u > 1)
        return 0;
    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
    if (v < 0 || v > 1)
        return 0;
    return u + v <= 1;
}

struct Plane {
	// ax + by + cz + d = 0
	double a, b, c, d;
	void read() {
		scanf("%lf%lf%lf%lf", &a, &b, &c, &d);
	}
	Plane() {
		
	}
	Plane(Pt3D pta, Pt3D ptb, Pt3D ptc) {
		Pt3D v = Pt3D::cross(pta, ptb, ptc);
		a = v.x, b = v.y, c = v.z, d = -(v.x*pta.x+v.y*pta.y+v.z*pta.z);
	}
	int raycast(Pt3D st, Pt3D ed, Pt3D &pa, Pt3D &pb, Pt3D &pc, Pt3D &o, double &t_limit) {
		Pt3D v = ed-st;
		double t = -(st.x*a+st.y*b+st.z*c+d)/(v.x*a+v.y*b+v.z*c);
		if (t < 0 || t > t_limit)
			return 0;
		o = st + v*t;
		if (inTriangle(pa, pb, pc, o)) {
			t_limit = t;
			return 1;
		}
		return 0;
	}
	Pt3D project(Pt3D p) {
		Pt3D v(a, b, c);
		double t = -(p.x*a+p.y*b+p.z*c+d)/(v.x*a+v.y*b+v.z*c);
		Pt3D o = p + v*t;
		return o;
	}
	double distance(Pt3D p) {
		Pt3D v(a, b, c);
		double t = -(p.x*a+p.y*b+p.z*c+d)/(v.x*a+v.y*b+v.z*c);
		v = v*t;
		return v.length();
	}
	void print() {
		printf("%.2lf %.2lf %.2lf %.2lf\n", a, b, c, d);
	}
};

const int MAXV = 50005;
const int MAXT = 100005;
Pt3D p[MAXV];
Plane plane[MAXT];
int tri[MAXT][3];

struct M_BVH {
	struct Node {
		vector<int> list;
		Pt3D l, r;	// bounding
		Node *lson, *rson;
		int axis;
	} nodes[MAXT*2];
	int nodesize;
	Node *root;
	vector<int> adj[MAXV];
	void init() {
		nodesize = 0;
		root = NULL;
	}
	Node* newNode() {
		assert(nodesize < MAXT*2);
		Node *p = &nodes[nodesize++];
		p->lson = p->rson = NULL;
		p->list.clear();
		return p;
	}
	void build(int n, int m) {
		init();
		Pt3D l = p[0];
		Pt3D r = p[0];
		// full box
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 3; j++) {
				l[j] = min(l[j], p[i][j]);
				r[j] = max(r[j], p[i][j]);
			}
			adj[i].clear();
		}
		
		// triangle adj
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < 3; j++)
				adj[tri[i][j]].push_back(i);
		}
		vector<int> idx;
		for (int i = 0; i < n; i++)
			idx.push_back(i);
		root = _build(idx, l, r);
	}
	int axis_child(int i, double axis_val, int axis) {
		if (p[i][axis] <= axis_val)
			return 0;
		return 1;
	}
	void resize_box(Node *u) {
		if (u == NULL)
			return;

		for (auto vi : u->list) {
			for (auto i : adj[vi]) {
				for (int k = 0; k < 3; k++) {
					for (int t = 0; t < 3; t++) {
						u->l[t] = min(u->l[t], p[tri[i][k]][t]);
						u->r[t] = max(u->r[t], p[tri[i][k]][t]);
					}
				}
			}
		}
		if (u->lson != NULL) {
			for (int i = 0; i < 3; i++)	{
				u->l[i] = min(u->l[i], u->lson->l[i]);
				u->r[i] = max(u->r[i], u->lson->r[i]);
			}
		}
		if (u->rson != NULL) {
			for (int i = 0; i < 3; i++)	{
				u->l[i] = min(u->l[i], u->rson->l[i]);
				u->r[i] = max(u->r[i], u->rson->r[i]);
			}
		}
	}
	Node* _build(vector<int> idx, Pt3D l, Pt3D r) {
		if (idx.size() == 0)
			return NULL;
		Node *ret = newNode();
		ret->l = l, ret->r = r;

		if (idx.size() == 1) {
			for (auto i : idx)
				ret->list.push_back(i);
			resize_box(ret);
			return ret;
		}
		int axis = 0;
		for (int i = 1; i < 3; i++) {
			if (r[i]-l[i] > r[axis]-l[axis])
				axis = i;
		}

		vector<double> axis_val;
		for (auto i : idx)
			axis_val.push_back(p[i][axis]);
		ret->axis = axis;
		sort(axis_val.begin(), axis_val.end());
		axis_val.erase(unique(axis_val.begin(), axis_val.end()), axis_val.end());
		int m = axis_val.size();
		double mid = axis_val[m/2];
		vector<int> lidx, ridx;
		for (auto i : idx) {
			int type = axis_child(i, mid, axis);
			if (type == 0)
				lidx.push_back(i);
			else
				ridx.push_back(i);
		}
		
		if (lidx.size() == idx.size() ||
			ridx.size() == idx.size()) {
			for (auto i : idx)
				ret->list.push_back(i);
			resize_box(ret);
			return ret;
		}
		idx.clear();
		Pt3D tl, tr;
		tl = l, tr = r;
		tr[axis] = mid;
		ret->lson = _build(lidx, tl, tr);
		tl = l, tr = r;
		tl[axis] = mid;
		ret->rson = _build(ridx, tl, tr);
		resize_box(ret);
		return ret;
	}

	double t_limit;
	Pt3D t_st, t_ed, t_v, t_hit;
	int t_id;

	static int clip(Pt3D p, Pt3D q, Pt3D l, Pt3D r, int x, int y) {
		double a, b, s, t;
		a = q[y] - p[y];
		b = q[x] - p[x];
	
		t = a * (l[x] - p[x]) - b * (l[y] - p[y]);
		if (fabs(t) < eps)
			return true;
		t = t < 0;
		s = (a * (l[x] - p[x]) - b * (r[y] - p[y]));
		if ((s < 0) != t || fabs(s) < eps)
			return true;
	
		s = (a * (r[x] - p[x]) - b * (l[y] - p[y]));
		if ((s < 0) != t || fabs(s) < eps)
			return true;
	
		s = (a * (r[x] - p[x]) - b * (r[y] - p[y]));
		if ((s < 0) != t || fabs(s) < eps)
			return true;
		return false;
	}
	int avaiable(Node *u) {
		if (u == NULL)
			return 0;
		Pt3D h = t_st + t_v*t_limit;
		for (int i = 0; i < 3; i++) {
			if (!clip(t_st, h, u->l, u->r, i, (i+1)%3))
				return 0;
		}
		return 1;
	}
	int _raycast(Node *u) {
		if (u == NULL)
			return 0;
		int f = 0;
		for (auto vi : u->list) {
			for (auto i : adj[vi]) {
				Pt3D hit;
				if (plane[i].raycast(t_st, t_ed, p[tri[i][0]], p[tri[i][1]], p[tri[i][2]], hit, t_limit))
					t_hit = hit, t_id = i, f = 1;
			}
		}

		if (t_v[u->axis] > 0) {
			if (avaiable(u->lson))
				f |= _raycast(u->lson);
			if (avaiable(u->rson))
				f |= _raycast(u->rson);
		} else {
			if (avaiable(u->rson))
				f |= _raycast(u->rson);
			if (avaiable(u->lson))
				f |= _raycast(u->lson);
		}
		return f;
	}
	void raycast(Pt3D st, Pt3D ed) {
		t_limit = 1e36;
		t_st = st, t_ed = ed;
		t_v = t_ed - t_st;
		t_id = -1;

		_raycast(root);

		if (t_id == -1)
			puts("-1");
		else
			printf("%d %.6lf %.6lf %.6lf\n", t_id, t_hit[0], t_hit[1], t_hit[2]);
	}
} tree;
int main() {
	int n, m, q;
	while (scanf("%d", &n) == 1 && n) {
		for (int i = 0; i < n; i++)
			p[i].read();
		scanf("%d", &m);
		for (int i = 0; i < m; i++) {
			scanf("%d %d %d", &tri[i][0], &tri[i][1], &tri[i][2]);
			plane[i] = Plane(p[tri[i][0]], p[tri[i][1]], p[tri[i][2]]);
		}
		
		tree.build(n, m);
		
		scanf("%d", &q);
		for (int i = 0; i < q; i++) {
			Pt3D st, ed;
			st.read(), ed.read();
			tree.raycast(st, ed);
		}
	}
	return 0;
}
/*
4
0 0 0
9 0 0
3 4 0
4 2 7
4
0 1 2
0 1 3
0 2 3
1 2 3
5
-3 0 0 4 2 3
3 -1 -2 4 2 3
6 -3 5 4 2 3
12 7 5 4 2 3
4 2 8 5 3 9
0



1
14 59 60 40 2 7

*/

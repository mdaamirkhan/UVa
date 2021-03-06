#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
using namespace std;

struct DICE {
    int dice[6]; // front, right, up, back, left, down
	void rightTurn() {
    	static int t;
    	t = dice[0], dice[0] = dice[4];
    	dice[4] = dice[3], dice[3] = dice[1], dice[1] = t;
	}
	void upTurn() {
    	static int t;
    	t = dice[0], dice[0] = dice[5];
    	dice[5] = dice[3], dice[3] = dice[2], dice[2] = t;
	}
	void clockwise() {
	    static int t;
	    t = dice[2], dice[2] = dice[4];
	    dice[4] = dice[5], dice[5] = dice[1], dice[1] = t;
	}
	void print() {
		for (int i = 0; i < 6; i++)
			printf("%d ", dice[i]);
		puts("");
	}
	vector<DICE> generate() {
		vector<DICE> ret;
		DICE tmp = *this;
		
	    for (int i = 0; i < 4; i++) {
	        for(int j = 0; j < 4; j++) {
	        	ret.push_back(tmp);
	            tmp.clockwise();
	        }        
	        tmp.rightTurn();
	    }
	    tmp.upTurn();
	    for (int i = 0; i < 2; i++) {
	        for (int j = 0; j < 4; j++) {
	            ret.push_back(tmp);
	            tmp.clockwise();
	        }        
	        tmp.upTurn(), tmp.upTurn();
	    }
	    return ret;
	}
	bool operator<(const DICE &x) const {
		for (int i = 0; i < 6; i++)
			if (dice[i] != x.dice[i])
				return dice[i] < x.dice[i];
		return false;
	}
	bool operator==(const DICE &x) const {
		for (int i = 0; i < 6; i++)
			if (dice[i] != x.dice[i])
				return false;
		return true;
	}
};

int path[8];
vector<DICE> kind[8];
vector<DICE> permutation;
int ret = 0x3f3f3f3f, N;
int counter[32];
void dfs(int idx) {
	if (idx == N) {
		int ch = 0;
		for (int i = 0; i < 6; i++) {
			int mx = 0;
			memset(counter, 0, sizeof(counter));
			for (int j = 0; j < N; j++)
				mx = max(mx, ++counter[kind[j][path[j]].dice[i]]);
			ch += N - mx;
			if (ch >= ret)	return ;
		}
		ret = min(ret, ch); 
		return ;
	}
	for (int i = 0; i < kind[idx].size(); i++) {
		path[idx] = i;
		dfs(idx+1);
	}
}
int main() {
    DICE p;
    for (int i = 0; i < 6; i++)
    	p.dice[i] = i;
    permutation = p.generate();
    
    char s[128];
    while (scanf("%d", &N) == 1 && N) {
    	map<string, int> colors;
    	for (int i = 0; i < N; i++) {
    		int v[6];
    		for (int j = 0; j < 6; j++) {
    			scanf("%s", s);
    			int &x = colors[s];
    			if (x == 0)	x = colors.size();
    			v[j] = x;
    		}
    		DICE D;
    		D.dice[0] = v[0], D.dice[1] = v[1], D.dice[2] = v[2];
    		D.dice[5] = v[3], D.dice[4] = v[4], D.dice[3] = v[5];
    		
    		kind[i] = D.generate();
    		sort(kind[i].begin(), kind[i].end());
    		kind[i].resize(unique(kind[i].begin(), kind[i].end()) - kind[i].begin());
//    		printf("%d\n", kind[i].size());
    	}
    	
    	ret = 0x3f3f3f3f;
    	dfs(0);
    	printf("%d\n", ret);
    }
    return 0;
} 

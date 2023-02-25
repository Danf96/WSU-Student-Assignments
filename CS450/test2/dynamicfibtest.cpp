    #include <iostream>
    #include <vector>
    #include <limits>
    #include <algorithm>
    using namespace std;
    int custFibo(int n, vector<int>& solutions){ 
        if(solutions[n] != -1) return solutions[n];
        auto a = custFibo(n-4, solutions);
        auto b = custFibo(n-3, solutions);
        auto c = custFibo(n-2, solutions);
        auto d = custFibo(n-1, solutions);
        solutions[n] = (a + b + c + d) * 2;
        return solutions[n];
    }

    int main() {
        vector<int> solutions(101, -1);
        solutions[0] = 0;
        solutions[1] = 5;
        solutions[2] = 10;
        solutions[3] = 30;

        cout << custFibo(100, solutions) << endl;
   
    	return 0;
    }

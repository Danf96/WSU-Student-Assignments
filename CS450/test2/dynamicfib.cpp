    #include <iostream>
    using namespace std;
    int custFibo(int n){ //does fibonacci for 3 numbers instead of 2
        if (n == 0) return 0;
        if (n == 1) return 1;
        if (n == 2) return 2;
        int i = 3;
        int a = 0;
        int b = 1;
        int c = 2;
        int d;
        while (i <= n){
            d = a + b + c;
            a = b;
            b = c;
            c = d;
            i++;
        }
        return c;
    }

    int main() {
        
    	for (int i = 0; i < 10; i++){
    	    cout << custFibo(i) << endl;
        }
    	return 0;
    }

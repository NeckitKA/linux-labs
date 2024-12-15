#include <iostream>
using namespace std;

int main(int argc, char* argv[]) { 
        string line;
        int max = 0;
        while (true) {
                getline(cin, line);
                if (line == "")  break;
                int a = atoi(line.c_str()); // преобразуем строку в число
                if (max<a) {
		     max = a;
                }
        }
        cout << max << endl;
        return 0;
}


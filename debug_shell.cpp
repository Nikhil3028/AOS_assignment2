#include "Header.h"

int main() {
    cout << "Testing basic shell functionality:" << endl;
    
    // Test 1: Basic echo
    cout << "1. Testing echo:" << endl;
    echo("echo hello world");
    
    // Test 2: Basic ls
    cout << "2. Testing ls:" << endl;
    ls("");
    
    // Test 3: Test pipe splitting
    cout << "3. Testing pipe splitting:" << endl;
    string pipeInput = "ls | grep .cpp";
    if (hasPipe(pipeInput)) {
        vector<string> commands = splitPipe(pipeInput);
        cout << "Found " << commands.size() << " commands:" << endl;
        for (size_t i = 0; i < commands.size(); i++) {
            cout << "  Command " << i << ": '" << commands[i] << "'" << endl;
        }
        
        // Test 4: Execute the pipe
        cout << "4. Testing pipe execution:" << endl;
        executePipe(commands);
    }
    
    return 0;
}

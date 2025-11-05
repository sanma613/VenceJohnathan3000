#include <vector>
#include <utility>

using namespace std;


// Forward declaration para evitar dependencia circular
class Piece;

vector<pair<int, int>> IsCenterAvailible(vector<vector<Piece*>> Grid){
    int center_positions[4][2]  = {{2, 2}, {2, 3}, {3, 2}, {3, 3}};

    vector<pair<int, int>> availableCenters;

    for (int i = 0; i < 4; i++) {
        int row = center_positions[i][0];
        int col = center_positions[i][1];
        
        if (Grid[row][col] == nullptr) {  
            availableCenters.push_back({row, col});
        }
    }
    
    return availableCenters;
}
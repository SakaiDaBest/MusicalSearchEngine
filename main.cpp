#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

struct SearchDB {
    vector<vector<string>> data;
    unordered_map<string, vector<int>> artistIndex;
    unordered_map<string, vector<int>> songIndex;
    unordered_map<string, vector<int>> albumIndex;
};

SearchDB readCSV(const string& filename) {
    SearchDB db;
    vector<vector<string>> data;
    unordered_map<string, vector<int>> artistIndex;
    unordered_map<string, vector<int>> songIndex;
    unordered_map<string, vector<int>> albumIndex;

    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return db;
    }
    
    string line;
    int currentLine=0;
    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        db.data.push_back(row);
        
        string artist = row[2];
        string song = row[4];
        string album = row[3];
        db.artistIndex[artist].push_back(currentLine);
        db.songIndex[song].push_back(currentLine);
        db.albumIndex[album].push_back(currentLine);

        currentLine++;
    }
    
    file.close();
    return db;
}

int main() {
    SearchDB db = readCSV("dataset.csv");
    //row[0]  row[1]    row[2]    row[3]      row[4]      row[5]      row[6]        row[7]
    //index   track_id  artists   album_name  track_name  popularity  duration_ms   explicit
    //row[8]        row[9]  row[10]   row[11]    row[12]  row[13]       row[14]       row[15]
    //danceability  energy  key       loudness   mode     speechiness   acousticness  instrumentalness
    //row[16]     row[17]   row[18]   row[19]          row[20]
    //liveness    valence   tempo     time_signature   track_genre

    for (const auto& row : db.data) {
        if (row[2]=="Gen Hoshino") {
          for (const auto& cell : row) {
            cout << cell << "\t";
          }
          cout << endl;
        }

    }

    return 0;
}

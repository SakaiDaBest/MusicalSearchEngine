#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

vector<vector<string>> readCSV(const string& filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

int main() {
    auto data = readCSV("dataset.csv");
    //row[0]  row[1]    row[2]    row[3]      row[4]      row[5]      row[6]        row[7]
    //index   track_id  artists   album_name  track_name  popularity  duration_ms   explicit
    //row[8]        row[9]  row[10]   row[11]    row[12]  row[13]       row[14]       row[15]
    //danceability  energy  key       loudness   mode     speechiness   acousticness  instrumentalness
    //row[16]     row[17]   row[18]   row[19]          row[20]
    //liveness    valence   tempo     time_signature   track_genre
    for (const auto& row : data) {
        // for (const auto& cell : row) {
        //     cout << cell << "\t";
        // }
        if (row[2]=="Gen Hoshino") {
          for (const auto& cell : row) {
            cout << cell << "\t";
          }
          cout << endl;
        }

    }

    return 0;
}

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// #include <unordered_map>
using namespace std;
template <class S, class T>

class unordered_map {
public:
  struct ll {
    S key;
    T value;
    ll *next;
  };
  ll *arr[100000];
  unordered_map() {
    for (int i = 0; i < 100000; i++)
      arr[i] = NULL;
  }
  void insert(S key, T value) {
    int h = hash(key);
    if (arr[h] == NULL) {
      ll *p = new ll{key, value, NULL};
      arr[h] = p;
    } else {
      ll *q = arr[h];
      while (true) {
        if (q->key == key) {
          q->value = value;
          return;
        }
        if (q->next == NULL)
          break;
        q = q->next;
      }
      q->next = new ll{key, value, NULL};
    }
  }

  void erase(S key) {
    int h = hash(key);
    ll *p = arr[h];
    ll *prev = NULL;

    while (p != NULL) {
      if (p->key == key) {
        if (prev == NULL)
          arr[h] = p->next;
        else
          prev->next = p->next;

        delete p;
        return;
      }
      prev = p;
      p = p->next;
    }
  }

  int hash(S key) {
    long long h = 0;
    ostringstream os;
    os << key;
    string key1 = os.str();
    long long m = 1;
    for (int i = 0; i < int(key1.size()); i++) {
      h += int(key1[i]) * m;
      h = h % 100000;
      m = (m * 17) % 100000;
    }
    return (int)h;
  }
  bool find(S key) {
    int h = hash(key);
    if (arr[h] == NULL)
      return false;
    ll *p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return true;
      p = p->next;
    }
    return false;
  }
  T &operator[](S key) {
    int h = hash(key);
    ll *p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value;
      p = p->next;
    }
    insert(key, T{});
    p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value;
      p = p->next;
    }
    static T default_value;
    return default_value;
  }
};
// LEVENSHTEIN DISTANCE (DID YOU MEAN?)
int levenshteinDist(string word1, string word2) {
  int size1 = word1.size();
  int size2 = word2.size();

  vector<vector<int>> verif(size1 + 1, vector<int>(size2 + 1));

  if (size1 == 0)
    return size2;
  if (size2 == 0)
    return size1;

  for (int i = 0; i <= size1; i++)
    verif[i][0] = i;
  for (int j = 0; j <= size2; j++)
    verif[0][j] = j;

  for (int i = 1; i <= size1; i++) {
    for (int j = 1; j <= size2; j++) {
      int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

      verif[i][j] = min(min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                        verif[i - 1][j - 1] + cost);
    }
  }

  return verif[size1][size2];
}

struct TrieNode {
  TrieNode *children[26]; // 26 pointers
  bool isEnd;             // check if this node is the end of a word
  string word;

  TrieNode() {
    isEnd = false;
    word = "";
    for (int i = 0; i < 26; i++)
      children[i] = NULL;
  }
};
class Trie { // stores a word into the trie by building paths
public:
  TrieNode *root; // starting point of trie

  Trie() {
    root = new TrieNode(); // create root node when trie starts
  }
  void insert(string s) {  // to store words into trie
    TrieNode *node = root; // start from root

    for (char c : s) { // loop through each char
      if (!isalpha(c))
        continue;
      c = tolower(c);

      int index = c - 'a';

      if (node->children[index] == NULL)        // exists?
        node->children[index] = new TrieNode(); // letter does not exit,create
                                                // it

      node = node->children[index]; // move to next node
    }
    node->isEnd = true; // mark the last word down
    node->word = s;     // store full word for output(without this only know
                        // letters)
  }
  void dfs(TrieNode *node,
           vector<string> &result) { // collects all words from the trie
    if (node == NULL)
      return;

    if (node->isEnd)
      result.push_back(node->word); // complete? add to result

    for (int i = 0; i < 26; i++)
      dfs(node->children[i], result); // go through can explore every possible
                                      // word under this node?
  }
  vector<string> autocomplete(string prefix) { // return all word match prefix
    TrieNode *node = root; // every search starts from begining

    for (char c : prefix) {
      if (!isalpha(c))
        continue;
      c = tolower(c);

      int index = c - 'a';

      if (node->children[index] == NULL)
        return {}; // if path doesnt exist no suggestion

      node = node->children[index]; // move doen the tree
    }
    vector<string> result; // store suggestion
    dfs(node, result);     // ex:prefix ca then find all words starting from ca
    return result;
  }
};

struct SearchDB {
  vector<vector<string>> data;
  unordered_map<string, vector<int>> artistIndex;
  unordered_map<string, vector<int>> songIndex;
  unordered_map<string, vector<int>> albumIndex;

  Trie artistTrie;
  Trie songTrie;
  Trie albumTrie;
};

SearchDB readCSV(const string &filename) {
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
  int currentLine = 0;
  while (getline(file, line)) {
    vector<string> row;
    stringstream ss(line);
    string cell;

    while (getline(ss, cell, ',')) {
      row.push_back(cell);
    }

    db.data.push_back(row);

    string artist = row[1];
    string album = row[2];
    string song = row[3];
    db.artistIndex[artist].push_back(currentLine);
    db.songIndex[song].push_back(currentLine);
    db.albumIndex[album].push_back(currentLine);
    db.artistTrie.insert(artist);
    db.songTrie.insert(song);
    db.albumTrie.insert(album);

    currentLine++;
  }

  file.close();
  return db;
}

vector<int> multiKeywordSearch(SearchDB &db, string query) {
  vector<int> rsult;

  stringstream ss(query);
  string word;

  bool first = true;

  while (ss >> word) {
    vector<int> temp;

    if (!db.artistIndex[word].empty())
      for (int x : db.artistIndex[word])
        temp.push_back(x);
    else if (!db.songIndex[word].empty())
      for (int x : db.songIndex[word])
        temp.push_back(x);
    else if (!db.albumIndex[word].empty())
      for (int x : db.albumIndex[word])
        temp.push_back(x);
    if (first) {
      rsult = temp;
      first = false;
    } else {
      vector<int> newResult;
      for (int x : rsult) {
        for (int y : temp) {
          if (x == y) {
            newResult.push_back(x);
          }
        }
      }
      rsult = newResult;
    }
  }
  return rsult;
}

vector<int> smartSearch(SearchDB &db, string query) {
  if (!db.artistIndex[query].empty())
    return db.artistIndex[query];
  if (!db.songIndex[query].empty())
    return db.songIndex[query];
  if (!db.albumIndex[query].empty())
    return db.albumIndex[query];

  return multiKeywordSearch(db, query);
}
// Levenshetein Search
vector<int> levenshteinSearch(SearchDB &db, string query, int threshold = 2) {
  vector<int> result;

  for (int i = 0; i < db.data.size(); i++) {
    string artist = db.data[i][1];
    string album = db.data[i][2];
    string song = db.data[i][3];

    int d1 = levenshteinDist(query, artist);
    int d2 = levenshteinDist(query, album);
    int d3 = levenshteinDist(query, song);

    int d = min(d1, min(d2, d3));

    if (d <= threshold) {
      result.push_back(i);
    }
  }

  return result;
}

void merge(vector<int> &indices, int left, int mid, int right, SearchDB &db,
           int col, bool ascending) {
  vector<int> L(indices.begin() + left, indices.begin() + mid + 1);
  vector<int> R(indices.begin() + mid + 1, indices.begin() + right + 1);

  int i = 0, j = 0, k = left;
  while (i < L.size() && j < R.size()) {
    string a = db.data[L[i]][col];
    string b = db.data[R[j]][col];
    bool takeLeft = ascending ? (a <= b) : (a >= b);
    if (takeLeft)
      indices[k++] = L[i++];
    else
      indices[k++] = R[j++];
  }
  while (i < L.size())
    indices[k++] = L[i++];
  while (j < R.size())
    indices[k++] = R[j++];
}

void mergeSort(vector<int> &indices, int left, int right, SearchDB &db, int col,
               bool ascending) {
  if (left >= right)
    return;
  int mid = (left + right) / 2;
  mergeSort(indices, left, mid, db, col, ascending);
  mergeSort(indices, mid + 1, right, db, col, ascending);
  merge(indices, left, mid, right, db, col, ascending);
}

// LOOK AT THIS \/
// col: 1=artist, 2=album, 3=song
void sortResults(vector<int> &indices, SearchDB &db, int col = 2,
                 bool ascending = true) {
  if (indices.empty())
    return;
  mergeSort(indices, 0, indices.size() - 1, db, col, ascending);
}

// Example usage
// sortResults(results, db, 2, true);   // A-Z by artist
//  sortResults(results, db, 4, false);  // Z-A by song title
//  sortResults(results, db, 3, true);   // A-Z by album

void edit(SearchDB &db) {
  cout << "======= Edit Mode =======" << endl;
  cout << "Enter the song name that you want to edit: ";
  string songName;
  getline(cin, songName);

  // Bug 2 fix: use find() before operator[] to avoid phantom insertion
  if (!db.songIndex.find(songName)) {
    cout << "Song not found." << endl;
    return;
  }

  vector<int> matchingRows = db.songIndex[songName];
  int selectedRow = -1;

  if (matchingRows.size() > 1) {
    cout << "Multiple songs found with that name. Please select one:" << endl;
    for (int i = 0; i < (int)matchingRows.size(); i++) {
      int r = matchingRows[i];
      cout << i + 1 << ". Song: " << db.data[r][3] << "\n";
      cout << "   Artist: " << db.data[r][1] << "\n";
      cout << "   Album: " << db.data[r][2] << "\n";
    }
    cout << "Enter the number of the song you want to edit: ";
    int choiceE1;
    cin >> choiceE1;
    cin.ignore(); // Bug 1 fix: ignore leftover newline after cin >>

    if (choiceE1 < 1 || choiceE1 > (int)matchingRows.size()) {
      cout << "Invalid choice. Exiting edit mode." << endl;
      return;
    }
    selectedRow = matchingRows[choiceE1 - 1];
  } else if (matchingRows.size() == 1) {
    selectedRow = matchingRows[0];
  }

  // Bug 3 fix: guard against no match
  if (selectedRow == -1) {
    cout << "No valid song selected." << endl;
    return;
  }

  cout << "== What do you want to edit? ==" << endl;
  cout << "1. Song\n2. Artist\n3. Album\n";
  cout << "Enter your choice: ";
  int choiceE2;
  cin >> choiceE2;
  cin.ignore();

  if (choiceE2 < 1 || choiceE2 > 3) {
    cout << "Invalid choice. Exiting..." << endl;
    return;
  }

  if (choiceE2 == 1) {
    string oldSong = db.data[selectedRow][4];
    cout << "Enter the new song name: ";
    string newSong;
    getline(cin, newSong);

    db.data[selectedRow][4] = newSong;

    // Bug 4 fix: remove old index row reference, add new one
    vector<int> &oldVec = db.songIndex[oldSong];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.songIndex.erase(oldSong);

    db.songIndex[newSong].push_back(selectedRow);

  } else if (choiceE2 == 2) {
    string oldArtist = db.data[selectedRow][2];
    cout << "Enter the new artist name: ";
    string newArtist;
    getline(cin, newArtist);

    db.data[selectedRow][2] = newArtist;

    vector<int> &oldVec = db.artistIndex[oldArtist];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.artistIndex.erase(oldArtist);

    db.artistIndex[newArtist].push_back(selectedRow);

  } else if (choiceE2 == 3) {
    string oldAlbum = db.data[selectedRow][3];
    cout << "Enter the new album name: ";
    string newAlbum;
    getline(cin, newAlbum);

    db.data[selectedRow][3] = newAlbum;

    vector<int> &oldVec = db.albumIndex[oldAlbum];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.albumIndex.erase(oldAlbum);

    db.albumIndex[newAlbum].push_back(selectedRow);
  }

  cout << "Edit successful!" << endl;
}

int main() {
  SearchDB db;

  db = readCSV("dataset.csv");
  if (db.data.empty()) {
    cout << "Error: dataset cannot be loaded." << endl;
    return 1;
  }
  int choice;
  while (true) {
    cout << "Dataset is loaded successfully! It's ready to use." << endl;
    cout << "======= Welcome to Loz Music Search Engine =======" << endl;
    cout << "1. Search Song/Artist/Album" << endl;
    cout << "2. Add Song/Artist/Album" << endl;
    cout << "3. Delete Song/Artist/Album" << endl;
    cout << "4. Edit Song/Artist/Album" << endl;
    cout << "5. Exit..." << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1: {
      // call search function
      break;
    }
    case 2:
      // call add function
      break;
    case 3:
      // call delete function
      break;
    case 4:
      edit(db);
      break;
    case 5:
      cout << "Exiting... Goodbye!" << endl;
      return 0;
    default:
      cout << "Invalid choice. Please try again." << endl;
    }
  }

  return 0;
}

// int main() {
//   SearchDB db;
//
//   db = readCSV("dataset.csv");
//
//   cout << string(50, '=') << "\n";
//   cout << " FEATURE 1: AUTOCOMPLETE SUGGESTIONS (TRIE)\n";
//   cout << string(50, '=') << "\n";
//
//   // Example 1: User starts typing "Ta" for an artist
//   string prefix = "Ta";
//   cout << "User is typing artist name: '" << prefix << "'\n";
//
//   vector<string> suggestions = db.artistTrie.autocomplete(prefix);
//
//   if (suggestions.empty()) {
//     cout << "No suggestions found.\n";
//   } else {
//     cout << "Autocomplete Suggestions:\n";
//     for (const auto &s : suggestions) {
//       cout << " -> " << s << "\n";
//     }
//   }
//
//   cout << "\n" << string(50, '=') << "\n";
//   cout << " FEATURE 2: DID YOU MEAN? (LEVENSHTEIN)\n";
//   cout << string(50, '=') << "\n";
//
//   // Example 2: User makes a typo while searching
//   string typoQuery = "Ed Sheran"; // Missing an 'e'
//   cout << "User hit 'Enter' searching for: '" << typoQuery << "'\n";
//
//   // First, check if exact match exists using your smartSearch
//   vector<int> results = smartSearch(db, typoQuery);
//
//   if (results.empty()) {
//     cout << "\nNo exact match found. Searching for 'Did you mean?' "
//             "alternatives...\n";
//
//     // Use Levenshtein to find close matches (Threshold of 2 edits)
//     results = levenshteinSearch(db, typoQuery, 2);
//
//     if (results.empty()) {
//       cout << "No close matches found.\n";
//     } else {
//       cout << "Did you mean??...\n";
//       for (int i : results) {
//         // Print out the Artist, Album, and Song for the corrected result
//         cout << " -> Artist: " << db.data[i][2] << " | Album: " <<
//         db.data[i][3]
//              << " | Song: " << db.data[i][4] << "\n";
//       }
//     }
//   } else {
//     cout << "Exact match found!\n";
//   }
//
//   cout << "\n" << string(50, '=') << "\n";
//   cout << " FEATURE 3: ANOTHER TYPO EXAMPLE\n";
//   cout << string(50, '=') << "\n";
//
//   string typoQuery2 = "Taylr Swft"; // Missing 'o' and 'i'
//   cout << "User searched for: '" << typoQuery2 << "'\n";
//
//   results = smartSearch(db, typoQuery2);
//   if (results.empty()) {
//     results = levenshteinSearch(
//         db, typoQuery2, 3); // Slightly higher threshold for 2 missing
//         letters
//     if (!results.empty()) {
//       cout << "Did you mean??...\n";
//       for (int i : results) {
//         cout << " -> Artist: " << db.data[i][2] << " | Album: " <<
//         db.data[i][3]
//              << " | Song: " << db.data[i][4] << "\n";
//       }
//     }
//   }
//
//   return 0;
// }

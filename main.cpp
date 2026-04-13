#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// UNORDERED_MAP IMPLEMENTATION
// This is a custom hash map using linked lists to handle collisions.
template <class S, class T> class unordered_map {
public:
  // Node structure for the linked list to handle hash collisions
  struct ll {
    S key;
    T value;
    ll *next;
  };

  // Fixed size array of pointers to the linked list nodes
  ll *arr[100000];

  // Constructor initializes all pointers to NULL
  unordered_map() {
    for (int i = 0; i < 100000; i++)
      arr[i] = NULL;
  }

  // Inserts a new key-value pair or updates the value if the key already exists
  void insert(S key, T value) {
    int h = hash(key); // Calculate hash bucket index
    if (arr[h] == NULL) {
      // If bucket is empty, create the first node
      ll *p = new ll{key, value, NULL};
      arr[h] = p;
    } else {
      // Traverse the linked list to find an existing key or append to the end
      ll *q = arr[h];
      while (true) {
        if (q->key == key) {
          q->value = value; // Update value if key is found
          return;
        }
        if (q->next == NULL)
          break;
        q = q->next;
      }
      // Append new node at the end of the chain
      q->next = new ll{key, value, NULL};
    }
  }

  // Removes a key-value pair from the map
  void erase(S key) {
    int h = hash(key);
    ll *p = arr[h];
    ll *prev = NULL;

    while (p != NULL) {
      if (p->key == key) {
        // If it's the first node in the bucket
        if (prev == NULL)
          arr[h] = p->next;
        // If it's in the middle or end
        else
          prev->next = p->next;

        delete p; // Free memory
        return;
      }
      prev = p;
      p = p->next;
    }
  }

  // Polynomial rolling hash function to convert a key into an integer index (0
  // to 99999)
  int hash(S key) {
    long long h = 0;
    ostringstream os;
    os << key;
    string key1 = os.str(); // Convert key to string
    long long m = 1;
    // Iterate through string characters to compute hash
    for (int i = 0; i < int(key1.size()); i++) {
      h += int(key1[i]) * m;
      h = h % 100000;
      m = (m * 17) % 100000; // Multiplier to reduce collisions
    }
    return (int)h;
  }

  // Checks if a key exists in the map
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

  // Overloads the [] operator for easy access and insertion (e.g., map["key"] =
  // value)
  T &operator[](S key) {
    int h = hash(key);
    ll *p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value; // Return reference if found
      p = p->next;
    }
    // If key is not found, insert a default-constructed value and return its
    // reference
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

// Trie Node for Autocomplete
struct TrieNode {
  TrieNode *children[26]; //array of 26 pointers
  bool isEnd;
  string word;

  TrieNode() {
    isEnd = false;
    word = ""; //no word ends initially,empty string
    for (int i = 0; i < 26; i++)
      children[i] = NULL;//no connections yet, all pointers starts as NULL
  }
};

// Trie implementation
class Trie {
public:
  TrieNode *root;

  Trie() { root = new TrieNode(); }
  void insert(string s) { //insert a wword into the trie
    TrieNode *node = root;

    for (char c : s) {
      if (!isalpha(c))//ignore invalid char
        continue;
      c = tolower(c);

      int index = c - 'a';

      if (node->children[index] == NULL)
        node->children[index] = new TrieNode(); //create node if needed

      node = node->children[index];
    }
    node->isEnd = true;
    node->word = s;//mark end of word
  }
  void dfs(TrieNode *node, vector<string> &result) { //collect all words under a node (used in autocomplete)
    if (node == NULL)
      return;

    if (node->isEnd)
      result.push_back(node->word);

    for (int i = 0; i < 26; i++)
      dfs(node->children[i], result);
  }
  vector<string> autocomplete(string prefix) { //return all words in the trie that start with a given prefix
    TrieNode *node = root;

    for (char c : prefix) { //loop through each character od the prefix
      if (!isalpha(c)) //skip non-alphabet characters (space/number)
        continue;
      c = tolower(c); //convertz 'a'->0, 'b'->1

      int index = c - 'a';

      if (node->children[index] == NULL) //if no child then prefix does not exist in the TRIE
        return {};

      node = node->children[index];
    }
    vector<string> result;
    dfs(node, result);
    return result; //once prefix node is reached, perform depth-first-search,collect all words under the node
  }

  bool removeThese(TrieNode *node, string word, int depth) {
    if (!node)
      return false;
    if (depth == (int)word.size()) {
      if (!node->isEnd)
        return false; //if no marked as word then invalid deletion
      node->isEnd = false;
      node->word = ""; //remove word ending
      for (int i = 0; i < 26; i++)
        if (node->children[i] != NULL)
          return false;//if node has childen then cannot delete node
      return true;
    }
    int index = tolower(word[depth]) - 'a';
    if (index < 0 || index >= 26)//invalid char check
      return false;
    if (removeThese(node->children[index], word, depth + 1)) {
      delete node->children[index];
      node->children[index] = NULL;//remove unused child node
      if (node->isEnd)
        return false;//if current node is end of another word then keep it
      for (int i = 0; i < 26; i++)
        if (node->children[i] != NULL)
          return false;
      return true;
    }
    return false;//if has other childern cannot delete
  }

  void remove(string word) { removeThese(root, word, 0); }
};

// ==========================================
// INDEXING STRUCTURES & LOGIC
// ==========================================

// Struct holding the core dataset and its optimized lookup structures
struct SearchDB {
  // Master list of all rows read from the CSV
  vector<vector<string>> data;

  // Inverted Indices: Maps a specific string (artist/song/album name) to a list
  // of row numbers in `data`. Using the custom unordered_map for O(1) average
  // lookup time.
  unordered_map<string, vector<int>> artistIndex;
  unordered_map<string, vector<int>> songIndex;
  unordered_map<string, vector<int>> albumIndex;

  // Tries used for autocomplete functionality (finding strings by prefix)
  Trie artistTrie;
  Trie songTrie;
  Trie albumTrie;
};

// Populates the database and builds the indices from the CSV file
SearchDB readCSV(const string &filename) {
  SearchDB db;
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

    // Extract columns assuming format: ID, Artist, Album, Song, [Genre]
    string artist = row[1];
    string album = row[2];
    string song = row[3];

    // Build the inverted indices: map the exact name to its row ID
    db.artistIndex[artist].push_back(currentLine);
    db.songIndex[song].push_back(currentLine);
    db.albumIndex[album].push_back(currentLine);

    // Insert into tries for prefix matching (autocomplete)
    db.artistTrie.insert(artist);
    db.songTrie.insert(song);
    db.albumTrie.insert(album);

    currentLine++;
  }

  file.close();
  return db;
}

void saveToCSV(const string &filename, SearchDB &db) {
  ofstream file(filename);

  if (!file.is_open()) {
    cout << "Error saving file!\n";
    return;
  }
  for (int i = 0; i < db.data.size(); i++) {
    if (db.data[i][1] == "DELETED" || db.data[i][2] == "DELETED" ||
        db.data[i][3] == "DELETED")
      continue;
    for (int j = 0; j < db.data[i].size(); j++) {
      file << db.data[i][j];
      if (j != db.data[i].size() - 1)
        file << ",";
    }
    file << "\n";
  }
  file.close();
}

vector<int> multiKeywordSearch(SearchDB &db, string query) {
  vector<int> rsult; //stores the final filtered result

  stringstream ss(query);
  string word; //split sentenc into words

  bool first = true;

  while (ss >> word) { //read one word at a time
    vector<int> temp; //stores search result for one keyword only

    if (!db.artistIndex[word].empty())
      for (int x : db.artistIndex[word])
        temp.push_back(x);
    else if (!db.songIndex[word].empty())
      for (int x : db.songIndex[word])
        temp.push_back(x);
    else if (!db.albumIndex[word].empty())
      for (int x : db.albumIndex[word])
        temp.push_back(x); //copy all matching indices into temp
    if (first) {
      rsult = temp;
      first = false;
    } else {
      vector<int> newResult; //for second keyword onward, find common elements
      for (int x : rsult) {
        for (int y : temp) {
          if (x == y) {
            newResult.push_back(x);//compare previous (rsult) and current keyword resuly (temp) keep only common values
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

  return multiKeywordSearch(db, query); //if query not found then fallback into multikeywordsearch
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

// ==========================================
// SORTING FUNCTIONS (MERGE SORT)
// ==========================================

// Helper function to merge two sorted halves of index arrays
void merge(vector<int> &indices, int left, int mid, int right, SearchDB &db,
           int col, bool ascending) {
  // Copy data to temporary arrays L[] and R[]
  vector<int> L(indices.begin() + left, indices.begin() + mid + 1);
  vector<int> R(indices.begin() + mid + 1, indices.begin() + right + 1);

  int i = 0, j = 0, k = left;

  // Merge the temp arrays back into indices[left..right]
  while (i < L.size() && j < R.size()) {
    // Look up the actual string data from the DB using the index and target
    // column
    string a = db.data[L[i]][col];
    string b = db.data[R[j]][col];

    // Determine sort order condition based on boolean flag
    bool takeLeft = ascending ? (a <= b) : (a >= b);

    if (takeLeft)
      indices[k++] = L[i++];
    else
      indices[k++] = R[j++];
  }

  // Copy any remaining elements of L[]
  while (i < L.size())
    indices[k++] = L[i++];

  // Copy any remaining elements of R[]
  while (j < R.size())
    indices[k++] = R[j++];
}

// Recursive function to split the indices array and sort it using Merge Sort
void mergeSort(vector<int> &indices, int left, int right, SearchDB &db, int col,
               bool ascending) {
  if (left >= right)
    return; // Base case: subarray size is 1
  int mid = (left + right) / 2;

  // Recursively sort the left and right halves
  mergeSort(indices, left, mid, db, col, ascending);
  mergeSort(indices, mid + 1, right, db, col, ascending);

  // Merge the sorted halves
  merge(indices, left, mid, right, db, col, ascending);
}

// Wrapper function to initiate the merge sort on query results
void sortResults(vector<int> &indices, SearchDB &db, int col = 2,
                 bool ascending = true) {
  if (indices.empty())
    return;
  // Note: We sort an array of `indices` (integers pointing to db.data rows)
  // rather than shuffling the db.data rows themselves.
  mergeSort(indices, 0, indices.size() - 1, db, col, ascending);
}

void edit(SearchDB &db) {
  cout << "======= Edit Mode =======" << endl;
  cout << "Enter the song name that you want to edit: ";
  string songName;
  getline(cin, songName);

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
      cout << i + 1 << ". Song: " << db.data[r][3] << "\n"
           << "   Artist: " << db.data[r][1] << "\n"
           << "   Album: " << db.data[r][2] << "\n";
    }
    cout << "Enter the number of the song you want to edit: ";
    int choiceE1;

    if (!(cin >> choiceE1)) {
      cout << "Invalid input. Exiting edit mode." << endl;
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choiceE1 < 1 || choiceE1 > (int)matchingRows.size()) {
      cout << "Invalid choice. Exiting edit mode." << endl;
      return;
    }
    selectedRow = matchingRows[choiceE1 - 1];
  } else {
    selectedRow = matchingRows[0];
  }

  cout << "== What do you want to edit? ==" << endl;
  cout << "1. Song\n2. Artist\n3. Album\n";
  cout << "Enter your choice: ";
  int choiceE2;

  if (!(cin >> choiceE2)) {
    cout << "Invalid input. Exiting..." << endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return;
  }
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  if (choiceE2 < 1 || choiceE2 > 3) {
    cout << "Invalid choice. Exiting..." << endl;
    return;
  }

  if (choiceE2 == 1) {
    string oldSong = db.data[selectedRow][3];
    cout << "Enter the new song name: ";
    string newSong;
    getline(cin, newSong);

    db.data[selectedRow][3] = newSong;

    vector<int> &oldVec = db.songIndex[oldSong];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.songIndex.erase(oldSong);

    db.songIndex[newSong].push_back(selectedRow);
    db.songTrie.remove(oldSong);
    db.songTrie.insert(newSong);

  } else if (choiceE2 == 2) {
    string oldArtist = db.data[selectedRow][1];
    cout << "Enter the new artist name: ";
    string newArtist;
    getline(cin, newArtist);

    db.data[selectedRow][1] = newArtist;

    vector<int> &oldVec = db.artistIndex[oldArtist];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.artistIndex.erase(oldArtist);

    db.artistIndex[newArtist].push_back(selectedRow);
    db.artistTrie.remove(oldArtist);
    db.artistTrie.insert(newArtist);

  } else if (choiceE2 == 3) {
    string oldAlbum = db.data[selectedRow][2];
    cout << "Enter the new album name: ";
    string newAlbum;
    getline(cin, newAlbum);

    db.data[selectedRow][2] = newAlbum;

    vector<int> &oldVec = db.albumIndex[oldAlbum];
    oldVec.erase(remove(oldVec.begin(), oldVec.end(), selectedRow),
                 oldVec.end());
    if (oldVec.empty())
      db.albumIndex.erase(oldAlbum);

    db.albumIndex[newAlbum].push_back(selectedRow);
    db.albumTrie.remove(oldAlbum);
    db.albumTrie.insert(newAlbum);
  }

  saveToCSV("dataset.csv", db);
  cout << "Edit successful!" << endl;
}

void deleteArtist(SearchDB &db) {
  cout << "Enter the artist name to delete: ";
  string artist;
  getline(cin, artist);

  if (!db.artistIndex.find(artist)) {//check if artist exists
    cout << "Artist not found.\n";
    return;
  }

  vector<int> rows = db.artistIndex[artist];//all record of this artist
  vector<int> validRows;//fitered (not deleted)

  cout << "\nSongs by this artist:\n";
  for (int i : rows) {
    if (db.data[i][1] == "DELETED") //already deleted entries
      continue;
    cout << "  - " << db.data[i][3] << " (Album: " << db.data[i][2] << ")\n";
    validRows.push_back(i); //store valid rows
  }

  if (validRows.empty()) {
    cout << "No valid entries found.\n";
    return;
  }

  cout << "Confirm delete ALL songs by \"" << artist << "\"? (yes/no): ";
  string confirm;
  cin >> confirm;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  if (confirm != "yes") {
    cout << "Delete cancelled.\n";
    return;
  }

  for (int i : validRows) {//loop through valid rows
    string song = db.data[i][3];
    string album = db.data[i][2];//extract info

    db.data[i][1] = "DELETED";
    db.data[i][3] = "DELETED";

    vector<int> &sv = db.songIndex[song];//update song index
    sv.erase(remove(sv.begin(), sv.end(), i), sv.end()); //remoe row index
    if (sv.empty())
      db.songIndex.erase(song);

    vector<int> &av = db.albumIndex[album];
    av.erase(remove(av.begin(), av.end(), i), av.end());
    if (av.empty())
      db.albumIndex.erase(album);
  }

  db.artistIndex.erase(artist);
  db.artistTrie.remove(artist);
  saveToCSV("dataset.csv", db);
  cout << "Artist deleted successfully.\n";
}

void deleteSong(SearchDB &db) {
  cout << "Enter the song name to delete: ";
  string song;
  getline(cin, song);

  if (!db.songIndex.find(song)) {
    cout << "Song not found.\n";
    return;
  }

  vector<int> rows = db.songIndex[song];
  vector<int> validRows;

  cout << "\nSearch results:\n";
  for (int i : rows) {
    if (db.data[i][3] == "DELETED")
      continue;
    cout << i + 1 << ". \"" << db.data[i][3] << "\" by " << db.data[i][1]
         << " (Album: " << db.data[i][2] << ")\n";
    validRows.push_back(i);
  }

  if (validRows.empty()) {
    cout << "No valid songs found.\n";
    return;
  }

  int selected = -1;
  if (validRows.size() == 1) {
    selected = validRows[0];
  } else {
    cout << "Enter the number of the song to delete: ";
    int pick;

    if (!(cin >> pick)) {
      cout << "Invalid choice. Cancelled.\n";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (pick < 1 || pick > (int)validRows.size()) {
      cout << "Invalid choice.\n";
      return;
    }
    selected = validRows[pick - 1];
  }

  cout << "Confirm delete \"" << db.data[selected][3] << "\"? (yes/no): ";
  string confirm;
  cin >> confirm;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  if (confirm != "yes") {
    cout << "Delete cancelled.\n";
    return;
  }

  string album = db.data[selected][2];
  string artist = db.data[selected][1];

  db.data[selected][3] = "DELETED";

  vector<int> &sv = db.songIndex[song];
  sv.erase(remove(sv.begin(), sv.end(), selected), sv.end());
  if (sv.empty())
    db.songIndex.erase(song);

  vector<int> &av = db.albumIndex[album];
  av.erase(remove(av.begin(), av.end(), selected), av.end());
  if (av.empty())
    db.albumIndex.erase(album);

  vector<int> &arv = db.artistIndex[artist];
  arv.erase(remove(arv.begin(), arv.end(), selected), arv.end());
  if (arv.empty())
    db.artistIndex.erase(artist);

  db.songTrie.remove(song);
  saveToCSV("dataset.csv", db);
  cout << "Song deleted successfully.\n";
}

void deleteAlbum(SearchDB &db) {
  cout << "Enter the album name to delete: ";
  string album;
  getline(cin, album);

  if (!db.albumIndex.find(album)) {
    cout << "Album not found.\n";
    return;
  }

  vector<int> rows = db.albumIndex[album];
  vector<int> validRows;

  cout << "\nSongs in this album:\n";
  for (int i : rows) {
    if (db.data[i][2] == "DELETED")
      continue;
    cout << "  - " << db.data[i][3] << " by " << db.data[i][1] << "\n";
    validRows.push_back(i);
  }

  if (validRows.empty()) {
    cout << "No valid entries found.\n";
    return;
  }

  cout << "Confirm delete ALL songs in album \"" << album << "\"? (yes/no): ";
  string confirm;
  cin >> confirm;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  if (confirm != "yes") {
    cout << "Delete cancelled.\n";
    return;
  }

  for (int i : validRows) {
    string song = db.data[i][3];
    string artist = db.data[i][1];

    db.data[i][2] = "DELETED";
    db.data[i][3] = "DELETED";

    vector<int> &sv = db.songIndex[song];
    sv.erase(remove(sv.begin(), sv.end(), i), sv.end());
    if (sv.empty())
      db.songIndex.erase(song);

    vector<int> &arv = db.artistIndex[artist];
    arv.erase(remove(arv.begin(), arv.end(), i), arv.end());
    if (arv.empty())
      db.artistIndex.erase(artist);
  }

  db.albumIndex.erase(album);
  db.albumTrie.remove(album);
  saveToCSV("dataset.csv", db);
  cout << "Album deleted successfully.\n";
}

void addSong(SearchDB &db) {
  cout << "======= Add Song =======" << endl;

  string artist, album, song, genre;

  cout << "Enter artist name: ";
  getline(cin, artist);

  cout << "Enter album name: ";
  getline(cin, album);

  cout << "Enter song name: ";
  getline(cin, song);

  cout << "Enter genre name: ";
  getline(cin, genre);

  int newIndex = db.data.size();
  string newId = to_string(newIndex);

  vector<string> newRow = {newId, artist, album, song, genre};
  db.data.push_back(newRow);
  db.artistIndex[artist].push_back(newIndex);
  db.songIndex[song].push_back(newIndex);
  db.albumIndex[album].push_back(newIndex);
  db.artistTrie.insert(artist);
  db.songTrie.insert(song);
  db.albumTrie.insert(album);
  saveToCSV("dataset.csv", db);

  cout << "Song added successfully!\n";
}

// ==========================================
// MAIN FUNCTION (ENTRY POINT & CLI LOOP)
// ==========================================
int main() {
  SearchDB db;

  // Initial load: parse the CSV file and build up the tries/indices in memory
  db = readCSV("dataset.csv");
  if (db.data.empty()) {
    cout << "Error: dataset cannot be loaded." << endl;
    return 1;
  }

  int choice;
  // Infinite loop for the Command Line Interface (CLI) menu
  while (true) {
    cout << "\nDataset is loaded successfully! It's ready to use." << endl;
    cout << "======= Welcome to Loz Music Search Engine =======" << endl;
    cout << "1. Search Song/Artist/Album" << endl;
    cout << "2. Add Song/Artist/Album" << endl;
    cout << "3. Delete Song/Artist/Album" << endl;
    cout << "4. Edit Song/Artist/Album" << endl;
    cout << "5. Exit..." << endl;
    cout << "Enter your choice: ";

    // Handle string inputs for numeric menu to prevent infinite loop errors
    if (!(cin >> choice)) {
      cout << "Invalid input. Please enter a valid number." << endl;
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      continue;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (choice) {
    case 1: {
      // --- Search Flow ---
      cout << "======= Search Mode =======" << endl;
      cout << "Enter your search query: ";
      string query;
      getline(cin, query);

      // 1. Autocomplete Suggestions (using Trie)
      // Provides "typeahead" hints based on the artist names that match the
      // query prefix
      vector<string> suggestions = db.artistTrie.autocomplete(query);
      if (!suggestions.empty()) {
        cout << "\n[Tip] Artist Autocomplete Suggestions:" << endl;
        int c = 0;
        for (const auto &s : suggestions) {
          cout << " -> " << s << endl;
          c++;
          // Cap suggestions at 5, hide the rest
          if (c == 5 && suggestions.size() > c) {
            cout << "\nThere are " << suggestions.size() - c
                 << " more suggestions that are hidden" << endl;
            break;
          }
        }
      }

      // 2. Exact & Multi-Keyword Search
      // Uses the O(1) hash maps to find exact term matches quickly
      vector<int> results = smartSearch(db, query);

      // 3. Did you mean? (Levenshtein search fallback)
      // If exact matches yield no results, use edit distance to find typos
      if (results.empty()) {
        cout << "\nNo exact match found. Searching for 'Did you mean?' "
                "alternatives..."
             << endl;
        // Search the DB allowing up to 2 typos (insertions, deletions,
        // mutations)
        results = levenshteinSearch(db, query, 2);

        if (results.empty()) {
          cout << "No close matches found either.\n" << endl;
          break;
        } else {
          cout << "Did you mean?..." << endl;
        }
      } else {
        cout << "\nMatch(es) found!" << endl;
      }

      // Print the raw (unsorted) search results mapped from their row indices
      cout << "\n--- Search Results ---" << endl;
      for (int i = 0; i < (int)results.size(); i++) {
        int r = results[i]; // Get actual row index
        cout << i + 1 << ". Song: " << db.data[r][3]
             << " | Artist: " << db.data[r][1] << " | Album: " << db.data[r][2]
             << endl;

        // Cap results printing to top 5 if it was a fuzzy match lookup
        if (i == 5 && results.size() > i) {
          cout << "\nThere are " << results.size() - i
               << " more 'Did You Mean?' that are hidden" << endl;
          break;
        }
      }

      // 4. Sorting Functionality
      // After retrieving results, optionally re-order them dynamically
      if (!results.empty()) {
        cout << "\nDo you want to sort these results? (y/n): ";
        string sortChoice;
        getline(cin, sortChoice);

        if (sortChoice == "y" || sortChoice == "Y") {
          cout << "Sort by (1=Artist, 2=Album, 3=Song): ";
          int colChoice;

          if (!(cin >> colChoice)) {
            cout << "Invalid input. Skipping sort." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
          }

          cout << "Order (1=Ascending A-Z, 2=Descending Z-A): ";
          int orderChoice;

          if (!(cin >> orderChoice)) {
            cout << "Invalid input. Skipping sort." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
          }
          cin.ignore(numeric_limits<streamsize>::max(), '\n');

          // Map choice integer back to the correct CSV column index (0=ID,
          // 1=Artist, 2=Album, 3=Song)
          int col = 3;
          if (colChoice == 1)
            col = 1;
          else if (colChoice == 2)
            col = 2;

          bool ascending = (orderChoice == 1);

          // Apply Merge Sort to the array of matched integer IDs based on the
          // selected column text
          sortResults(results, db, col, ascending);

          // Print freshly sorted output
          cout << "\n--- Sorted Results ---" << endl;
          for (int i = 0; i < (int)results.size(); i++) {
            int r = results[i];
            cout << i + 1 << ". Song: " << db.data[r][3]
                 << " | Artist: " << db.data[r][1]
                 << " | Album: " << db.data[r][2] << endl;
          }
        }
      }
      break;
    }
    case 2:
      addSong(db); // Branches to insertion logic
      break;
    case 3: {
      // Cascading deletion branches depending on user target scope
      cout << "What do you want to delete? (song/album/artist): ";
      string delType;
      cin >> delType;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      for (char &c : delType)
        c = tolower(c);

      if (delType == "song")
        deleteSong(db);
      else if (delType == "artist")
        deleteArtist(db);
      else if (delType == "album")
        deleteAlbum(db);
      else
        cout << "Invalid type.\n";
      break;
    }
    case 4:
      edit(db); // Branches to update logic
      break;
    case 5:
      // Graceful termination
      cout << "Exiting... Goodbye!" << endl;
      return 0;
    default:
      cout << "Invalid choice. Please try again." << endl;
    }
  }
}

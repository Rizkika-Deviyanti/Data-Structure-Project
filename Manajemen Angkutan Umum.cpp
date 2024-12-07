#include <iostream>
#include <string>
#include <queue>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <limits>
#include <utility>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <map>

using namespace std;

struct PathResult {
        vector<string> route;
        int total_weight;
    };
const int INT_MAX_VAL = numeric_limits<int>::max();

class User {
public:
    User(const string& username, const string& password, bool isAdmin = false)
        : username(username), password(password), isAdmin(isAdmin), loggedIn(false) {}

    bool login(const string& enteredPassword) {
        if (enteredPassword == password) {
            loggedIn = true;
            return true;
        }
        return false;
    }

    void logout() {
        loggedIn = false;
    }

    bool isLoggedIn() const {
        return loggedIn;
    }

    bool isAdminUser() const {
        return isAdmin;
    }

    string getUsername() const {
        return username;
    }

private:
    string username;
    string password;
    bool isAdmin;
    bool loggedIn;
};



class Graph {
private:
    vector<string> cities; // Vektor dinamis untuk menyimpan nama kota
    map<string, int> vertex_map; // Untuk memetakan nama kota ke indeks
    vector<vector<int>> adj_matrix;
    vector<vector<int>> adj_matrix1;

public:
    PathResult result;

    Graph() {
        adj_matrix.resize(100, vector<int>(100, 0)); // Maksimum 100 kota
        adj_matrix1.resize(100, vector<int>(100, 0)); // Maksimum 100 kota
    }

    void add_city(const string& city) {
        if (cities.size() >= 100) {
            throw overflow_error("Maximum number of cities reached");
        }
        cities.push_back(city);
        vertex_map[city] = cities.size() - 1;
    }

    void add_edge(const string& from_city, const string& to_city, int weight = 1, int weight1 = 1) {
        if (vertex_map.find(from_city) == vertex_map.end() || vertex_map.find(to_city) == vertex_map.end()) {
            throw invalid_argument("City not found in graph");
        }
        int from_vertex = vertex_map[from_city];
        int to_vertex = vertex_map[to_city];
        adj_matrix[from_vertex][to_vertex] = weight;
        adj_matrix[to_vertex][from_vertex] = weight; // For undirected graph
        adj_matrix1[from_vertex][to_vertex] = weight1;
        adj_matrix1[to_vertex][from_vertex] = weight1; // For undirected graph
    }

    void remove_edge(const string& from_city, const string& to_city) {
        if (vertex_map.find(from_city) == vertex_map.end() || vertex_map.find(to_city) == vertex_map.end()) {
            throw invalid_argument("City not found in graph");
        }
        int from_vertex = vertex_map[from_city];
        int to_vertex = vertex_map[to_city];
        adj_matrix[from_vertex][to_vertex] = 0;
        adj_matrix[to_vertex][from_vertex] = 0; // For undirected graph
        adj_matrix1[from_vertex][to_vertex] = 0;
        adj_matrix1[to_vertex][from_vertex] = 0; // For undirected graph
    }

    void remove_city(const string& city) {
        if (vertex_map.find(city) == vertex_map.end()) {
            throw invalid_argument("City not found in graph");
        }
        int city_index = vertex_map[city];
        cities.erase(cities.begin() + city_index); // Menghapus kota dari vektor cities
        vertex_map.erase(city); // Menghapus kota dari map
        // Menghapus semua jalur yang terhubung ke kota yang dihapus
        for (size_t i = 0; i < cities.size(); ++i) {
            adj_matrix[i][city_index] = 0;
            adj_matrix[city_index][i] = 0;
            adj_matrix1[i][city_index] = 0;
            adj_matrix1[city_index][i] = 0;
        }
    }

    const vector<string>& get_cities() const {
        return cities;
    }

    const vector<vector<int>>& get_adj_matrix() const {
        return adj_matrix;
    }
    const vector<vector<int>>& get_adj_matrix1() const {
        return adj_matrix1;
    }

    int get_vertex_index(const string& city) const {
        auto it = vertex_map.find(city);
        if (it == vertex_map.end()) {
            throw invalid_argument("City not found in graph");
        }
        return it->second;
    }

    void print_adj_matrix_with_names() const {
        // Mencetak header nama kota
        cout << setw(12) << " ";
        for (const auto& city : cities) {
            cout << setw(12) << city;
        }
        cout << endl;

        // Mencetak adjacency matrix dengan nama kota
        for (size_t i = 0; i < cities.size(); ++i) {
            cout << setw(12) << cities[i];
            for (size_t j = 0; j < cities.size(); ++j) {
                cout << setw(12) << adj_matrix[i][j];
            }
            cout << endl;
        }
    }
    void print_adj_matrix_with_names1() const {
        // Mencetak header nama kota
        cout << setw(12) << " ";
        for (const auto& city : cities) {
            cout << setw(12) << city;
        }
        cout << endl;

        // Mencetak adjacency matrix dengan nama kota
        for (size_t i = 0; i < cities.size(); ++i) {
            cout << setw(12) << cities[i];
            for (size_t j = 0; j < cities.size(); ++j) {
                cout << setw(12) << adj_matrix1[i][j];
            }
            cout << endl;
        }
    }

    PathResult dijkstra(const string& start_city, const string& end_city, bool tipe = 0) {
    int start_index = get_vertex_index(start_city);
    int end_index = get_vertex_index(end_city);
    const vector<string>& cities = get_cities();
    
    const vector<vector<int>>& adj_matrix = (tipe == 0) ? get_adj_matrix() : get_adj_matrix1();

    vector<int> dist(cities.size(), 100);
    vector<int> parent(cities.size(), -1);
    dist[start_index] = 0;

    using Node = pair<int, int>; // {distance, vertex}
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    pq.push({0, start_index});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (u == end_index) break;

        for (int v = 0; v < cities.size(); ++v) {
            if (adj_matrix[u][v] != 0) {
                int weight = adj_matrix[u][v];
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }

    if (dist[end_index] == 100) {
        return { {}, -1 }; // No path exists
    }

    result.total_weight = dist[end_index];

    // Membangun jalur dari end_city ke start_city
    for (int v = end_index; v != -1; v = parent[v]) {
        result.route.push_back(cities[v]);
    }

    // Membalikkan rute secara manual
    int n = result.route.size();
    for (int i = 0; i < n / 2; ++i) {
        swap(result.route[i], result.route[n - 1 - i]);
    }

    return result;
}


    void print_dijkstra_result(const PathResult& result) {
    if (result.total_weight == -1) {
        cout << "No path exists." << endl;
    } else {
        cout << "Best path: ";
        for (size_t i = 0; i < result.route.size(); ++i) {
            cout << result.route[i];
            if (i < result.route.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl << "Total price/distance: " << result.total_weight << endl;
    }
}
};

class UserManager {
public:
    bool registerUser(const string& username, const string& password, bool isAdmin) {
        auto it = users.find(username);
        if (it != users.end()) {
            cout << "Username already exists. Please choose another one." << endl;
            return false;
        }
        users.insert(make_pair(username, User(username, password, isAdmin)));
        return true;
    }

    bool loginUser(const string& username, const string& password) {
        auto it = users.find(username);
        if (it != users.end()) {
            if (it->second.login(password)) {
                currentUser = &(it->second);
                return true;
            }
        }
        return false;
    }

    void logoutCurrentUser() {
        if (currentUser) {
            currentUser->logout();
            currentUser = nullptr;
        }
    }

    User* getCurrentUser() const {
        return currentUser;
    }

    bool isAdminLoggedIn() const {
        if (currentUser) {
            return currentUser->isAdminUser();
        }
        return false;
    }

private:
    unordered_map<string, User> users;
    User* currentUser = nullptr;
};

class Jadwal {
public:
    string asal;
    string tujuan;
    string waktu_keberangkatan;
    string waktu_kedatangan;
    string kendaraan;
    int harga;

    void tampilkan() const {
        cout << setw(8) << asal << setw(8) << tujuan
             << setw(17) << waktu_keberangkatan << setw(17) << waktu_kedatangan << setw(17) << "Rp "<<harga<< endl;
    }

    string pilihKendaraan() {
        int pilihan;
        do {
            cout << "Pilih jenis kendaraan:\n";
            cout << "1. Bus\n";
            cout << "2. Kereta\n";
            cout << "Pilihan Anda: ";
            cin >> pilihan;
            if (pilihan != 1 && pilihan != 2) {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            }
        } while (pilihan != 1 && pilihan != 2);

        return (pilihan == 1) ? "Bus" : "Kereta";
    }

    void tambahkanJadwal(queue<Jadwal>& s, Jadwal jadwal_baru) {
        s.push(jadwal_baru);
    }

    void hapusJadwal(queue<Jadwal>& s) {
        if (s.empty()) {
            cout << "Tidak ada jadwal yang tersedia\n";
            return;
        }
        s.pop();
    }

    void tampilkanJadwal(queue<Jadwal> s) const {
        if (s.empty()) {
            cout << "Tidak ada jadwal yang tersedia\n";
            return;
        }
        cout << "====================================================================" << endl;
        cout << setw(8) << "Asal" << setw(8) << "Tujuan"
             << setw(17) << "Keberangkatan" << setw(17) << "Kedatangan" << setw(17) << "Harga "<< endl;
        cout << "====================================================================" << endl;
        while (!s.empty()) {
            s.front().tampilkan();
            s.pop();
        }
        cout << "====================================================================" << endl;
    }
};

Jadwal jadwal;
vector<Graph> Kendaraan;
UserManager userManager;
queue<Jadwal> jadwal_bus, jadwal_kereta;
Graph bus, kereta;
unordered_set<string> kota;

void session() {
    Kendaraan.push_back(bus);
    Kendaraan.push_back(kereta);
    char choice;
    bool isRunning = true;

    while (isRunning) {
        if (!userManager.getCurrentUser()) {
            cout << "Choose an option:" << endl;
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "Your choice: ";
            cin >> choice;

            if (choice == '1') {
                string username, password;
                cout << "Username: ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                cout << "Register as (a) admin or (u) user: ";
                char userType;
                cin >> userType;
                if (userType == 'a' || userType == 'A') {
                    if (userManager.registerUser(username, password, true)) {
                        cout << "Registration as admin successful. Please login." << endl;
                    }
                } else if (userType == 'u' || userType == 'U') {
                    if (userManager.registerUser(username, password, false)) {
                        cout << "Registration as user successful. Please login." << endl;
                    }
                } else {
                    cout << "Invalid choice for user type." << endl;
                }
            } else if (choice == '2') {
                string username, password;
                cout << "Username: ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                if (userManager.loginUser(username, password)) {
                    cout << "Login successful." << endl;
                } else {
                    cout << "Login failed. Invalid username or password." << endl;
                }
            } else {
                cout << "Invalid choice." << endl;
            }
        } else {
            cout << "\nLogged in as: " << userManager.getCurrentUser()->getUsername() << endl;
            if (userManager.isAdminLoggedIn()) {
                cout << "\nAdmin Options:\n";
                cout << "1. Add Schedule\n";
                cout << "2. Remove Schedule\n";
            }
            cout << "3. Show Route\n";
            cout << "4. Show Schedule\n";
            cout << "5. Show Cheapest Route\n";
            cout << "6. Show Closest Route\n";
            cout << "7. Logout\n";
            cout << "Your choice: ";
            cin >> choice;

            switch (choice) {
                case '1': {
                    // Add schedule
                    if (!userManager.isAdminLoggedIn()) {
                        cout << "Only admin can add schedules." << endl;
                        break;
                    }
                    string jenis_kendaraan = jadwal.pilihKendaraan();
                    Jadwal jadwal_baru;
                    cout << "Masukkan Asal: ";
                    cin >> jadwal_baru.asal;
                    if (kota.find(jadwal_baru.asal) == kota.end()) {
                        Kendaraan[0].add_city(jadwal_baru.asal);
                        Kendaraan[1].add_city(jadwal_baru.asal);
                        kota.insert(jadwal_baru.asal);
                    }
                    cout << "Masukkan Tujuan: ";
                    cin >> jadwal_baru.tujuan;
                    if (kota.find(jadwal_baru.tujuan) == kota.end()) {
                        Kendaraan[0].add_city(jadwal_baru.tujuan);
                        Kendaraan[1].add_city(jadwal_baru.tujuan);
                        kota.insert(jadwal_baru.tujuan);
                    }
                    cout << "Masukkan Waktu Keberangkatan: ";
                    cin >> jadwal_baru.waktu_keberangkatan;
                    cout << "Masukkan Waktu Kedatangan: ";
                    cin >> jadwal_baru.waktu_kedatangan;
                    cout << "Masukkan Harga: ";
                    cin >> jadwal_baru.harga;
                    int jarak;
                    cout <<"Masukkan Jarak Kota (dalam kilometer): ";
                    cin >>jarak; 
                    jadwal_baru.kendaraan = jenis_kendaraan;
                    if (jenis_kendaraan == "Bus") {
                        jadwal.tambahkanJadwal(jadwal_bus, jadwal_baru);
                        Kendaraan[0].add_edge(jadwal_baru.asal, jadwal_baru.tujuan, jadwal_baru.harga,jarak);
                    } else {
                        jadwal.tambahkanJadwal(jadwal_kereta, jadwal_baru);
                        Kendaraan[1].add_edge(jadwal_baru.asal, jadwal_baru.tujuan, jadwal_baru.harga,jarak);
                    }
                    break;
                }
                case '2': {
                    // Remove schedule
                    if (!userManager.isAdminLoggedIn()) {
                        cout << "Only admin can remove schedules." << endl;
                        break;
                    }
                    string jenis_kendaraan = jadwal.pilihKendaraan();
                    if (jenis_kendaraan == "Bus")
                        jadwal.hapusJadwal(jadwal_bus);
                    else
                        jadwal.hapusJadwal(jadwal_kereta);
                    break;
                }
                case '3': {
                    int choice;
                    // Show route
                    string jenis_kendaraan = jadwal.pilihKendaraan();
                    cout << "Rute untuk kendaraan " << jenis_kendaraan << endl;
                    if (jenis_kendaraan == "Bus") {
                        cout<<"Pilih tampilkan berdasarkan:"<<endl<<"1.Harga"<<endl<<"2.Jarak"<<endl;
                        cin>>choice;
                        if(choice==1)Kendaraan[0].print_adj_matrix_with_names();
                        else Kendaraan[0].print_adj_matrix_with_names1();
                    } else {
                        cout<<"Pilih tampilkan berdasarkan:"<<endl<<"1.Harga"<<endl<<"2.Waktu"<<endl;
                        cin>>choice;
                        if(choice==2)Kendaraan[1].print_adj_matrix_with_names();
                        else Kendaraan[1].print_adj_matrix_with_names1();
                    }
                    break;
                }
                case '4': {
                    // Show schedule
                    cout << "\nJadwal Bus:\n";
                    jadwal.tampilkanJadwal(jadwal_bus);
                    cout << "\nJadwal Kereta:\n";
                    jadwal.tampilkanJadwal(jadwal_kereta);
                    break;
                }

                case '5': {
                    string kota_asal;
                    string kota_tujuan;
                    int kendaraan;
                    cout<<"\nPilih Jenis Kendaraan\n 1.Bus\n 2.Kereta\n";
                    cin>>kendaraan;
                    cout<<"\nPilih Kota Asal:";
                    cin>>kota_asal;            
                    cout<<"\nPilih Kota Tujuan:\n";
                    cin>>kota_tujuan;
                    if(kendaraan==1){
                        Kendaraan[0].print_dijkstra_result(Kendaraan[0].dijkstra(kota_asal,kota_tujuan));
                    }
                    else{
                        Kendaraan[1].print_dijkstra_result(Kendaraan[1].dijkstra(kota_asal,kota_tujuan));
                    }
                    break;            
                }
                case '6': {
                    string kota_asal;
                    string kota_tujuan;
                    int kendaraan;
                    cout<<"\nPilih Jenis Kendaraan\n 1.Bus\n 2.Kereta\n";
                    cin>>kendaraan;
                    cout<<"\nPilih Kota Asal:";
                    cin>>kota_asal;            
                    cout<<"\nPilih Kota Tujuan:\n";
                    cin>>kota_tujuan;
                    if(kendaraan==1){
                        Kendaraan[0].print_dijkstra_result(Kendaraan[0].dijkstra(kota_asal,kota_tujuan,1));
                    }
                    else {
                        Kendaraan[1].print_dijkstra_result(Kendaraan[1].dijkstra(kota_asal,kota_tujuan,1));
                    } 
                    break;           
                }
                

                case '7': {
                    // Logout
                    userManager.logoutCurrentUser();
                    cout << "Logged out successfully." << endl;
                    break;
                }
                default:
                    cout << "Invalid choice." << endl;
            }
        }
    }
}

int main() {
    session();
    return 0;
}

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath> // Diperlukan untuk std::abs dan std::max/min

// --- Konfigurasi Dunia ---
const float WORLD_WIDTH = 2000.f;
const float WORLD_HEIGHT = 2000.f;
const float WALL_THICKNESS = 100.f; // Ketebalan tembok batas

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    sf::CircleShape shape;
    float radius;

    Particle(float x, float y, float m, float r) {
        position = sf::Vector2f(x, y);
        velocity = sf::Vector2f(0.f, 0.f);
        mass = m;
        radius = r;

        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Cyan);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
    }

    void update(sf::Vector2f force, float dt, const std::vector<sf::RectangleShape>& walls) {
        sf::Vector2f acceleration = force / mass;
        
        // Menambah kecepatan
        velocity += acceleration * dt;
        
        // GESEKAN (Friction): 
        // Ubah angka ini untuk mengatur kelicinan. 
        // 0.95 = Licin (seperti es), 0.90 = Kesat (seperti karpet)
        velocity *= 0.96f; 

        // --- PROSES SUMBU X ---
        float oldX = position.x;
        position.x += velocity.x * dt;
        
        // Cek tabrakan X
        shape.setPosition(position); // Update visual sementara untuk cek
        if (checkCollision(walls) || checkWorldBoundsX()) {
            position.x = oldX;      // Batalkan gerakan X
            velocity.x *= -0.5f;    // Pantulan pelan
        }

        // --- PROSES SUMBU Y ---
        float oldY = position.y;
        position.y += velocity.y * dt;

        // Cek tabrakan Y
        shape.setPosition(position); // Update visual lagi
        if (checkCollision(walls) || checkWorldBoundsY()) {
            position.y = oldY;      // Batalkan gerakan Y
            velocity.y *= -0.5f;    // Pantulan pelan
        }

        // Finalisasi posisi visual
        shape.setPosition(position);
    }

    // Helper: Cek tabrakan dengan tembok objek
    bool checkCollision(const std::vector<sf::RectangleShape>& walls) {
        sf::FloatRect particleBounds = shape.getGlobalBounds();
        for (const auto& wall : walls) {
            if (particleBounds.intersects(wall.getGlobalBounds())) {
                return true;
            }
        }
        return false;
    }

    // Helper: Cek batas dunia X
    bool checkWorldBoundsX() {
        if (position.x - radius < 0) return true;
        if (position.x + radius > 2000.f) return true; // Sesuai WORLD_WIDTH
        return false;
    }

    // Helper: Cek batas dunia Y
    bool checkWorldBoundsY() {
        if (position.y - radius < 0) return true;
        if (position.y + radius > 2000.f) return true; // Sesuai WORLD_HEIGHT
        return false;
    }

    void reset() {
        position = sf::Vector2f(1000.f, 1000.f);
        velocity = sf::Vector2f(0.f, 0.f);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

int main() {
    // Gunakan sf::Style::Close agar ukuran window tetap, tidak bisa di-resize user
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simulasi Robotika - Peta Rintangan", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
    Particle particle(1000.f, 1000.f, 10.f, 20.f); // Start di tengah
    sf::Clock clock;
    float forceStrength = 15000.f;

    std::vector<sf::RectangleShape> walls;
    
    // Fungsi lambda helper untuk membuat tembok
    auto createWall = [&](float x, float y, float w, float h, sf::Color color = sf::Color(100, 100, 100)) {
        sf::RectangleShape wall(sf::Vector2f(w, h));
        wall.setPosition(x, y);
        wall.setFillColor(color);
        // Tambahkan outline agar tembok lebih tegas terlihat
        wall.setOutlineThickness(2.f);
        wall.setOutlineColor(sf::Color(50, 50, 50));
        walls.push_back(wall);
    };

    // --- UPDATE 1: Membuat Tembok Batas Visual yang Tebal ---
    // Kita menaruhnya tepat di luar area 0-2000 dengan ketebalan WALL_THICKNESS
    sf::Color borderColor(80, 80, 80); // Warna tembok batas sedikit lebih gelap
    
    // Tembok Atas (di atas y=0)
    createWall(-WALL_THICKNESS, -WALL_THICKNESS, WORLD_WIDTH + 2*WALL_THICKNESS, WALL_THICKNESS, borderColor);
    // Tembok Bawah (di bawah y=2000)
    createWall(-WALL_THICKNESS, WORLD_HEIGHT, WORLD_WIDTH + 2*WALL_THICKNESS, WALL_THICKNESS, borderColor);
    // Tembok Kiri (di kiri x=0)
    createWall(-WALL_THICKNESS, 0, WALL_THICKNESS, WORLD_HEIGHT, borderColor);
    // Tembok Kanan (di kanan x=2000)
    createWall(WORLD_WIDTH, 0, WALL_THICKNESS, WORLD_HEIGHT, borderColor);


    // --- UPDATE 2: Menambahkan Lebih Banyak Rintangan Internal ---
    sf::Color obstacleColor(120, 120, 120); // Warna rintangan dalam
    
    // Rintangan yang sudah ada sebelumnya
    createWall(600, 600, 200, 50, obstacleColor);
    createWall(1200, 800, 50, 400, obstacleColor);
    createWall(800, 1200, 400, 50, obstacleColor);

    // Rintangan BARU
    createWall(300, 1500, 150, 150, obstacleColor); // Kotak di kiri bawah
    createWall(1600, 300, 50, 500, obstacleColor);  // Tiang panjang di kanan atas
    createWall(200, 300, 400, 50, obstacleColor);   // Tembok horizontal di kiri atas
    // Bentuk L di tengah agak bawah
    createWall(1000, 1500, 300, 50, obstacleColor); 
    createWall(1000, 1550, 50, 200, obstacleColor);


    // --- Setup Grid Background ---
    sf::VertexArray grid(sf::Lines);
    sf::Color gridColor(60, 60, 70, 150); // Sedikit transparan
    for (float i = 0; i <= WORLD_WIDTH; i += 100.f) {
        grid.append(sf::Vertex(sf::Vector2f(i, 0), gridColor));
        grid.append(sf::Vertex(sf::Vector2f(i, WORLD_HEIGHT), gridColor));
    }
    for (float i = 0; i <= WORLD_HEIGHT; i += 100.f) {
        grid.append(sf::Vertex(sf::Vector2f(0, i), gridColor));
        grid.append(sf::Vertex(sf::Vector2f(WORLD_WIDTH, i), gridColor));
    }

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                particle.reset();
            }
        }

        sf::Vector2f totalForce(0.f, 0.f);
        // Input dari Python Bridge akan masuk sebagai penekanan tombol keyboard di sini
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) totalForce.y -= forceStrength;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) totalForce.y += forceStrength;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) totalForce.x -= forceStrength;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) totalForce.x += forceStrength;

        particle.update(totalForce, dt, walls);

        // --- Update Kamera ---
        float viewHalfW = 800.f / 2.f;
        float viewHalfH = 600.f / 2.f;
        // Clamp posisi kamera agar tidak melihat area hitam di luar tembok batas
        float camX = std::max(viewHalfW, std::min(particle.position.x, WORLD_WIDTH - viewHalfW));
        float camY = std::max(viewHalfH, std::min(particle.position.y, WORLD_HEIGHT - viewHalfH));
        view.setCenter(camX, camY);

        window.clear(sf::Color(20, 20, 40)); // Warna background sedikit lebih gelap

        window.setView(view); // Terapkan kamera

        window.draw(grid); // Gambar grid
        for (const auto& wall : walls) {
            window.draw(wall); // Gambar semua tembok
        }
        particle.draw(window); // Gambar bola

        window.display();
    }

    return 0;
}
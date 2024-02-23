#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>

/// Distance in km
/// mass in kg 
/// Speed in km/s

#define LOG(x) {std::cout << "[INFO]: " << x << std::endl;} 

const float sizeScale = 10000;
const float distanceScale = 500000;

const float simSpeed = 10000000000000;

constexpr int HEIGHT = 800;
constexpr int WIDTH = 1500;
const float gravitationalConst = 6.6f * pow(10, -11);

class GravitySource
{
    sf::Vector2f pos;
    float mass; // In kg
    float size; // In km
    const char* name;

    sf::CircleShape shape;
public:
    sf::Vector2f vel;
    int id = 0;

    GravitySource(int& currentId, const char* name, sf::Vector2f pos, sf::Vector2f vel, float mass, float size, sf::Color color) :
        vel{ vel }, mass{ mass }, size{ size }, id{ currentId }, name{ name }
    {
        currentId++;
        this->mass = mass / pow(10, 20);
        float radius = size / sizeScale;
        this->pos = sf::Vector2f((pos.x / distanceScale) - radius, (pos.y / distanceScale) - radius);
        this->pos += sf::Vector2f(WIDTH / 2, HEIGHT / 2); // Center the circle
        shape.setRadius(size / sizeScale);
        shape.setPosition(pos);
        shape.setFillColor(color);

        LOG(name << " source created successfuly !", "");
        LOG("\t id: " << id);
        LOG("\t size: " << size << "km" << " (radius of " << size / sizeScale << "pix with scale)");
        LOG("\t mass: " << mass << "kg" << " (" << mass * pow(10, 20) << " with scale)");
    }

    sf::Color SetColorBaseOnVelocity()
    {
        float value = sqrt(vel.x * vel.x + vel.y * vel.y) * .1f;
        if (value >= 1)
            value = 1;

        float intensity = lerp(0, 255, value);
        sf::Color color(intensity, 10, 255 - intensity, 255);
        SetColor(color);
        return color;
    }

    float lerp(float a, float b, float f)
    {
        return a * (1.0 - f) + (b * f);
    }

    void Render(sf::RenderWindow& window)
    {
        shape.setPosition(pos);
        window.draw(shape);
    }

    void SetColor(sf::Color color)
    {
        shape.setFillColor(color);
    }

    void UpdatePhysics(std::vector<GravitySource> sources)
    {
        for (GravitySource& source : sources)
        {
            if (source.id == id)
                continue;

            // Getting distance
            float distX = source.GetPos().x - pos.x;
            float distY = source.GetPos().y - pos.y;


            float distance = sqrt(distX * distX + distY * distY);

            float x = (source.pos.x - pos.x) / distance;
            float y = (source.pos.y - pos.y) / distance;
            sf::Vector2f lookAtVector(x, y);

            lookAtVector *= GetGravPull(source.mass, distance);

            vel.x += lookAtVector.x * simSpeed / mass / sizeScale;
            vel.y += lookAtVector.y * simSpeed / mass / sizeScale;

            pos.x += vel.x / distanceScale;
            pos.y += vel.y / distanceScale;

            //LOG(name << " x: " << pos.x << " y: " << pos.y);

        }
    }

    sf::Vector2f GetPos() { return pos; }
    float GetStrenght() 
    { 
        return mass;
    }

    float GetGravPull(float m2, float distance)
    {
        float force = gravitationalConst * (((mass) * (m2)) / (distance * distance));
        return force;
    }
};

int main()
{
    int currentId = 0;
    sf::Vector2f middle(WIDTH / 2, HEIGHT / 2);

    // Divide value by scale to fit in screen
    const long double sunMass = (1.989f * pow(10, 30));
    const long float sunSize = (1.4f * pow(10, 6));
    const long float earthMass = (5.972f * pow(10, 24));
    const long float earthSize = (12.7f * pow(10, 3));

    const double earthSunDist = (150 * pow(10, 6));

    LOG("Using a scale of 1/" << sizeScale << " for size");
    LOG("Using a scale of 1/" << distanceScale << " for distance");
    LOG("earth <-> sun distance: " << earthSunDist << "km" << " (" << earthSunDist/distanceScale << "pix with scale)");


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Jen gravité");
    window.setFramerateLimit(90);

    std::vector<GravitySource> sources;
    sources.push_back(GravitySource(currentId, "Sun", sf::Vector2f(0, 0), sf::Vector2f(0, 0), sunMass, sunSize, sf::Color::Yellow));
    sources.push_back(GravitySource(currentId, "Earth", sf::Vector2f(0, earthSunDist), sf::Vector2f(10, 0), earthMass, earthSize, sf::Color::Blue));
    // 29.78f*pow(10,3)


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) 
                window.close();
        }

        window.clear();

        for (GravitySource& source : sources)
        {
            source.Render(window);
        }

        for (GravitySource& source : sources)
        {
            source.UpdatePhysics(sources);
            //source.SetColorBaseOnVelocity();
        }

        window.display();
    }

    return 0;
}

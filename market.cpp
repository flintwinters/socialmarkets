#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdlib>
#include <string>
#include <array>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.hpp>

using namespace std;
#define bnum 2
typedef double dbl;
typedef array<dbl, bnum> belief;
typedef sf::Vector2f v2f;
class Node;
belief merge(belief a, belief b, dbl (*f)(dbl, dbl)) {
    belief c;
    for (int i = 0; i < a.size(); i++) {c[i] = f(a[i], b[i]);}
    return c;
}
dbl fold(belief a, dbl (*f)(dbl, dbl)) {
    dbl d = a[0];
    for (int i = 1; i < a.size(); i++) {d = f(d, a[i]);}
    return d;
}
dbl magnitude(belief a, belief b) {
    belief c = merge(a, b, [](dbl n, dbl m) {return pow(n - m, 2);});
    return sqrt(abs(fold(c, [](dbl n, dbl m)-> dbl {return n + m;})));
}
dbl uniform(dbl a, dbl b) {
    return ((dbl) rand() / (dbl) RAND_MAX) * (b-a)+a; 
}
dbl uniform(dbl b) {
    return uniform(0, b); 
}
vector<Node*> all;
class Node {
private:
    float radius = 4;
public:
    int ID;
    vector<Node*> to, from;
    belief p, v, a = { };
    dbl comfy = 1;
    dbl bounce = 0.5;
    dbl friction = 0.1;
    sf::CircleShape shape;
    vector<sf::RectangleShape> edges;
    Node() {
        all.push_back(this);
        ID = all.size();
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::White);
    }
    Node(dbl x, dbl y, sf::Color c) : Node() {
        p[0] = x;
        p[1] = y;
        shape.setFillColor(c);
    }
    dbl impressionable(dbl x) {
        if (x < comfy) {return -bounce*(x-comfy)/4;}
        return -bounce*(x-comfy)/100;
    }
    Node* addmutual(Node* n) {
        addfrom(n);
        addto(n);
        return this;
    }
    void addto(Node* n) {
        n->to.push_back(this);
        from.push_back(n);
    }
    void addfrom(Node* n) {
        to.push_back(n);
        n->from.push_back(this);
    }
    void print() {
        string s = to_string(ID) + ": ";
        for (dbl d : p) {s += to_string(d).substr(0, 3) + " ";}
        s += "\n" + to_string(to.size()) + " to -> ";
        for (Node* t : to) {s += to_string(t->ID) + " ";}
        s += "\n" + to_string(from.size()) + " fo -> ";
        for (Node* f : from) {s += to_string(f->ID) + " ";}
        printf("%s\n\n", s.c_str());
    }
    void liteprint() {
        string s = to_string(ID) + ": ";
        s += " " + to_string(to.size()) + " to -> ";
        for (Node* t : to) {s += to_string(t->ID) + " ";}
        s += " " + to_string(from.size()) + " fo -> ";
        for (Node* f : from) {s += to_string(f->ID) + " ";}
        s += "\t||| ";
        for (dbl d : p) {s += to_string(d).substr(0, 3) + " ";}
        printf("%s\n", s.c_str());
    }
    
    void applyforce() {
        dbl m;
        for (int i = 0; i < to.size(); i++) {
            m = magnitude(p, to[i]->p);
            for (int j = 0; j < a.size(); j++) {
                a[j] = ((p[j]-to[i]->p[j])/m)*impressionable(m);
            }
        }
        for (int i = 0; i < p.size(); i++) {p[i] += v[i];}
        for (int i = 0; i < p.size(); i++) {v[i] += a[i]-friction*v[i];}
    }
    void draw() {
        shape.setPosition(v2f(p[0]*100, p[1]*100));
        edges = vector<sf::RectangleShape>();
        for (int i = 0; i < to.size(); i++) {
            edges.push_back(sf::RectangleShape());
            edges[i].setSize(v2f(1.f, 40*magnitude(p, to[i]->p)));
            edges[i].setPosition(v2f(p[0]*100+radius, p[1]*100+radius));
            edges[i].setFillColor(shape.getFillColor());
            edges[i].setRotation(90+180*atan2(p[1]-to[i]->p[1], p[0]-to[i]->p[0])/M_PI);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(512, 512), "market");
    int lightness = 200;
    for (int i = 0; i < 20; i++) {
        new Node(uniform(5), uniform(5), sf::Color(rand()%lightness+lightness, rand()%lightness+lightness, rand()%lightness+lightness));
    }
    for (int i = 0; i < 40; i++) {
        all[rand()%all.size()]->addmutual(all[rand()%all.size()]);
    }
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {window.close();}
        }
        window.clear(sf::Color::Black);
        for (Node* node : all) {node->applyforce();}
        for (Node* node : all) {node->draw();}
        for (Node* node : all) {
            for (sf::RectangleShape r : node->edges) {window.draw(r);}
        }
        for (Node* node : all) {window.draw(node->shape);}
        for (Node* node : all) {
            for (int i = 0; i < node->v.size(); i++) {
                node->v[i] += uniform(-0.001, 0.001); 
            }
        }
        window.display();
        window.setFramerateLimit(60);
    }
    printf("\n");
}

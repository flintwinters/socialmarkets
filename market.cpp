#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdlib>
#include <string>
#include <array>
#include <random>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.hpp>

using namespace std;
#define bnum 2
typedef float dbl;
typedef array<dbl, bnum> belief;
typedef sf::Vector2f v2f;
class Node;
class Edge;
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
    belief c = merge(a, b, [](dbl n, dbl m) {return (dbl) pow(n - m, 2);});
    return sqrt(abs(fold(c, [](dbl n, dbl m)-> dbl {return n + m;})));
}
default_random_engine eng;
dbl uniform(dbl a, dbl b) {
    uniform_real_distribution<double> d(a, b);
    return d(eng);
}
float scale = 10;
float xshift = 256;
float yshift = 256;
float radius = 4;
vector<Node*> alln;
vector<Edge*> alle;
class Edge {
public:
    int ID;
    Node* from, *to;
    dbl comfy = 1;
    dbl bounce = 0.5;
    sf::RectangleShape fshape, tshape;
    Edge(Node* from, Node* to);
    dbl impressionable(dbl x);
    void draw();
};
class Node {
public:
    int ID;
    vector<Edge*> E;
    belief p, v, a = { };
    dbl mass = 0.1;
    sf::CircleShape shape;
    Node() {
        alln.push_back(this);
        ID = alln.size();
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::White);
    }
    Node(dbl x, dbl y, sf::Color c) : Node() {
        p[0] = x;
        p[1] = y;
        shape.setFillColor(c);
    }
    
    Edge* addedge(Node* n) {
        E.push_back(new Edge(this, n));
        return E[E.size()-1];
    }
    void print() {
        string s = to_string(ID) + ": ";
        for (dbl d : p) {s += to_string(d).substr(0, 3) + " ";}
        s += "\n" + to_string(E.size()) + " fo -> ";
        for (Edge* e : E) {s += to_string(e->ID) + " ";}
        printf("%s\n\n", s.c_str());
    }
    void liteprint() {
        string s = to_string(ID) + ": ";
        s += " " + to_string(E.size()) + " fo -> ";
        for (Edge* e : E) {s += to_string(e->ID) + " ";}
        s += "\t||| ";
        for (dbl d : p) {s += to_string(d).substr(0, 3) + " ";}
        printf("%s\n", s.c_str());
    }
    
    void applyforce() {
        dbl m;
        for (int i = 0; i < E.size(); i++) {
            m = magnitude(p, E[i]->to->p);
            for (int j = 0; j < a.size(); j++) {
                a[j] = ((p[j]-E[i]->to->p[j])/m)*E[i]->impressionable(m);
            }
        }
        for (int i = 0; i < p.size(); i++) {p[i] += v[i];}
        for (int i = 0; i < p.size(); i++) {v[i] += a[i]-v[i]/mass;}
    }
    void draw() {
        dbl r = mass*radius*scale/40;
        shape.setRadius(r);
        shape.setPosition(v2f(xshift+(p[0])*scale, yshift+(p[1])*scale));
    }
};
Edge::Edge(Node* from, Node* to) : from(from), to(to) {
    alle.push_back(this);
    ID = alle.size();
}
dbl Edge::impressionable(dbl x) {
    comfy -= .1*(comfy-x);
    return -bounce*(x-comfy)/10;
}
void Edge::draw() {
    dbl m = magnitude(from->p, to->p);
    dbl w = ((0.5/bounce)+0.5)*scale/20;
    dbl l = m*((50/comfy))*scale/10;
    fshape.setSize(v2f(w, l));
    fshape.setPosition(v2f(xshift+from->p[0]*scale+from->shape.getRadius(), yshift+from->p[1]*scale+from->shape.getRadius()));
    fshape.setFillColor(from->shape.getFillColor());
    fshape.setRotation(90+180*atan2(from->p[1]-to->p[1], from->p[0]-to->p[0])/M_PI);

    tshape.setSize(v2f(w, l));
    tshape.setPosition(v2f(xshift+to->p[0]*scale+to->shape.getRadius(), yshift+to->p[1]*scale+to->shape.getRadius()));
    tshape.setFillColor(to->shape.getFillColor());
    tshape.setRotation(-90+180*atan2(from->p[1]-to->p[1], from->p[0]-to->p[0])/M_PI);
}

int main() {
    for (int i = 0; i < 10; i++) {
        Node* n = new Node(uniform(-1, 1), uniform(-1, 1), sf::Color(rand()%255, rand()%255, rand()%255));
        n->mass = uniform(1, 2);
    }
    for (int i = 0; i < 10; i++) {
        int a = rand()%alln.size();
        int b;
        while (a == (b = rand()%alln.size()));
        Edge* e = alln[a]->addedge(alln[b]);
        e->bounce = uniform(0.1, 3);
        e->comfy = uniform(0.1, 3);
    }
    for (int i = 0; i < 100000; i++) {
        for (Node* node : alln) {node->applyforce();}
        for (Node* node : alln) {
            for (int i = 0; i < node->v.size(); i++) {
                node->v[i] += uniform(-0.001, 0.001);
            }
        }
    }
    sf::RenderWindow window(sf::VideoMode(512, 512), "market");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {window.close();}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {scale *= 1.1;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {scale /= 1.1;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {yshift += 10;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {xshift += 10;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {yshift -= 10;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {xshift -= 10;}
        }
        window.clear(sf::Color::Black);
        for (int i = 0; i < 1; i++) {
            for (Node* node : alln) {node->applyforce();}
            for (Node* node : alln) {
                for (int i = 0; i < node->v.size(); i++) {
                    node->v[i] += uniform(-0.001, 0.001);
                }
            }
        }
        for (Node* node : alln) {node->draw();}
        for (Edge* e : alle) {e->draw();}
        for (Node* node : alln) {
            for (Edge* e : alle) {
                window.draw(e->fshape);
                window.draw(e->tshape);
            }
        }
        for (Node* node : alln) {window.draw(node->shape);}
        window.display();
        window.setFramerateLimit(60);
    }
}

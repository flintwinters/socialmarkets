#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
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
class Physics;
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
float radius = 4;
vector<Node*> alln;
vector<Edge*> alle;
class Physics {
public:
    dbl p, v, a;
    dbl epsilon, beta;
    Physics(dbl epsilon = 1, dbl beta = 0): p(0), v(0), a(0), epsilon(epsilon), beta(beta){}
    void update(dbl f=0) {p += v; v += a; if (abs(v) < epsilon+beta) {v = 0;} else if (v < 0) {v += f;} else {v -= f;}}
    void print() {
        printf("%f %f %f : %f %f", p, v, a, epsilon, beta);
    }
};
Physics X, Y, scale(0.01);
class Edge {
public:
    int ID;
    Node* from, *to;
    dbl comfy = 1;
    dbl bounce = 0.5;
    dbl flex = 0.5;
    sf::ConvexShape fshape, tshape;
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
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(radius/4);
        // shape.setFillColor(sf::Color::White);
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
        dbl r = mass*radius*scale.p/200;
        shape.setRadius(r);
        shape.setOutlineThickness(r/4);
        shape.setPosition(v2f(X.p+(p[0])*scale.p, Y.p+(p[1])*scale.p));
    }
};
Edge::Edge(Node* from, Node* to) : from(from), to(to) {
    alle.push_back(this);
    ID = alle.size();
    fshape.setPointCount(3);
    tshape.setPointCount(3);
}
dbl Edge::impressionable(dbl x) {
    comfy -= flex*(comfy-x);
    return -bounce*(x-comfy)/10;
}
void Edge::draw() {
    dbl m = magnitude(from->p, to->p);
    dbl w = ((0.5/bounce))*scale.p/70+1;
    dbl l = comfy*scale.p/3;
    dbl fr = from->shape.getRadius();
    dbl tr = to->shape.getRadius();
    dbl a = 180*atan2(from->p[1]-to->p[1], from->p[0]-to->p[0])/M_PI;
    fshape.setPosition(v2f(X.p+from->p[0]*scale.p+fr, Y.p+from->p[1]*scale.p+fr));
    // fshape.setPoint(0, v2f(0, 0));
    fshape.setPoint(0, v2f(0, w));
    fshape.setPoint(1, v2f(l, 0));
    fshape.setPoint(2, v2f(0, -w));
    sf::Color c = from->shape.getFillColor();
    c.a = 10;
    fshape.setFillColor(c);
    fshape.setRotation(180+a);

    tshape.setPosition(v2f(X.p+to->p[0]*scale.p+tr, Y.p+to->p[1]*scale.p+tr));
    // tshape.setPoint(0, v2f(0, 0));
    tshape.setPoint(0, v2f(0, w));
    tshape.setPoint(1, v2f(l, 0));
    tshape.setPoint(2, v2f(0, -w));
    c = to->shape.getFillColor();
    c.a = 10;
    tshape.setFillColor(c);
    tshape.setRotation(a);
}

int main() {
    int lightness = 220;
    for (int i = 0; i < 30; i++) {
        Node* n = new Node(uniform(-1, 1), uniform(-1, 1), sf::Color(rand()%lightness+lightness, rand()%lightness+lightness, rand()%lightness+lightness));
        n->mass = uniform(1, 2);
    }
    for (int i = 0; i < 40; i++) {
        int a = rand()%alln.size();
        int b;
        while (a == (b = rand()%alln.size()));
        Edge* e = alln[a]->addedge(alln[b]);
        e->bounce = uniform(0.1, 3);
        e->comfy = uniform(0.1, 3);
        e->flex = uniform(0.1, 0.6);
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
    scale.p = 200;
    X.p = Y.p = 256;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {window.close();}
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up:    scale.a = +0.04; break;
                    case sf::Keyboard::Down:  scale.a = -0.04; break;
                    case sf::Keyboard::W: Y.a = -1; break;
                    case sf::Keyboard::A: X.a = -1; break;
                    case sf::Keyboard::S: Y.a = +1; break;
                    case sf::Keyboard::D: X.a = +1; break;
                    default: break;
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    case sf::Keyboard::Up:    scale.a = 0; break;
                    case sf::Keyboard::Down:  scale.a = 0; break;
                    case sf::Keyboard::W: Y.a = 0; break;
                    case sf::Keyboard::A: X.a = 0; break;
                    case sf::Keyboard::S: Y.a = 0; break;
                    case sf::Keyboard::D: X.a = 0; break;
                    default: break;
                }
            }
        }
        X.update(0.2); Y.update(0.2);
        scale.update(0.01);
        window.clear(sf::Color::Black);
        for (int i = 0; i < 1; i++) {
            for (Node* node : alln) {node->applyforce();}
            for (Node* node : alln) {
                for (int i = 0; i < node->v.size(); i++) {
                    node->v[i] += uniform(-0.002, 0.002);
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

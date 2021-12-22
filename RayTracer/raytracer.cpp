#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

#define EPSILON 0.0000001

/* generics */
template<typename T>
struct Vec3 {
  T x, y, z;

  /* contructors */
  /* se não for passado nada para o construtor, inicializa tudo como 0 */
  Vec3() {
    this->x = T(0);
    this->y = T(0);
    this->z = T(0);
  };

  /* se for passado somente um número w ao construtor, inicializa tudo como w */
  Vec3(T w) {
    this->x = w;
    this->y = w;
    this->z = w;
  };

  /* se forem dados os três valores, incializa como os valores */
  Vec3(T x, T y, T z) {
    this->x = x;
    this->y = y;
    this->z = z;
  };

  /* methods */
  /* calcula o comprimento do vetor */
  T length() const { 
    const T length2 = x * x + y * y + z * z; 
    return sqrt(length2);
  };
  
  /* normaliza o vetor -> comprimento = 1 */
  Vec3& normalize(){
    const T length = length();

    if(length > 0){
      x = x/length;
      y = y/length;
      z = z/length;
    };

    return *this;
  };

  /* vetor + vetor */
  Vec3<T> operator + (const Vec3<T> &v) const {
    return Vec3<T>(x + v.x, y + v.y, z + v.z);
  };

  Vec3<T>& operator += (const Vec3<T> &v) {
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
  };

  /* vetor - vetor */
  Vec3<T> operator - (const Vec3<T> &v) const {
    return Vec3<T>(x - v.x, y - v.y, z - v.z);
  }

  /* vetor * número */
  Vec3<T> operator * (const T &f) const {
    return Vec3<T>(x * f, y * f, z * f);
  };
  
  /* vetor * vetor */
  Vec3<T> operator * (const Vec3<T> &v) const {
    return Vec3<T>(x * v.x, y * v.y, z * v.z);
  };


  Vec3<T>& operator *= (const Vec3<T> &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this; 
  } 

  Vec3<T> operator - () const {
    return Vec3<T>(-x, -y, -z);
  };

  /* produto escalar */
  T dot(const Vec3<T> &v) const {
    return x * v.x + y * v.y + z * v.z;
  };

  /* produto vetorial */
  Vec3<T> cross(const Vec3<T> &v) const {
    Vec3<T> r;
    r.x = y * v.z - z * v.y;
    r.y = z * v.x - x * v.z;
    r.z = x * v.y - y * v.x;

    return r;
  };

  /* retorna o vetor printado */
  friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v) { 
    os << "[" << v.x << " " << v.y << " " << v.z << "]"; 
    return os; 
  };
};


typedef Vec3<float> Vec3f; 
 
struct Sphere { 
  Vec3f center;
  float radius, radius2;
  Vec3f surfaceColor, emissionColor;
  float transparency, reflection;

  /* constructor */
  Sphere(const Vec3f &c, const float &r, const Vec3f &sc, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0) {
    this->center = c;
    this->radius = r;
    this->radius2 = r * r;
    this->surfaceColor = sc;
    this->emissionColor = ec;
    this->transparency = transp;
    this->reflection = refl;
  };

  bool intersect(const Vec3f &rayOrig, const Vec3f &rayDir, float &t0, float &t1) const {
    /* l vai ser o vetor que aponta da origem do raio de luz para o centro */
    Vec3f l = center - rayOrig;
    /* tca vai ser o produto escalar de l com a direção do raio */
    float tca = l.dot(rayDir);

    /* se tca for igual a 0, quer dizer que l é perpendicular à direção do raio, logo não intersecta */
    /* se tca for menor a 0, quer dizer que l é obtuso à direção do raio, logo, tem componentes x e y, 
    porém em sentindo contrário à esfera, logo, não intersecta */
    if (tca < 0) {
      return false;
    }

    /* fórmula para descobrir se um ponto pertence a uma esfera ->||p(t) - c||² = r² */
    /* l.dot(l) calcula ||l||² e tca * tca calcula o comprimento */
    float d2 = l.dot(l) - tca * tca; 
    
    /* se d > r, o raio n intersecta a esfera */
    if (d2 > radius2) {
      return false;
    }

    /* caso contratrio, intersecta, podendo ter 1 ou 2 pontos de intersecção */
    float thc = sqrt(radius2 - d2); 
    t0 = tca - thc; 
    t1 = tca + thc; 
 
    return true; 
  };
};


struct Triangle { 
  Vec3f vertex1, vertex2, vertex3;
  Vec3f surfaceColor, emissionColor;
  float transparency, reflection;

  /* constructor */
  Triangle(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3, const Vec3f &sc, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0) {
    this->vertex1 = v1;
    this->vertex2 = v2;
    this->vertex3 = v3;
    this->surfaceColor = sc;
    this->emissionColor = ec;
    this->transparency = transp;
    this->reflection = refl;
  };

  /*  Möller–Trumbore ray-triangle intersection algorithm */
  bool intersect(Vec3f rayOrig, Vec3f rayDir, Triangle* inTriangle, Vec3f& outIntersectionPoint) {
    Vec3f vertex1 = inTriangle->vertex1;
    Vec3f vertex2 = inTriangle->vertex2;  
    Vec3f vertex3 = inTriangle->vertex3;
    
    Vec3f edge1, edge2, h, s, q;

    float a,f,u,v;

    edge1 = vertex2 - vertex1;
    edge2 = vertex3 - vertex2;

    h = rayDir.cross(edge2);
    a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON) {
      return false;    // This ray is parallel to the triangle
    };

    f = 1.0/a;
    s = rayOrig - vertex1;
    u = f * s.dot(h);

    if (u < 0.0 || u > 1.0) {
      return false;
    };

    q = s.cross(edge1);
    v = f * rayDir.dot(q);

    if (v < 0.0 || u + v > 1.0){
      return false;
    };
    
    // At this stage we can compute t to find out where the intersection point is on the line
    float t = f * edge2.dot(q);

    if (t > EPSILON){ // ray intersection
      outIntersectionPoint = rayOrig + rayDir * t;
      return true;
    } else {  // This means that there is a line intersection but not a ray intersection
      return false;
    };
  };
}; 


struct Plane {
  Vec3f dot;
  Vec3f normal;
  Vec3f surfaceColor, emissionColor;
  float transparency, reflection;

  /* constructor */
  Plane(const Vec3f &d, const Vec3f &n, const Vec3f &sc, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0) {
    this->dot = d;
    this->normal = n;
    this->surfaceColor = sc;
    this->emissionColor = ec;
    this->transparency = transp;
    this->reflection = refl;
  };

  bool intersect(const Vec3f &n, const Vec3f &p0, const Vec3f &rayOrig, const Vec3f &rayDir, float &t) { 
    // assuming vectors are all normalized
    float denom = n.dot(rayDir);
    // se n ⋅ rayDir igual 0, logo a normal do plano e a direção do raio são perpendiculares, logo não há interseção
    // se for menor, o raio é contrário, logo não aparecerá interseção
    if (denom > EPSILON) { 
        Vec3f p0rayOrig = p0 - rayOrig; 
        t = p0rayOrig.dot(n) / denom; 
        return (t >= 0); 
    };
 
    return false; 
  };
};


#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

/* generics */
template<typename T>
struct Vec3D {
  T x, y, z;

  /* contructors */
  /* se não for passado nada para o construtor, inicializa tudo como 0 */
  Vec3D() {
    this->x = T(0);
    this->y = T(0);
    this->z = T(0);
  };

  /* se for passado somente um número w ao construtor, inicializa tudo como w */
  Vec3D(T w) {
    this->x = w;
    this->y = w;
    this->z = w;
  };

  /* se forem dados os três valores, incializa como os valores */
  Vec3D(T x, T y, T z) {
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
  Vec3D& normalize(){
    const T length = length();

    if(length > 0){
      x = x/length;
      y = y/length;
      z = z/length;
    };

    return *this;
  };

  /* vetor + vetor */
  Vec3D<T> operator + (const Vec3D<T> &v) const {
    return Vec3D<T>(x + v.x, y + v.y, z + v.z);
  };

  Vec3D<T>& operator += (const Vec3D<T> &v) {
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
  };

  /* vetor - vetor */
  Vec3D<T> operator - (const Vec3D<T> &v) const {
    return Vec3D<T>(x - v.x, y - v.y, z - v.z);
  }

  /* vetor * número */
  Vec3D<T> operator * (const T &f) const {
    return Vec3D<T>(x * f, y * f, z * f);
  };
  
  /* vetor * vetor */
  Vec3D<T> operator * (const Vec3D<T> &v) const {
    return Vec3D<T>(x * v.x, y * v.y, z * v.z);
  };


  Vec3D<T>& operator *= (const Vec3D<T> &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this; 
  } 

  Vec3D<T> operator - () const {
    return Vec3D<T>(-x, -y, -z);
  };

  /* produto escalar */
  T dot(const Vec3D<T> &v) const {
    return x * v.x + y * v.y + z * v.z;
  };

  /* produto vetorial */
  Vec3D<T> cross(const Vec3D<T> &v) const {
    Vec3D<T> r;
    r.x = y * v.z - z * v.y;
    r.y = z * v.x - x * v.z;
    r.z = x * v.y - y * v.x;

    return r;
  }

  /* retorna o vetor printado */
  friend std::ostream & operator << (std::ostream &os, const Vec3D<T> &v) { 
    os << "[" << v.x << " " << v.y << " " << v.z << "]"; 
    return os; 
  };
};


typedef Vec3D<float> Vec3Df; 
 
struct Sphere { 
  Vec3Df center;
  float radius, radius2;
  Vec3Df surfaceColor, emissionColor;
  float transparency, reflection;

  /* constructor */
  Sphere(const Vec3Df &c, const float &r, const Vec3Df &sc, const float &refl = 0, const float &transp = 0, const Vec3Df &ec = 0) {
    this->center = c;
    this->radius = r;
    this->radius2 = r * r;
    this->surfaceColor = sc;
    this->emissionColor = ec;
    this->transparency = transp;
    this->reflection = refl;
  }

  bool intersect(const Vec3Df &rayOrig, const Vec3Df &rayDir, float &t0, float &t1) const {
    /* l vai ser o vetor que aponta da origem do raio de luz para o centro */
    Vec3Df l = center - rayOrig;
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
  Vec3Df vertex1, vertex2, vertex3;
  Vec3Df surfaceColor, emissionColor;
  float transparency, reflection;

  /* constructor */
  Triangle(const Vec3Df &v1, const Vec3Df &v2, const Vec3Df &v3, const Vec3Df &sc, const float &refl = 0, const float &transp = 0, const Vec3Df &ec = 0) {
    this->vertex1 = v1;
    this->vertex2 = v2;
    this->vertex3 = v3;
    this->surfaceColor = sc;
    this->emissionColor = ec;
    this->transparency = transp;
    this->reflection = refl;
  }

  bool intersect(Vec3Df rayOrig, Vec3Df rayDir, Triangle* inTriangle, Vec3Df& outIntersectionPoint) {
    const float EPSILON = 0.0000001;

    Vec3Df vertex1 = inTriangle->vertex1;
    Vec3Df vertex2 = inTriangle->vertex2;  
    Vec3Df vertex3 = inTriangle->vertex3;
    
    Vec3Df edge1, edge2, h, s, q;

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
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * edge2.dot(q);

    if (t > EPSILON){ // ray intersection
      outIntersectionPoint = rayOrig + rayDir * t;
      return true;
    } else {  // This means that there is a line intersection but not a ray intersection
      return false;
    }
  }
}; 
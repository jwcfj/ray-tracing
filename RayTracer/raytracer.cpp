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

  T dot(const Vec3D<T> &v) const {
    return x * v.x + y * v.y + z * v.z;
  } 
  
  /* retorna o vetor printado */
  friend std::ostream & operator << (std::ostream &os, const Vec3D<T> &v) { 
    os << "[" << v.x << " " << v.y << " " << v.z << "]"; 
    return os; 
  } 
};
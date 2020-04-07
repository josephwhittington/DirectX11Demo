#define D3DRELEASE(pointer) if((pointer)) (pointer)->Release()

#define NULLIFYPTR(pointer) (pointer) = nullptr
#define RELEASEPTR(pointer) if ((pointer)) delete pointer

#define RELEASEARRAY(arrpointer) if((arrpointer)) delete[] arrpointer

#define WMATH_PI 3.1415926
#define DEG2RADS(deg) ((deg) * (3.1415926)/(180))
#define RADS2DEGS(rads) ((rads) * (180)/(3.1415926))
#ifndef TIPSY_H
#define TIPSY_H

#define DIM 3
typedef float Real;

// Units from runtime/README:
// - distance: 1 kpc
// - velocity: 100 km/s
// - mass: 2.324876e9 Msun
// - time: 9.778145 Myr

struct head {

    head(double time = 0.0, int nbodies = 0, int ndim = 0, int nsph = 0, int ndark = 0, int nstar = 0)
     : time(time), nbodies(nbodies), ndim(ndim), nsph(nsph), ndark(ndark), nstar(nstar)
    {}

    head(head const& other)
     : time(other.time), nbodies(other.nbodies), ndim(other.ndim), nsph(other.nsph), ndark(other.ndark), nstar(other.nstar)
    {}

    double time;
    int nbodies;
    int ndim;
    int nsph;
    int ndark;
    int nstar;
};

struct dark_particle
{
    dark_particle(float mass = 0.0, float posx = 0.0, float posy = 0.0, float posz = 0.0,
        float velx = 0.0, float vely = 0.0, float velz = 0.0, float eps = 0.0, int phi = 0)
      : mass(mass), eps(eps), phi(phi)
    {
        pos[0] = posx;
        pos[1] = posy;
        pos[2] = posz;
        vel[0] = velx;
        vel[1] = vely;
        vel[2] = velz;
    }

    float mass;
    float pos[DIM];
    float vel[DIM];
    float eps;
    int phi;  // FileIO.cpp: store particle id where potential was stored
};

struct star_particle {

    star_particle(float mass = 0.0, float posx = 0.0, float posy = 0.0, float posz = 0.0,
        float velx = 0.0, float vely = 0.0, float velz = 0.0, float metals = 0.0,
        float tform = 0.0, float eps = 0.0, int phi = 0,
        float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0)
        : mass(mass), pos(), vel(), metals(metals), tform(tform), eps(eps), phi(phi), rgba()
    {
        pos[0] = posx;
        pos[1] = posy;
        pos[2] = posz;
        vel[0] = velx;
        vel[1] = vely;
        vel[2] = velz;
        rgba[0] = r;
        rgba[1] = g;
        rgba[2] = b;
        rgba[3] = a;
    }

    float mass;
    float pos[DIM];
    float vel[DIM];
    float metals;
    float tform;
    float eps;
    int phi;  // FileIO.cpp: store particle id where potential was stored
    float rgba[4];
};

#endif // TIPSY_H

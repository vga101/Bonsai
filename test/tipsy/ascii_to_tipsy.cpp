#include <algorithm>  // std::max
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "tipsy.h"

const int INDEX_D_START = 200000000;
const int INDEX_S_START = 100000000;

// unit definitions from runtime/README
// Msun, km/s and kpc in internal code units
const float MSUN = 2.324876e9f;
const float KMS = 100.0f;
const float KPC = 1.0f;


int main(int argc, char* argv[])
{
    std::vector<dark_particle> vd{};
    std::vector<star_particle> vs{};

    // VG added
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " infile.ascii outfile.tipsy" << std::endl;
        return 1;
    }

    std::cout << "Infile: " << argv[1] << std::endl;
    std::cout << "Outfile: " << argv[2] << std::endl;

    std::string inname(argv[1]);
    std::string outname(argv[2]);

    std::ifstream infile(inname);
    std::string line;

    //std::vector<dark_particle> vd;
    //std::vector<star_particle> vs;

    int id_d = INDEX_D_START;   // index identifier for dark matter
    int id_s = INDEX_S_START;   // index identifier for stars

    float m_s = 0.0f;
    float m_d = 0.0f;

    while(std::getline(infile, line)) {

        if (line.at(0) == '#') {
            // ignore comment lines
            std::cout << "#" << std::endl;

        } else {
            // parse data line

            std::stringstream linestream(line);
            float posx, posy, posz, velx, vely, velz, mass, L_B, L_G, L_R, flag;
            int idx;
            linestream >> posx >> posy >> posz >> velx >> vely >> velz >> mass >> L_B >> L_G >> L_R >> flag;
            // ASCII file:
            // pos[xyz] in kpc, vel[xyz] in km/s, mass in Msun, L_[BGR] in erg/s

            if (flag == 1) {
                // dark matter

                std::cout << "dm particle " << id_d << " with mass " << mass << std::endl;
                dark_particle d(mass/MSUN,
                                posx/KPC, posy/KPC, posz/KPC,
                                velx/KMS, vely/KMS, velz/KMS,
                                0.0f, 
                                id_d);
                m_d += mass/MSUN;
                vd.push_back(d);  // add to end of vector

                id_d += 1;

            } else if (flag == 4) {
                // star

                // test hack:
                float L_max = std::max(std::max(L_B, L_G), L_R);
                std::cout << "star particle " << id_s << " with mass " << mass << std::endl;
                star_particle s(mass/MSUN, 
                                posx/KPC, posy/KPC, posz/KPC,
                                velx/KMS, vely/KMS, velz/KMS,
                                0.0f, 0.0f, 0.0f,
                                id_s,
                                L_R/L_max, L_G/L_max, L_B/L_max, 1.0   // rgba 0...1
                               );
                m_s += mass/MSUN;
                vs.push_back(s);  // add to end of vector

                id_s += 1;
                if (id_s >= INDEX_D_START) {
                    std::cout << "Error: too many star particles. Overlapping indices." << std::endl;
                    return 1;
                }

            } else {
                // not defined
                std::cout << "Error: flag " << flag << " is not defined." << std::endl;
                return 1;
            }
        }
    }
    std::cout << "Added " << (id_d-INDEX_D_START) << " dark matter particles, total mass: " << m_d << std::endl;
    std::cout << "Added " << (id_s-INDEX_S_START) << " star particles, total mass: " << m_s << std::endl;
    std::cout << "Total mass (dm + stars): " << m_d + m_s << std::endl;


//    std::vector<dark_particle> vd{{0.00443292, -0.0120859, -0.0410267, -0.00437124, -1.605, -0.643298, -0.367065, 0.206867, 200000000},
//                                  {0.00443292, -0.0558513, -0.0209438, -0.0878785, 0.262374, -0.872229, -0.156694, 0.206867, 200000001},
//                                  {0.00443292, 0.0447762, -0.0448422, -0.0366114, -0.526952, -0.395124, 1.78094, 0.206867, 200000002},
//                                  {0.00443292, -0.0381424, -0.0317452, 0.0489303, -1.76929, 0.513822, -1.72173, 0.206867, 200000003},
//                                  {0.00443292, 0.158146, 0.0222634, 0.128441, 0.634014, -0.497379, -0.336336, 0.206867, 200000004},
//                                  {0.00443292, 0.188687, -0.111929, 0.0948612, -0.107827, 0.4779, -0.517985, 0.206867, 200000005},
//                                  {0.00443292, 0.179841, -0.142335, 0.0327414, -0.371021, -0.779585, -0.406808, 0.206867, 200000006},
//                                  {0.00443292, -0.0337725, 0.203328, -0.00501226, 0.0512849, 1.55558, -0.287626, 0.206867, 200000007}};
//
//    std::vector<star_particle> vs{{0.00430987, -0.0269203, 0.0215147, 0.00322919, -0.0424188, 0.809628, -0.663159, 0, 0, 0.204936, 100000000},
//                                  {0.00430987, 0.0070073, 0.0232225, 0.0373815, 0.783409, 1.0458, -0.00211907, 0, 0, 0.204936, 100000001},
//                                  {0.00430987, -0.0425167, 0.0320775, 0.0427321, 0.280932, -0.194072, 0.548338, 0, 0, 0.204936, 100000002},
//                                  {0.00430987, -0.0279896, 0.00917036, 0.0592467, -0.603823, -0.182352, -0.42594, 0, 0, 0.204936, 100000003},
//                                  {0.00430987, -0.01737, 0.0088137, 0.0705704, -0.320545, -0.58951, 0.346465, 0, 0, 0.204936, 100000004},
//                                  {0.00430987, -0.0247747, -0.0107097, -0.0589867, 0.0475681, 0.954196, -0.617804, 0, 0, 0.204936, 100000005},
//                                  {0.00430987, -0.0263219, -0.0303222, -0.0564402, -0.191889, 0.38757, 0.91078, 0, 0, 0.204936, 100000006},
//                                  {0.00430987, 0.0345237, -0.0248301, 0.0205758, 1.2972, 0.89203, 0.472138, 0, 0, 0.204936, 100000007}};

//    std::vector<dark_particle> vd{{0.0, 1000.0, 1000.0, 1000.0, 0.0, 0.0, 0.0, 0.0, 200000000},
//                                  {0.0, 1000.0, 1000.0, 1001.0, 0.0, 0.0, 0.0, 0.0, 200000001},
//                                  {0.0, 1000.0, 1000.0, 1002.0, 0.0, 0.0, 0.0, 0.0, 200000002},
//                                  {0.0, 1000.0, 1000.0, 1003.0, 0.0, 0.0, 0.0, 0.0, 200000003}};
//
//    std::vector<star_particle> vs{{0.0, 1000.0, 1001.0, 1000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000000},
//                                  {0.0, 1000.0, 1001.0, 1001.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000001},
//                                  {0.0, 1000.0, 1001.0, 1002.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000002},
//                                  {0.0, 1000.0, 1001.0, 1003.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100000003}};

    head h(0.0, vd.size() + vs.size(), DIM, 0, vd.size(), vs.size());

    //std::ofstream os("test.tipsy", std::ios::binary);
    std::ofstream os(outname, std::ios::binary);
    os.write((char*) &h, sizeof(h));

    for (auto const& d : vd) os.write((char*) &d, sizeof(d));
    for (auto const& s : vs) os.write((char*) &s, sizeof(s));
}

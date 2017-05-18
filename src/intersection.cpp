#include "intersection.h"
#include "utils.h"

namespace jtrace {
    Intersection::Intersection(const double _t, const Vec3 _point, const Vec3 _surfaceNormal) :
        t(_t), point(_point), surfaceNormal(_surfaceNormal.UnitVec3()) {}

    Ray Intersection::reflectedRay(const Ray& r) const {
        double n = 1.0 / r.v.Magnitude();
        Vec3 nv = r.v * n;
        double c1 = DotProduct(nv, surfaceNormal);
        return Ray(point, (nv - 2*c1*surfaceNormal).UnitVec3()/n, t, r.wavelength, r.isVignetted);
    }

    // Reflect lots of rays at single intersection
    std::vector<Ray> Intersection::reflectedRay(const std::vector<Ray>& rays) const {
        auto result = std::vector<Ray>();
        result.reserve(rays.size());
        for (const auto& ray : rays) {
            result.push_back(reflectedRay(ray));
        }
        return result;
    }

    Ray Intersection::refractedRay(const Ray& r, double n1, double n2) const {
        //assert n1 == 1./r.Magnitude()
        Vec3 nv = r.v * n1;
        double alpha = DotProduct(nv, surfaceNormal);
        double a = 1.;
        double b = 2*alpha;
        double c = (1. - (n2*n2)/(n1*n1));
        double k1, k2;
        solveQuadratic(a, b, c, k1, k2);
        Vec3 f1 = (nv+k1*surfaceNormal).UnitVec3();
        Vec3 f2 = (nv+k2*surfaceNormal).UnitVec3();
        if (DotProduct(f1, nv) > DotProduct(f2, nv))
            return Ray(point, f1/n2, t, r.wavelength, r.isVignetted);
        else
            return Ray(point, f2/n2, t, r.wavelength, r.isVignetted);
    }

    std::vector<Ray> Intersection::refractedRay(const std::vector<Ray>& rays, double n1, double n2) const {
        auto result = std::vector<Ray>();
        result.reserve(rays.size());
        for (const auto& ray : rays) {
            result.push_back(refractedRay(ray, n1, n2));
        }
        return result;
    }

    Ray Intersection::refractedRay(const Ray& r, const Medium& m1, const Medium& m2) const {
        double n1 = m1.getN(r.wavelength);
        double n2 = m2.getN(r.wavelength);
        return refractedRay(r, n1, n2);
    }

    std::vector<Ray> Intersection::refractedRay(const std::vector<Ray>& rays, const Medium& m1, const Medium& m2) const {
        auto result = std::vector<Ray>();
        result.reserve(rays.size());
        for (const auto& ray : rays) {
            result.push_back(refractedRay(ray, m1, m2));
        }
        return result;
    }

    std::string Intersection::repr() const {
        std::ostringstream oss(" ");
        oss << "Intersection(" << t << ", " << point << ", " << surfaceNormal << ")";
        return oss.str();
    }

    // Reflect lots of rays at lots of different intersections
    std::vector<Ray> reflectMany(const std::vector<Intersection>& isecs, const std::vector<Ray>& rays) {
        auto result = std::vector<Ray>();
        result.reserve(isecs.size());
        for (unsigned i=0; i<isecs.size(); i++) {
            result.push_back(isecs[i].reflectedRay(rays[i]));
        }
        return result;
    }

    // Refract lots of rays at lots of different intersections
    std::vector<Ray> refractMany(const std::vector<Intersection>& isecs, const std::vector<Ray>& rays, double n1, double n2) {
        auto result = std::vector<Ray>();
        result.reserve(isecs.size());
        for (unsigned i=0; i<isecs.size(); i++) {
            result.push_back(isecs[i].refractedRay(rays[i], n1, n2));
        }
        return result;
    }

    std::vector<Ray> refractMany(const std::vector<Intersection>& isecs, const std::vector<Ray>& rays, const Medium& m1, const Medium& m2) {
        auto result = std::vector<Ray>();
        result.reserve(isecs.size());
        for (unsigned i=0; i<isecs.size(); i++) {
            result.push_back(isecs[i].refractedRay(rays[i], m1, m2));
        }
        return result;
    }
}

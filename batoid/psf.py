import numpy as np
import batoid

def huygensPSF(optic, xs=None, ys=None, zs=None, rays=None):
    if zs is None:
        zs = np.zeros_like(xs)
    rays, outCoordSys = optic.trace(rays)
    # transform = batoid.CoordTransform(outCoordSys, batoid.CoordSys())
    # rays = transform.applyForward(rays)
    rays = batoid.trimVignetted(rays)
    points = np.concatenate([aux[..., None] for aux in (xs, ys, zs)], axis=-1)
    time = rays[0].t0
    amplitudes = np.empty(xs.shape, dtype=np.complex128)
    for (i, j) in np.ndindex(xs.shape):
        amplitudes[i, j] = np.sum(
            batoid._batoid.amplitudeMany(
                rays,
                batoid.Vec3(*points[i, j]),
                time
            )
        )
    return np.abs(amplitudes)**2